#include "tp3.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

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

size_t murmurhash(const char *key, size_t len, uint32_t seed) {

  uint32_t c1 = 0xcc9e2d51;
  uint32_t c2 = 0x1b873593;
  uint32_t r1 = 15;
  uint32_t r2 = 13;
  uint32_t m = 5;
  uint32_t n = 0xe6546b64;
  uint32_t h = 0;
  uint32_t k = 0;
  uint8_t *d = (uint8_t *)key; // 32 bit extract from `key'
  const uint32_t *chunks = NULL;
  const uint8_t *tail = NULL; // tail - last 8 bytes
  size_t i = 0;
  size_t l = len / 4; // chunk length

  h = seed;

  chunks = (const uint32_t *)(d + l * 4); // body
  tail = (const uint8_t *)(d + l * 4);    // last 8 byte chunk of `key'

  // for each 4 byte chunk of `key'
  for (i = -l; i != 0; ++i) {
    // next 4 byte chunk of `key'
    k = chunks[i];

    // encode next 4 byte chunk of `key'
    k *= c1;
    k = (k << r1) | (k >> (32 - r1));
    k *= c2;

    // append to hash
    h ^= k;
    h = (h << r2) | (h >> (32 - r2));
    h = h * m + n;
  }

  k = 0;

  // remainder
  switch (len & 3) { // `len % 4'
    case 3:
      k ^= (tail[2] << 16);
    case 2:
      k ^= (tail[1] << 8);

    case 1:
      k ^= tail[0];
      k *= c1;
      k = (k << r1) | (k >> (32 - r1));
      k *= c2;
      h ^= k;
  }

  h ^= (uint32_t)len;

  h ^= (h >> 16);
  h *= 0x85ebca6b;
  h ^= (h >> 13);
  h *= 0xc2b2ae35;
  h ^= (h >> 16);

  return (size_t)h;
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
  }while (real_index != hash_key % dictionary->size);
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
    if ((float)dictionary_size(dictionary) / (float) dictionary->size >=0.6) {
     if(!rehash_table(dictionary)) return false;
    }
    bool err = true;
    size_t index = get_index(dictionary,key,&err);
    if(!err) return insert_existing_key(dictionary,value,index);
    return insert_bucket (dictionary,key,value,index);
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