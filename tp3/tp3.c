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
    size_t used_buckets;
    uint32_t seed;
};
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
dictionary_t *dictionary_create(destroy_f destroy) { 
    dictionary_t* new_dict = malloc(sizeof(dictionary_t));
    if(!new_dict) return NULL;
    new_dict->size = 1024;
    new_dict->buckets = calloc(sizeof(bucket_t),new_dict->size);
    if (!new_dict->buckets) return NULL;
    new_dict->used_buckets = 0;
    new_dict->destroy = destroy;
    new_dict->seed = 0xbc9f1d34;
    return new_dict;
};
size_t get_index(dictionary_t* dictionary, const char *key, size_t len, uint32_t seed, bool* err) {
    size_t hash_key = murmurhash(key, strlen(key), dictionary->seed);
    size_t real_index = hash_key % dictionary->size;
    
    do{
        if (dictionary->buckets[real_index].key && strcmp(dictionary->buckets[real_index].key, key) == 0) {
            *err = false;
            return real_index;
        } 
        if (!dictionary->buckets[real_index].key && !dictionary->buckets[real_index].is_deleted) {
            *err = true;
            return real_index;
        }
        real_index++;
        if (real_index == dictionary->size) {
            real_index = 0;
        }
        if (real_index == hash_key % dictionary->size) {
            break;
        }
    }while (real_index<=dictionary->size);
    return 0;
}

bool rehash_table(dictionary_t* old_dict) {
  printf("\n----------REHASH-----------------\n");
  dictionary_t* new_dict = dictionary_create(old_dict->destroy);
  if (!new_dict) return false;
  new_dict->size = old_dict->size*2;
  free(new_dict->buckets);
  new_dict->buckets = calloc(sizeof(bucket_t), new_dict->size);
  if (!new_dict->buckets) {
    dictionary_destroy(new_dict);
    return false;
  }

  for (size_t i = 0; i <old_dict->size; i++) {
    if (old_dict->buckets[i].key) {
       if (!dictionary_put(new_dict, old_dict->buckets[i].key, old_dict->buckets[i].value)) {
          dictionary_destroy(new_dict);
          return false;
       }
       free(old_dict->buckets[i].key);
    }
  }
  old_dict->size= new_dict->size;
  free(old_dict->buckets);
  old_dict->buckets = new_dict->buckets;
  free(new_dict);
  return true;
}
bool dictionary_put(dictionary_t *dictionary, const char *key, void *value) {
    float sobrecarga = (float)dictionary_size(dictionary) / (float) dictionary->size;
    if (sobrecarga >=0.7) {
     if(!rehash_table(dictionary)) return false;
    }
    bool err = true;
    size_t index = get_index(dictionary,key,strlen(key),dictionary->seed,&err);
    if(!err){
        if( dictionary->destroy && dictionary->buckets[index].value != value){
            dictionary->destroy(dictionary->buckets[index].value);
        }
        dictionary->buckets[index].value = value;
        return true;
    }
    char* new_key = malloc (sizeof(char)*(strlen(key)+1));
    if (!new_key) return false;
    strcpy(new_key,key);
    dictionary->buckets[index].key = new_key;
    dictionary->buckets[index].value = value;
    dictionary->buckets[index].is_deleted = false;
    dictionary->used_buckets++;
    return true;
  }

void *dictionary_get(dictionary_t *dictionary, const char *key, bool *err) {
  size_t index = get_index(dictionary,key,strlen(key),dictionary->seed,err);
  if(!(*err)) return dictionary->buckets[index].value;    
  return NULL;
};

bool dictionary_delete(dictionary_t *dictionary, const char *key) {
  bool err = true;
  size_t index = get_index(dictionary,key,strlen(key),dictionary->seed,&err);
  if(!err){
    if (dictionary->destroy) dictionary->destroy(dictionary->buckets[index].value);
    dictionary_pop(dictionary,key,&err);
    // dictionary->buckets[index].value = NULL;
    dictionary->buckets[index].is_deleted = true;
    dictionary->used_buckets--;
  }
  return !err;
};

void *dictionary_pop(dictionary_t *dictionary, const char *key, bool *err) {
    size_t index = get_index(dictionary,key,strlen(key),dictionary->seed,err);
    if(!index) return NULL;
    void* value = dictionary_get(dictionary,key,err);
    free(dictionary->buckets[index].key);
    dictionary->buckets[index].key = NULL; 
    return value;
}

bool dictionary_contains(dictionary_t *dictionary, const char *key) {
    bool err = true;
    get_index(dictionary,key,strlen(key),dictionary->seed,&err);
    return !err;
}

size_t dictionary_size(dictionary_t *dictionary) { return dictionary->used_buckets;}

void dictionary_destroy(dictionary_t *dictionary){
    for(size_t i = 0; i<dictionary->size;i++){
        if(dictionary->buckets[i].key){
            dictionary_delete(dictionary,dictionary->buckets[i].key);
        }
    }
    free(dictionary->buckets);
    free(dictionary);
}
