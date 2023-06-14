#include "tp3.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h> 

bool delete_single_key(dictionary_t* dictionary,size_t index){
  if(!dictionary->buckets[index].key) return false;
  free(dictionary->buckets[index].key);
  dictionary->buckets[index].key = NULL; 
  dictionary->buckets[index].is_deleted = true;
  dictionary->used_buckets--;
  return true;
}

void delete_keys(dictionary_t* dictionary){
  for(size_t i=0;i< dictionary->size;i++){
      delete_single_key(dictionary,i);
  }
}

uint32_t murmurhash( const char * key, size_t len, uint32_t seed )
{
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

dictionary_t* new_dictionary(destroy_f destroy,size_t size){
  dictionary_t* new_dict = malloc(sizeof(dictionary_t));
  if(!new_dict) return NULL;
  new_dict->size = size;
  new_dict->buckets = calloc(sizeof(bucket_t),new_dict->size);
  if (!new_dict->buckets){
    free(new_dict);
    return NULL;
  }
  new_dict->used_buckets = 0;
  new_dict->deleted_index = 0;
  new_dict->destroy = destroy;
  new_dict->seed = 0xbc9f1d34;
  return new_dict;
}

bool change_buckets(dictionary_t* new_dict,dictionary_t* old_dict){
  delete_keys(old_dict);
  old_dict->used_buckets = new_dict->used_buckets;
  old_dict->size= new_dict->size;
  free(old_dict->buckets);
  old_dict->buckets = new_dict->buckets;
  free(new_dict);
  return true;
}

bool rehash_table(dictionary_t* old_dict) {
  dictionary_t*  new_dict = new_dictionary(old_dict->destroy,old_dict->size*2);
  if(!new_dict) return NULL;
  for(size_t i= 0;i< old_dict->size;i++){
    if(old_dict->buckets[i].key && !dictionary_put(new_dict,
        old_dict->buckets[i].key,old_dict->buckets[i].value))
    {
      delete_keys(new_dict);
      dictionary_destroy(new_dict);
      return NULL;
    }
  }
  delete_keys(old_dict);
  return change_buckets(new_dict,old_dict);
}

size_t get_index(dictionary_t* dictionary, const char *key, bool* err) {
  size_t hash_key = murmurhash(key, strlen(key), dictionary->seed);
  size_t real_index = hash_key % dictionary->size;
  *err = true;
  do{
      if (dictionary->buckets[real_index].key && strcmp(dictionary->buckets[real_index].key, key) == 0) {
          *err = false;
          return real_index;
      } 
      if(dictionary->buckets[real_index].is_deleted){
        dictionary->deleted_index = real_index;
      }
      if (!dictionary->buckets[real_index].key && !dictionary->buckets[real_index].is_deleted) {
          return real_index;
      }
      real_index = (real_index+1)%(dictionary->size);
  }while (real_index < dictionary->size);
  return 0;
}

char* key_copy(const char* key){
  if(!key) return NULL;
  char* new_key = malloc (sizeof(char)*(strlen(key)+1));
  if (!new_key) return NULL;
  strcpy(new_key,key);
  return new_key;
}

bool insert_bucket (dictionary_t* dictionary,const char* key, void* value,size_t index){
  char* new_key = key_copy(key);
  if(!new_key) return NULL;
  if(dictionary->deleted_index) index = dictionary->deleted_index;
  dictionary->buckets[index].key = new_key;
  dictionary->buckets[index].value = value;
  dictionary->buckets[index].is_deleted = false;
  dictionary->used_buckets++;
  return true;

}

bool insert_existing_key(dictionary_t* dictionary,void* value,size_t index){
  dictionary->destroy(dictionary->buckets[index].value); 
  dictionary->buckets[index].value = value;
  return true;
}

dictionary_t *dictionary_create(destroy_f destroy) { 
    return new_dictionary(destroy,1021);
};


bool dictionary_put(dictionary_t *dictionary, const char *key, void *value) {
  size_t size= dictionary->size;
  uint32_t hs = murmurhash(key, strlen(key),dictionary->seed);
  size_t index = hs & (size-1); //hash dentro del rango del tamaño de la tabla
  if(dictionary_contains(dictionary,key)){insert_existing_key(dictionary,value,index); }
  for (size_t i = index; i < 32; i++) {
    if (!dictionary->buckets[i].key) { // si está vacío igual lo guardamos en algún vecino (evitar colisiones)
      size_t j = 1;
      while ((i + j) < dictionary->size) {
        if (!dictionary->buckets[i + j].key) { // encontró un vecino vacío
          index += j;
          break;
        }
        j++;
      }
      return insert_bucket(dictionary,key,value,index);
    }
    rehash_table(dictionary);
  }
  return false;
}
void *dictionary_get(dictionary_t *dictionary, const char *key, bool *err) {
  size_t index = get_index(dictionary,key,err);
  if(!(*err)) return dictionary->buckets[index].value;    
  return NULL;
};

bool dictionary_delete(dictionary_t *dictionary, const char *key) {
  if(!key) return NULL;
  bool err = true;
  void* value = dictionary_pop(dictionary,key,&err);
  if(dictionary->destroy) dictionary->destroy(value);
  return !err;
};

void *dictionary_pop(dictionary_t *dictionary, const char *key, bool *err) {
    void* value = dictionary_get(dictionary,key,err);
    delete_single_key(dictionary,get_index(dictionary,key,err));
    return value;
}

bool dictionary_contains(dictionary_t *dictionary, const char *key) {
    bool err = true;
    get_index(dictionary,key,&err);
    return !err;
}

size_t dictionary_size(dictionary_t *dictionary) { return dictionary->used_buckets;}    

void dictionary_destroy(dictionary_t *dictionary){
  for(size_t i=0;0<dictionary->used_buckets;i++){ 
    dictionary_delete(dictionary,dictionary->buckets[i].key);
  }
  free(dictionary->buckets);
  free(dictionary);
}