#include <stdbool.h>
#include "operable_dict.h"
#include <string.h>

void* value_copy(void* value){
    void* bucket = malloc(sizeof(*value));
    if (!bucket) return NULL;
    memcpy(bucket, value, sizeof(*value));
    return bucket;
}

dictionary_t* dictionary_copy (dictionary_t* dictionary){
  dictionary_t* copy = new_dictionary(dictionary->destroy,dictionary->size);
  for(size_t i= 0;i< dictionary->size;i++){
    if(dictionary->buckets[i].key){
        copy->buckets[i].key = key_copy(dictionary->buckets[i].key);
        copy->buckets[i].value = value_copy (dictionary->buckets[i].value);
        copy->used_buckets++;
      } 
    }
  return copy;
}

/*
 * Punto extra de operable dict, suma 3 puntos como máximo.
 */

/*
 * Inserta o pisa en dictionary1 todas las claves y valores que están en dictionary2.
 * Las claves se mantienen independientes entre ambos diccionarios, pero los valores no.
 * (las claves nuevas de agregar dictionary2 en dictionary1 deben ser punteros distintos
 * pero los valores son el mismo puntero).
 * La operación es atómica, si falla dictionary1 queda en el mismo estado que estaba antes del update.
 * Devuelve true si funcionó, false si falló.
 */

bool dictionary_update(dictionary_t *dictionary1, dictionary_t *dictionary2){
    if(!dictionary1 || !dictionary2) return false;
    dictionary_t* copy = dictionary_copy(dictionary1);
    if(!copy) return false;
    for( size_t i = 0; i<dictionary2->size;i++){
        if (dictionary2->buckets[i].key){
            if(!dictionary_put(dictionary1,dictionary2->buckets[i].key,dictionary2->buckets[i].value)){
                change_buckets(copy,dictionary1);
                return false;
            }
        }
    }
    dictionary_destroy(copy);
    return true;
}

/*
 * Combina ambos diccionarios en uno nuevo que contiene solo las claves que están presentes en ambos.
 * En todos los casos se conservan los valores del dictionary1.
 * Devuelve NULL si falla.
 */
dictionary_t* dictionary_and(dictionary_t *dictionary1, dictionary_t *dictionary2){
    if(!dictionary1 || !dictionary2) return NULL;
    dictionary_t* new_dict = dictionary_create(dictionary1->destroy);
    if(!new_dict) return NULL;
    for(size_t i =0; i<dictionary1->size;i++){
        if(dictionary1->buckets[i].key && dictionary_contains(dictionary2,dictionary1->buckets[i].key)){
            if(!dictionary_put(new_dict,dictionary1->buckets[i].key,dictionary1->buckets[i].value)){
                delete_keys(new_dict);
                dictionary_destroy(new_dict);
                return NULL;
            }
        }
    }
    return new_dict;
}

/*
 * Combina ambos diccionarios en uno nuevo que contiene todas las claves de ambos.
 * En caso de que ambos tengan una misma clave se conserva el valor de dictionary1.
 * Devuelve NULL si falla.
 */

bool insert_dictionary_elements(dictionary_t*new_dict ,dictionary_t* dictionary){
    for(size_t i = 0; i<dictionary->size;i++){
        if(dictionary->buckets[i].key){
            if(!dictionary_contains(new_dict,dictionary->buckets[i].key)){
                if(!dictionary_put(new_dict,dictionary->buckets[i].key,dictionary->buckets[i].value))
                return false;
            }
        }
    }
    return true;
}
dictionary_t* dictionary_or(dictionary_t *dictionary1, dictionary_t *dictionary2){
    if(!dictionary1 || !dictionary2) return NULL;
    dictionary_t* new_dict = dictionary_and(dictionary1,dictionary2);
    if(!new_dict) return NULL;
    if(!insert_dictionary_elements(new_dict,dictionary1) || 
       !insert_dictionary_elements(new_dict,dictionary2))
    {
        delete_keys(new_dict);
        dictionary_destroy(new_dict);
        return NULL;
    }
    return new_dict;
}

/*
 * Devuelve true si ambos diccionarios son iguales. Dos diccionarios son iguales sí:
 *  - Las claves son iguales aunque puedan tener distinta posición en memoria.
 *  - Los values son los mismos punteros para cada clave
 *  - Tienen la misma cantidad de claves
 */
bool dictionary_equals(dictionary_t *dictionary1, dictionary_t *dictionary2){
    if(!dictionary1 || !dictionary2) return false;
    bool err = true;
    for(size_t i=0;i<dictionary2->size;i++){
        if(dictionary2->buckets[i].key){
            if(dictionary_get(dictionary1,dictionary2->buckets[i].key,&err)!=dictionary2->buckets[i].value){
                return false;
            }
        }
    }
    return true;
}
