#include "test_malloc.h"
#include "testing.h"
#include "internal_iterator.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct stack{
  dictionary_t* dict;
  size_t limit;
}stack_t;
char* generate_random_string(int length,int seed) {
  srand(seed);
    static const char charset[] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
    char* random_string = malloc(length + 1);
    if (!random_string) return NULL;
    
    for (int i = 0; i < length; i++) {
        size_t index = rand() % (sizeof(charset) - 1);
        random_string[i] = charset[index];
    }
    
    random_string[length] = '\0';
    return random_string;
}

bool f1 (const char* key, void* value, stack_t* extra){
  if(extra->dict->used_buckets<extra->limit){
     dictionary_put(extra->dict,key,value);
     return false;
  }
  return true;
}

bool f2 (const char* key, void* value, stack_t* extra){
  if((strlen(key)%11!=0)){
    dictionary_put(extra->dict,key,value);
    return false;
  }
  return true;
}
stack_t* stack_create(int limit) {
  stack_t* extra = malloc(sizeof(stack_t));
  if (!extra) return NULL;
  extra->dict = dictionary_create(free);
  if (!extra->dict) {
    free(extra); 
    return NULL;
  }
  extra->limit = limit;
  return extra;
}
bool test_prime_stop(){
  printf("\n========== %s ==========\n",
         __PRETTY_FUNCTION__);
  
  bool tests_result = true;
  stack_t* extra = stack_create(20);
  tests_result &= test_assert("El stack está vacío", extra != NULL);
  dictionary_t *dict1 = dictionary_create(free);
  tests_result &= test_assert("El diccionario está vacio", dict1!= NULL);
  bool insert = true;
  for (int i = 0; i < 30; i++) {
    int random_number = rand();
    char *str = generate_random_string(i,123+i);
    int *random_number_copy = malloc(sizeof(int));
    *random_number_copy = random_number;
    insert &= dictionary_put(dict1, str, random_number_copy);
    free(str);
  }
  tests_result &= test_assert("se insertaron elementos en el diccionario", insert);
  tests_result &= test_assert("hay 30 elementos en el diccionario", dict1->used_buckets==30);
  iterate_f func = (iterate_f) &f2;
  iterate(dict1,func,extra);
  tests_result &= test_assert("NO iteró hasta llegar a su límite",extra->dict->used_buckets!=20);
  bool non_prime = true;
  for(size_t i = 0; i<extra->dict->size;i++){
    if(extra->dict->buckets[i].key){
      non_prime &= (strlen(extra->dict->buckets[i].key)%11!=0);
    }
  }
  tests_result &= test_assert("Ningún elemento ingresado tiene largo 11",non_prime);
  dictionary_destroy(dict1);
  delete_keys(extra->dict);
  dictionary_destroy(extra->dict);
  free(extra);
  return tests_result; 
}
bool test_iterate (int n, int limit){
  printf("\n========== %s (n=%u, limit =%u) ==========\n",
         __PRETTY_FUNCTION__, n,limit);
  
  bool tests_result = true;
  stack_t* extra = stack_create(limit);
  tests_result &= test_assert("El stack está vacío", extra != NULL);
  dictionary_t *dict1 = dictionary_create(free);
  tests_result &= test_assert("El diccionario está vacio", dict1!= NULL);
  bool insert = true;
  for (int i = 0; i < n; i++) {
    int random_number = rand();
    char *str = generate_random_string(5,123+i);
    int *random_number_copy = malloc(sizeof(int));
    *random_number_copy = random_number;
    insert &= dictionary_put(dict1, str, random_number_copy);
    free(str);
  }
  tests_result &= test_assert("se insertaron elementos en el diccionario", insert);
  tests_result &= test_assert("hay n elementos en el diccionario", dict1->used_buckets==n);
  iterate_f func = (iterate_f) &f1;
  iterate(dict1,func,extra);
  tests_result &= test_assert("Se iteró hasta llegar al límite", extra->dict->used_buckets==extra->limit);
  tests_result &= test_assert("el extra tiene limit elementos", extra->dict->used_buckets==limit);
  dictionary_destroy(dict1);
  delete_keys(extra->dict);
  dictionary_destroy(extra->dict);
  free(extra);
  return tests_result; 
}

bool test_bigger_limit(int n, int limit){
  printf("\n========== %s (n=%u, limit =%u) ==========\n",
         __PRETTY_FUNCTION__, n,limit);
  
  bool tests_result = true;
  stack_t* extra = stack_create(limit);
  tests_result &= test_assert("El stack está vacío", extra != NULL);
  dictionary_t *dict1 = dictionary_create(free);
  tests_result &= test_assert("El diccionario está vacio", dict1!= NULL);
  bool insert = true;
  for (int i = 0; i < n; i++) {
    int random_number = rand();
    char *str = generate_random_string(5,123+i);
    int *random_number_copy = malloc(sizeof(int));
    *random_number_copy = random_number;
    insert &= dictionary_put(dict1, str, random_number_copy);
    free(str);
  }
  tests_result &= test_assert("se insertaron elementos en el diccionario", insert);
  tests_result &= test_assert("hay n elementos en el diccionario", dict1->used_buckets==n);
  iterate_f func = (iterate_f) &f1;
  iterate(dict1,func,extra);
  tests_result &= test_assert("no se iteró hasta llegar al límite", extra->dict->used_buckets!=extra->limit);
  tests_result &= test_assert("el extra tiene n elementos", extra->dict->used_buckets==n);
  dictionary_destroy(dict1);
  delete_keys(extra->dict);
  dictionary_destroy(extra->dict);
  free(extra);
  return tests_result; 
}

bool test_iterate_null_dict( int limit){
  printf("\n========== %s ( limit =%u) ==========\n",
         __PRETTY_FUNCTION__,limit);
  
  bool tests_result = true;
  stack_t* extra = stack_create(limit);
  tests_result &= test_assert("El stack está vacío", extra != NULL);
  iterate_f func = (iterate_f) &f1;
  iterate(NULL,func,extra);
  tests_result &= test_assert("no se iteró hasta llegar al límite", extra->dict->used_buckets!=extra->limit);
  tests_result &= test_assert("el extra tiene 0 elementos", extra->dict->used_buckets==0);
  delete_keys(extra->dict);
  dictionary_destroy(extra->dict);
  free(extra);
  return tests_result; 
}


int main(void) {
  srand(117);
  int return_code = 0;
  return_code += !test_iterate (20,10);
  return_code += !test_iterate (5000,1120);
  return_code += !test_bigger_limit(50,400);
  return_code += !test_iterate_null_dict(100);
  return_code += !test_prime_stop();
  return return_code;
}
