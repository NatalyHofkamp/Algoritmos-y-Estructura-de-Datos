#include "tp3.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>


typedef struct bucket{
    void* key;
    void* value;
    bool is_deleted;
}bucket_t;

struct dictionary {
    bucket_t* buckets;
    size_t size;
    destroy_f destroy;
    size_t last_filled;
    size_t* filled_buckets;
    uint32_t seed;
    size_t  index_deleted;
};

dictionary_t* dictionary_copy (dictionary_t* dictionary){
  dictionary_t* copy = dictionary_create(dictionary->destroy);
  for(size_t i= 0;i< dictionary->size;i++){
    if(dictionary->buckets[i].key){
      if(!dictionary_put(copy,dictionary->buckets[i].key,dictionary->buckets[i].value)){
        dictionary_delete_keys(copy);
        free(copy);
        return NULL;
      } 
    }
  }
  return copy;
}

void dictionary_delete_keys(dictionary_t* dictionary){
  for(size_t i=0;i< dictionary->size;i++){
    if(dictionary->buckets[i].key){
      free(dictionary->buckets[i].key);
      dictionary->buckets[i].key = NULL;
      dictionary->buckets[i].is_deleted = true;
    }
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

dictionary_t* new_dictionary(destroy_f destroy ,size_t size){
  dictionary_t* new_dict = malloc(sizeof(dictionary_t));
    if(!new_dict) return NULL;
    new_dict->size = size;
    new_dict->buckets = calloc(sizeof(bucket_t),new_dict->size);
    if (!new_dict->buckets){
      free(new_dict);
      return NULL;
    }
    new_dict->last_filled=0;
    new_dict->index_deleted=0;
    new_dict->filled_buckets= calloc(sizeof(size_t),new_dict->size);
    if (!new_dict->filled_buckets){
      free(new_dict->buckets);
      free(new_dict);
      return NULL;
    }
    new_dict->destroy = destroy;
    new_dict->seed = 0xbc9f1d34;

    return new_dict;
};

dictionary_t *dictionary_create(destroy_f destroy) { 
    return new_dictionary(destroy,1024);
};

bool insert_new_bucket (dictionary_t* dictionary, const char* key, void*value, size_t index){
  char* new_key = malloc (sizeof(char)*(strlen(key)+1));
  if (!new_key) return false;
  strcpy(new_key,key);
  if(dictionary->index_deleted<index){
    index = dictionary->index_deleted;
  }
  dictionary->buckets[index].key = new_key;
  dictionary->buckets[index].value = value;
  dictionary->buckets[index].is_deleted = false;
  dictionary->filled_buckets[dictionary->last_filled] = index;
  dictionary->last_filled++;
  return true;
}

size_t get_index(dictionary_t* dictionary, const char* key, bool* err){
  for(size_t i = 0; i<dictionary_size(dictionary);i++){
    size_t index = dictionary->filled_buckets[i];
    if (strcmp(dictionary->buckets[index].key, key) == 0) {
      *err = false;
      return index;
    } 
  }
  *err=true;
  return 0;
}

size_t get_free_index(dictionary_t* dictionary, const char *key, size_t len, uint32_t seed) {
    size_t hash_key = murmurhash(key, strlen(key), dictionary->seed);
    size_t real_index = hash_key % dictionary->size;
    do{
        if (!dictionary->buckets[real_index].key){
          if(dictionary->buckets[real_index].is_deleted){
            dictionary->index_deleted = real_index;
          }
          if (!dictionary->buckets[real_index].is_deleted) {
            return real_index;
          }
        }
        real_index = (real_index+1)%(dictionary->size);
    } while (real_index != hash_key % dictionary->size);
    return 0;
}
bool rehash_table(dictionary_t* old_dict) {
  printf("####### HIZO REHASH ########\n");
  dictionary_t* new_dict = new_dictionary(old_dict->destroy, old_dict->size*2);
  for (size_t i = 0; i <old_dict->last_filled; i++) {
      size_t index = old_dict->filled_buckets[i];
       if (!dictionary_put(new_dict, old_dict->buckets[index].key, old_dict->buckets[index].value)) {
          dictionary_delete_keys(new_dict);
          dictionary_destroy(new_dict);
          return false;
       }
  }
  dictionary_delete_keys(old_dict);
  old_dict->size= new_dict->size;
  free(old_dict->buckets);
  old_dict->buckets = new_dict->buckets;
  free(old_dict->filled_buckets);
  old_dict->filled_buckets= new_dict->filled_buckets;
  free(new_dict);
  return true;
}

bool replace_same_key(dictionary_t* dictionary,const char* key,void*value){
  bool err = true;
  size_t index = get_index(dictionary,key,&err);
  dictionary->destroy(dictionary->buckets[index].value);
  dictionary->buckets[index].value = value;
  return !err;
}

bool dictionary_put(dictionary_t *dictionary, const char *key, void *value) {
    if ((float)dictionary_size(dictionary) / (float) dictionary->size >=0.7) {
     if(!rehash_table(dictionary)) return false;
    }
    if(dictionary_contains(dictionary,key)) return replace_same_key(dictionary,key,value);
    size_t index = get_free_index(dictionary,key,strlen(key),dictionary->seed);
    return insert_new_bucket(dictionary,key,value,index);
  }

void *dictionary_get(dictionary_t *dictionary, const char *key, bool *err) {
  size_t index = get_index(dictionary,key,err);
  if(!*err){
    return dictionary->buckets[index].value;   
  }
  return NULL;
}

bool dictionary_delete(dictionary_t *dictionary, const char *key) {
  bool err = true;
  void* value = dictionary_pop(dictionary,key,&err);
  if(!err){
    if (dictionary->destroy) dictionary->destroy(value);
  }
  return !err;
};

void *dictionary_pop(dictionary_t *dictionary, const char *key, bool *err) {
    size_t index = get_index(dictionary,key,err);
    if(*err){
      // printf("##########\n");
      return NULL;
    } 
    void* value = dictionary->buckets[index].value;
    free(dictionary->buckets[index].key);
    dictionary->buckets[index].key = NULL; 
    dictionary->buckets[index].is_deleted = true;

    size_t i = 0;
    while(dictionary->filled_buckets[i] != index){
      i++;
    }
    size_t last_val=dictionary->filled_buckets[dictionary->last_filled-1];
    dictionary->filled_buckets[i] = last_val;
    dictionary->filled_buckets[dictionary->last_filled-1] = 0;
    dictionary->last_filled-=1;

    return value;
}

bool dictionary_contains(dictionary_t *dictionary, const char *key) {
  bool err = true;
  get_index(dictionary,key,&err);
  return !err;
}

size_t dictionary_size(dictionary_t *dictionary) { return dictionary->last_filled;}    

void dictionary_destroy(dictionary_t *dictionary){
  // printf("CANTIDAD DE ELEMENTOS %li\n",dictionary->last_filled);
  while(dictionary->last_filled>0){
    // printf("entró a borrar.\n");
    size_t index = dictionary->filled_buckets[0];
    dictionary_delete(dictionary,dictionary->buckets[index].key);
  }
  free(dictionary->filled_buckets);
  free(dictionary->buckets);
  free(dictionary);
}