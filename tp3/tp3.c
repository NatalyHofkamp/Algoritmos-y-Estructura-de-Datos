#include "tp3.h"
#include <string.h>
#include <stdlib.h>

typedef struct bucket bucket_t;

struct bucket {
    const void *key;
    void *value;
    unsigned long long int bitmap;
} __attribute__ ((aligned (8)));


struct dictionary {
    size_t exponent;
    size_t bitmap_len;
    bucket_t*buckets;
    destroy_f destroy;
    size_t full_buckets;
    
};

static __inline__ uint32_t _jenkins_hash(const char *key, size_t len){
    uint32_t hash;
    size_t i;
    hash = 0;
    for ( i = 0; i < len; i++ ) {
        hash += key[i];
        hash += (hash << 10);
        hash ^= (hash >> 6);
    }
    hash += (hash << 3);
    hash ^= (hash >> 11);
    hash += (hash << 15);

    return hash;
}

dictionary_t *dictionary_create(destroy_f destroy) { 
    dictionary_t* new_dict = (dictionary_t*) malloc (sizeof(dictionary_t));
    if(!new_dict) return NULL;

    new_dict->exponent = 10;
    new_dict->bitmap_len = 32; 
    new_dict->full_buckets = 0;
    if(destroy) new_dict->destroy = destroy;
    else new_dict->destroy = free;

    new_dict->buckets = (bucket_t*) malloc (sizeof(bucket_t)*(1<<new_dict->exponent));
    if(!new_dict->buckets) return NULL;

    memset(new_dict->buckets,0,sizeof(bucket_t)*(1<<new_dict->exponent));
    return new_dict;
 };

dictionary_t* rehash(dictionary_t* dictionary) {
  dictionary_t* new_dict = dictionary_create(dictionary->destroy);
  new_dict->exponent = dictionary->exponent + 2;
  new_dict->full_buckets = dictionary->full_buckets;
  size_t size = 1ULL << new_dict->exponent;
  memset(new_dict->buckets, 0, sizeof(bucket_t) * size);
  size_t i = 0;
  while (i < size) {
    if (dictionary->buckets[i].key) {
      dictionary_put(new_dict, dictionary->buckets[i].key, dictionary->buckets[i].value);
    }
    ++i;
  }
  return new_dict;
}

size_t index_search (dictionary_t* dictionary, const char* key){
  size_t size = 1ULL << dictionary->exponent;
  uint32_t hs = _jenkins_hash(key, strlen(key));
  size_t index = hs & (size - 1); //mete el hash dentro del rango del tamaño de la tabla
  for (size_t i = index; i < (1ULL << dictionary->exponent); i++) {
    if (dictionary->buckets[i].key == key) {
      return index; // Se encontró el elemento en el bucket actual
    }
    size_t j = 1;
    while (j < dictionary->bitmap_len) {
      size_t neighbor_index = (i + j) % size; // Índice del vecino considerando el tamaño de la tabla
      if (dictionary->buckets[neighbor_index].key== key) {
        return neighbor_index; // Se encontró el elemento en un vecino
      }
      j++;
    }
  }
  return false; // No se encontró el elemento en la tabla
}


bool dictionary_put(dictionary_t *dictionary, const char *key, void *value) {
  size_t size= 1ULL << dictionary->exponent;
  uint32_t hs = _jenkins_hash(key, strlen(key));
  size_t index = hs & (size-1); //hash dentro del rango del tamaño de la tabla
  if(dictionary_contains(dictionary,key)){
    dictionary->buckets[index_search(dictionary,key)].value = value;
  }
 for (size_t i = index; i < (1ULL << dictionary->exponent); i++) {
    if (!dictionary->buckets[i].key) { // si está vacío igual lo guardamos en algún vecino (evitar colisiones)
      size_t j = 1;
      while ((i + j) < (1ULL << dictionary->exponent)) {
        if (!dictionary->buckets[i + j].key) { // encontró un vecino vacío
          index += j;
          break;
        }
        j++;
      }
      dictionary->buckets[index].key = key;
      dictionary->buckets[index].value = value;
      dictionary->buckets[index].bitmap |= (1ULL << index);
      dictionary->full_buckets += 1;
      return true;
    }
  // recorrió todo el vecindario pero estaban todos llenos, prueba en otro bucket
}
    //recorrió toda la tabla y estaba llena, hay que hacer rehash.
    return rehash(dictionary);
}

/* Obtiene un valor del diccionario desde su clave. O(1).
 * Pre-condiciones
 * - El diccionario existe
 * - La clave tiene largo mayor a cero
 * Post-condiciones:
 * - Si la clave está presente, retorna el valor asociado y err debe ser false
 * - De otro modo, debe retornar NULL y err debe ser true*/
void *dictionary_get(dictionary_t *dictionary, const char *key, bool *err) {
  *err = !dictionary_contains(dictionary,key);
  if(!(*err)) return dictionary->buckets[index_search(dictionary,key)].value;    
  return NULL;
};
/* Elimina una clave del diccionario. O(1).
 * Pre-condiciones
 * - El diccionario existe
 * - La clave tiene largo mayor a cero
 * Retorna true si la clave estaba presente y se pudo eliminar, o false
 * de otro modo.
 */
bool dictionary_delete(dictionary_t *dictionary, const char *key) {
  if(dictionary_contains(dictionary,key)){
    size_t index = index_search(dictionary,key);
    dictionary->buckets[index].key = NULL;
    dictionary->buckets[index].value = NULL;
    dictionary->buckets[index].bitmap &= ~(1ULL << index);
    dictionary->full_buckets--;
    return true;
}
return false;
};

/* Elimina una clave y retorna su valor asociado. O(1).
 * Pre-condiciones:
 * - El diccionario existe
 * - La clave tiene largo mayor a cero
 * Post-condiciones:
 * - Si la calve está presente, retorna el valor asocaido y err debe ser false
 * - De otro modo, debe retornar NULL y err debe ser true
 */

void *dictionary_pop(dictionary_t *dictionary, const char *key, bool *err) {
  *err = !dictionary_contains(dictionary,key); 
  if(!(*err)){
      size_t index = index_search(dictionary,key);
      void* value = dictionary->buckets[index].value;
      dictionary_delete (dictionary,key);
      return value;
  }
  return NULL;
};

bool dictionary_contains(dictionary_t *dictionary, const char *key) {
  if(!index_search(dictionary,key)) return false;
  return true;
}
//puedo elegir que devuelva otra variable __inline__ uint32_t?
size_t dictionary_size(dictionary_t *dictionary) { return dictionary->full_buckets;};

void dictionary_destroy(dictionary_t *dictionary){
  if(dictionary){
    dictionary->destroy(dictionary->buckets);
    dictionary->destroy(dictionary);
    }
};
