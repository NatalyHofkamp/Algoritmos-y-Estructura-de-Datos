#include "tp3.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

// #preguntas:
// → pierdo memoria por no borrar los values que son punteros, si los borro
//   se me rompe todo en el pop (get)
// if(destroy value) dictionary->destroy (dictionary->bucket[i].value)

typedef struct bucket bucket_t;
struct bucket {
    char *key;
    void *value;
};


struct dictionary {
    size_t size;
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

size_t index_search (dictionary_t* dictionary, const char* key){
  size_t size = dictionary->size;
  uint32_t hs = _jenkins_hash(key, strlen(key));
  size_t index = hs & (size - 1); //mete el hash dentro del rango del tamaño de la tabla
  
  for (size_t i = index; i < dictionary->size; i++) {
    if (dictionary->buckets[i].key && strcmp(dictionary->buckets[i].key,key) == 0) {
      return index; // Se encontró el elemento en el bucket actual
    }
    size_t j = 1;
    while (j < dictionary->bitmap_len) {
      size_t neighbor_index = (i + j) % size; // Índice del vecino considerando el tamaño de la tabla
      if (dictionary->buckets[neighbor_index].key && strcmp(dictionary->buckets[neighbor_index].key,key) == 0) {
        return neighbor_index; // Se encontró el elemento en un vecino
      }
      j++;
    }
  }
  return false; // No se encontró el elemento en la tabla
}


dictionary_t *dictionary_create(destroy_f destroy) { 
    dictionary_t* new_dict = (dictionary_t*) malloc (sizeof(dictionary_t));
    if(!new_dict) return NULL;

    new_dict-> size = 1014;
    new_dict->bitmap_len = 32; 
    new_dict->full_buckets = 0;
    if(destroy) new_dict->destroy = destroy;
    else new_dict->destroy = NULL;

    new_dict->buckets = calloc (sizeof(bucket_t),new_dict->size);
    if(!new_dict->buckets) return NULL;
    return new_dict;
 }


bool rehash(dictionary_t* old_dict) {
  printf("\n----------REHASH-----------------\n");
  dictionary_t* new_dict = dictionary_create(old_dict->destroy);
  if (!new_dict) return false;
  new_dict->size = old_dict->size*2;
  free(new_dict->buckets);
  new_dict->buckets = calloc(sizeof(bucket_t), new_dict->size);
  if (!new_dict->buckets) {
    dictionary_destroy(new_dict); // Liberar la nueva tabla de hash en caso de fallo
    return false;
  }

  for (size_t i = 0; i < old_dict->size; i++) {
    if (old_dict->buckets[i].key) {
       if (!dictionary_put(new_dict, old_dict->buckets[i].key, old_dict->buckets[i].value)) {
          dictionary_destroy(new_dict);
          return false;
       }
    }
  }
  old_dict->size= new_dict->size;
  free(old_dict->buckets);
  old_dict->buckets = new_dict->buckets;
  free(new_dict);
  return true;
}

bool dictionary_put(dictionary_t *dictionary, const char *key, void *value) {
  size_t size = dictionary->size;
  uint32_t hs = _jenkins_hash(key, strlen(key));
  size_t index = hs & (size - 1); // hash dentro del rango del tamaño de la tabla
  if (dictionary_contains(dictionary, key)) {
    size_t existing_index = index_search(dictionary, key);
    if( dictionary->destroy && dictionary->buckets[existing_index].value != value){
          dictionary->destroy(dictionary->buckets[existing_index].value);
          dictionary->buckets[existing_index].value = value;
    }
    return true;
  }
  if (((float)dictionary_size(dictionary) / (float)(dictionary->size)) <= 0.7) {
    char *new_key = (char *)malloc((strlen(key)+1) * sizeof(char));
    if (!new_key) return false;
    strcpy(new_key, key);
    for (size_t i = index; i < dictionary->size; i++) {
      if (!(dictionary->buckets[i].bitmap & (1ULL << i))) { 
        size_t j = 1;
        while ((i + j) < dictionary->size) {
          if (!(dictionary->buckets[i + j].bitmap & (1ULL << (i + j)))) { // Verificar el bit correspondiente en el bitmap
            index += j;
            break;
          }
          j++;
        }
        dictionary->buckets[index].key = new_key;
        dictionary->buckets[index].value = value;
        dictionary->full_buckets ++;
        return true;
      }
    }
  }
  if(!rehash(dictionary)) return false;
  dictionary_put(dictionary, key, value);
  return true;
}

void *dictionary_get(dictionary_t *dictionary, const char *key, bool *err) {
  *err = !dictionary_contains(dictionary,key);
  if(!(*err)) return dictionary->buckets[index_search(dictionary,key)].value;    
  return NULL;
}

bool dictionary_delete(dictionary_t *dictionary, const char *key) {
  bool err = true;
  if(dictionary_contains(dictionary,key)){
    size_t index = index_search(dictionary,key);
    if( dictionary->destroy ){
                dictionary->destroy(dictionary->buckets[index].value);
              }
    if(dictionary_pop(dictionary,key,&err)) return true;
  }
  return false;
}

void *dictionary_pop(dictionary_t *dictionary, const char *key, bool *err) {
  *err = !dictionary_contains(dictionary,key);
  if(!(*err)){
    void* value = dictionary_get(dictionary,key,err);
    size_t index = index_search(dictionary,key);
    free(dictionary->buckets[index].key);
    dictionary->buckets[index].key = NULL; 
    dictionary->full_buckets--;
    return value;
  }
  return NULL;
}

bool dictionary_contains(dictionary_t *dictionary, const char *key) {
  if(!index_search(dictionary,key)) return false;
  return true;
}
//puedo elegir que devuelva otra variable __inline__ uint32_t?
size_t dictionary_size(dictionary_t *dictionary) { return dictionary->full_buckets;}

void dictionary_destroy(dictionary_t *dictionary){
  if(dictionary){
    for( size_t i = 0; i< dictionary->size;i++){
      if(dictionary->buckets[i].key){
        dictionary_delete(dictionary,dictionary->buckets[i].key);
      }
    }
    free(dictionary->buckets);
    free(dictionary);
    }
}
