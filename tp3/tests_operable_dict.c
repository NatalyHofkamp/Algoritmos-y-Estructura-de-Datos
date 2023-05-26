#include "test_malloc.h"
#include "testing.h"
#include "operable_dict.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

char* generate_random_string(int length) {
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

bool test_equals(size_t n, unsigned int seed){
   printf("\n========== %s (n=%lu, seed=%u) ==========\n",
         __PRETTY_FUNCTION__, n, seed);
  srand(seed);
  bool tests_result = true;
  dictionary_t *dict1 = dictionary_create(free);
  tests_result &= test_assert("El diccionario 1 fue creado", dict1 != NULL);
  bool insert1 = true;
  for (size_t i = 0; i < n; i++) {
    int random_number = rand();
    char *str = generate_random_string(5);
    int *random_number_copy = malloc(sizeof(int));
    *random_number_copy = random_number;
    insert1 &= dictionary_put(dict1, str, random_number_copy);
    free(str);
  }
  dictionary_t *dict2 = dictionary_copy(dict1);
  tests_result &= test_assert("El diccionario 2 tiene valores distintos", dict2 != NULL);
  
  bool test_equals = true;
  test_equals = dictionary_equals(dict1,dict2);
  tests_result &= test_assert("Los diccionarios son distintos", !test_equals);

  dictionary_destroy(dict1);
  dictionary_destroy(dict2);
  return tests_result;
}

bool test_equals_null(){
  printf("\n========== %s ==========\n",
         __PRETTY_FUNCTION__);
  bool tests_result = true;
  dictionary_t *dict1 = dictionary_create(free);
  dictionary_t* dict2 = dictionary_create (free);
  tests_result &= test_assert("El diccionario 1 está vacío", dict1 != NULL);
  tests_result &= test_assert("El diccionario 2 está vacío", dict1 != NULL);
  bool test_equals = true;
  test_equals = dictionary_equals(dict1,dict2);
  tests_result &= test_assert("Los diccionarios son iguales", test_equals);
  dictionary_destroy(dict1);
  dictionary_destroy(dict2);
  return tests_result;
}

bool test_equals_different(){
  printf("\n========== %s==========\n",
         __PRETTY_FUNCTION__);
  srand(324);
  bool tests_result = true;
  dictionary_t *dict1 = dictionary_create(free);
  tests_result &= test_assert("El diccionario 1 fue creado", dict1 != NULL);
  dictionary_t *dict2 = dictionary_create(free);
  tests_result &= test_assert("El diccionario 2 fue creado", dict2 != NULL);
   bool insert = true;
  for (size_t i = 0; i < 12; i++) {
    int random_number = rand();
    char *str = generate_random_string(5);
    int *random_number_copy = malloc(sizeof(int));
    *random_number_copy = random_number;
    insert &= dictionary_put(dict1, str, random_number_copy);
    free(str);
  }
  tests_result &= test_assert("Todas las inserciones en dict1 fueron exitosas", insert);
  for (size_t i = 0; i < 45; i++) {
    int random_number = rand();
    char *str = generate_random_string(5);
    int *random_number_copy = malloc(sizeof(int));
    *random_number_copy = random_number;
    insert &= dictionary_put(dict2, str, random_number_copy);
    free(str);
  }
  tests_result &= test_assert("Todas las inserciones en dict2 fueron exitosas", insert);
  
  bool test_equals = true;
  test_equals = dictionary_equals(dict1,dict2);
  tests_result &= test_assert("Los diccionarios no son iguales pero tienen elementos en común", !test_equals);

  dictionary_destroy(dict1);
  dictionary_destroy(dict2);
  return tests_result;
}

bool test_equals_error(){
  printf("\n========== %s ==========\n", __PRETTY_FUNCTION__);
  bool tests_result = true;
  set_malloc_status(true);
  dictionary_t *dict1 = dictionary_create(free);
  tests_result &=test_assert("se creó el diccionario 1", dict1!=NULL);
  set_malloc_status(false);
  dictionary_t *dict2 = dictionary_create(free);
  tests_result &=test_assert("NO se creó el diccionario 2", !dict2);
  set_malloc_status(true);
  bool equals = dictionary_equals(dict1,dict2);
  tests_result &=  test_assert("dictionary_equals falla", !equals);
  dictionary_destroy(dict1);
  return tests_result;
}

bool test_or_null (){
    printf("\n========== %s ==========\n",
         __PRETTY_FUNCTION__);
  srand(123);
  bool tests_result = true;
  dictionary_t *dict1 = dictionary_create(free);
  tests_result &= test_assert("El diccionario 1 está vacío", dict1 != NULL);
  dictionary_t *dict2 = dictionary_create(free);
  tests_result &= test_assert("El diccionario 2 está vacio", dict2 != NULL);
  bool insert = true;
  for (size_t i = 0; i < 14; i++) {
    int random_number = rand();
    char *str = generate_random_string(5);
    int *random_number_copy = malloc(sizeof(int));
    *random_number_copy = random_number;
    insert &= dictionary_put(dict1, str, random_number_copy);
    free(str);
  }
  tests_result &= test_assert("Todas las inserciones en dict1 fueron exitosas", insert);
  dictionary_t* result= dictionary_or(dict1,dict2);
  tests_result &= test_assert("dictionary_or funcionó", result != NULL);
  
  bool test_or = true;
  test_or += dictionary_equals(dict1,result);
  tests_result &= test_assert("El resultado es igual al diccionario1", test_or);
  dictionary_destroy(dict1);
  dictionary_destroy(dict2);
  delete_keys(result);
  free(result->buckets);
  free(result);
  return tests_result; 
}

bool test_or_error(){
  printf("\n========== %s ==========\n", __PRETTY_FUNCTION__);
  bool tests_result = true;
  set_malloc_status(true);
  dictionary_t *dict1 = dictionary_create(free);
  tests_result &=test_assert("se creó el diccionario 1", dict1!=NULL);
  set_malloc_status(false);
  dictionary_t *dict2 = dictionary_create(free);
  tests_result &=test_assert("NO se creó el diccionario 2", !dict2);
  set_malloc_status(true);
  bool or = dictionary_and(dict1,dict2);
  tests_result &=  test_assert("dictionary_or falla", !or );
  dictionary_destroy(dict1);
  return tests_result;
}

bool test_or_different_keys(size_t n,unsigned int seed){
  printf("\n========== %s (n=%lu) ==========\n",
         __PRETTY_FUNCTION__, n);
  bool tests_result = true;
  dictionary_t *dict1 = dictionary_create(free);
  tests_result &= test_assert("El diccionario 1 fue creado", dict1 != NULL);
  dictionary_t *dict2 = dictionary_create(free);
  tests_result &= test_assert("El diccionario 2 fue creado", dict2 != NULL);
  bool insert = true;
  for (int i = 0; i < n; i++) {
    srand(seed+i);
    int random_number = rand();
    char *str = generate_random_string(5);
    int *random_number_copy = malloc(sizeof(int));
    *random_number_copy = random_number;
    insert &= dictionary_put(dict1, str, random_number_copy);
    free(str);
  }
  tests_result &= test_assert("Todas las inserciones en dict1 fueron exitosas", insert);
  for (int i = 0; i < n; i++) {
    srand(seed+i+2);
    int random_number = rand();
    char *str = generate_random_string(5);
    int *random_number_copy = malloc(sizeof(int));
    *random_number_copy = random_number+25;
    insert &= dictionary_put(dict2, str, random_number_copy);
    free(str);
  }
  tests_result &= test_assert("Todas las inserciones en dict2 fueron exitosas", insert);
  dictionary_t* result= dictionary_or(dict1,dict2);
  tests_result &= test_assert("dictionary_or funcionó", result != NULL);
  
  bool test_val = true;
  bool test_or = true;
  for(size_t i = 0; i<result->size;i++){
    char* rst_key = result->buckets[i].key;
    void* rst_val = result->buckets[i].value;
    if(rst_key){
      if (!dictionary_contains(dict1,rst_key) && !dictionary_contains(dict2,rst_key)){
        test_or = false;
      }
      if (dictionary_contains(dict1,rst_key)&&dictionary_contains(dict2,rst_key)){
        bool err = true;
          if(rst_val != dictionary_get(dict1,rst_key,&err)){
              test_val = false;
          }
      }
    }    
  }
  tests_result &= test_assert("Todos los elementos pertencen a algún diccionario", test_or);
  tests_result &= test_assert("Los elementos repetidos tienen el valor del diccionario1.", test_val);

  dictionary_destroy(dict1);
  dictionary_destroy(dict2);
  delete_keys(result);
  dictionary_destroy(result);
  return tests_result; 
}

bool test_or_same_keys(size_t n,unsigned int seed){
  printf("\n========== %s (n=%lu) ==========\n",
         __PRETTY_FUNCTION__, n);
  bool tests_result = true;
  dictionary_t *dict1 = dictionary_create(free);
  tests_result &= test_assert("El diccionario 1 fue creado", dict1 != NULL);
  dictionary_t *dict2 = dictionary_create(free);
  tests_result &= test_assert("El diccionario 2 fue creado", dict2 != NULL);
  bool insert = true;
  for (int i = 0; i < n; i++) {
    srand(seed+i);
    int random_number = rand();
    char *str = generate_random_string(5);
    int *random_number_copy = malloc(sizeof(int));
    *random_number_copy = random_number;
    insert &= dictionary_put(dict1, str, random_number_copy);
    free(str);
  }
  tests_result &= test_assert("Todas las inserciones en dict1 fueron exitosas", insert);

  bool same_keys = true;
  for (int i = 0; i < n; i++) {
    srand(seed+i);
    int random_number = rand();
    char *str = generate_random_string(5);
    int *random_number_copy = malloc(sizeof(int));
    *random_number_copy = random_number+25;
    dictionary_put(dict2, str, random_number_copy);
    free(str);
  }
  for(int i = 0; i<dict2->size;i++){
    if(dict2->buckets[i].key && !dictionary_contains(dict1,dict2->buckets[i].key)){
      same_keys = false;
    }
  }
  tests_result &= test_assert("Dict2 tiene las mismas llaves que dict1", same_keys);
  dictionary_t* result= dictionary_or(dict1,dict2);
  tests_result &= test_assert("dictionary_or funcionó", result != NULL);
  
  bool test_equals = true;
  test_equals = dictionary_equals(dict1,result);
  tests_result &= test_assert("El resultado es igual al diccionario 1", test_equals);

  dictionary_destroy(dict1);
  dictionary_destroy(dict2);
  delete_keys(result);
  dictionary_destroy(result);
  return tests_result; 
}

bool test_and(size_t n, unsigned int seed){
  printf("\n========== %s (n=%lu, seed=%u) ==========\n",
         __PRETTY_FUNCTION__, n, seed);
  bool tests_result = true;
  dictionary_t *dict1 = dictionary_create(free);
  tests_result &= test_assert("El diccionario 1 fue creado", dict1 != NULL);
  dictionary_t *dict2 = dictionary_create(free);
  tests_result &= test_assert("El diccionario 2 fue creado", dict2 != NULL);
  bool insert = true;
  for (int i = 0; i < n; i++) {
    srand(seed+i);
    int random_number = rand();
    char *str = generate_random_string(5);
    int *random_number_copy = malloc(sizeof(int));
    *random_number_copy = random_number;
    insert &= dictionary_put(dict1, str, random_number_copy);
    free(str);
  }
  tests_result &= test_assert("Todas las inserciones en dict1 fueron exitosas", insert);
  for (int i = 0; i < n; i++) {
    srand(seed-2+i);
    int random_number = rand();
    char *str = generate_random_string(5);
    int *random_number_copy = malloc(sizeof(int));
    *random_number_copy = random_number+25;
    insert &= dictionary_put(dict2, str, random_number_copy);
    free(str);
  }
  tests_result &= test_assert("Todas las inserciones en dict2 fueron exitosas", insert);
  
  bool test_and = true;
  dictionary_t* result= dictionary_and(dict1,dict2);
  tests_result &= test_assert("dictionary_and funcionó", result != NULL);
  
  bool test_both = true;
  for(size_t i = 0; i<result->size;i++){
    char* rst_key = result->buckets[i].key;
    if(rst_key && !(dictionary_contains(dict1,rst_key) &&dictionary_contains(dict2,rst_key))){
        test_both =false;
    }
  }
  tests_result &= test_assert("todos los elementos se encuentran en ambos diccionarios", test_both);

  for(size_t i = 0; i<dict1->size;i++){
    bool err = true;
    if(dict1->buckets[i].key&&
      dictionary_contains(dict2,dict1->buckets[i].key) &&
      result->buckets[i].value != dictionary_get(dict1,result->buckets[i].key,&err)){
        test_and =false;
    }
  }
  tests_result &= test_assert("Se guardaron los valores del diccionario1.", test_and);

  dictionary_destroy(dict1);
  dictionary_destroy(dict2);
  delete_keys(result);
  dictionary_destroy(result);
  return tests_result;
}
bool test_and_null() {
    printf("\n========== %s ==========\n",
            __PRETTY_FUNCTION__);
    bool tests_result = true;
    srand(123);
    dictionary_t *dict1 = dictionary_create(free);
    tests_result &= test_assert("El diccionario 1 está vacío", dict1 != NULL);
    dictionary_t *dict2 = dictionary_create(free);
    tests_result &= test_assert("El diccionario 2  está vacío", dict2 != NULL);
    bool insert = true;
    for (size_t i = 0; i < 12; i++) {
        int random_number = rand();
        char *str = generate_random_string(5);
        int *random_number_copy = malloc(sizeof(int));
        *random_number_copy = random_number;
        insert &= dictionary_put(dict1, str, random_number_copy);
        free(str);
    }
    tests_result &= test_assert("Todas las inserciones en dict1 fueron exitosas", insert);
   dictionary_t* result= dictionary_and(dict1,dict2);
   tests_result &= test_assert("dictionary_and funcionó", result != NULL);
   bool test_both = true;
   for (size_t i = 0; i<result->size;i++){
    if(result->buckets[i].key){
        test_both = false;
    }
   }
   tests_result &= test_assert("El resultado está vacio", test_both);

  dictionary_destroy(dict1);
  dictionary_destroy(dict2);
  delete_keys(result);
  dictionary_destroy(result);
  return tests_result;
}

bool test_and_error(){
  printf("\n========== %s ==========\n", __PRETTY_FUNCTION__);
  bool tests_result = true;
  set_malloc_status(true);
  dictionary_t *dict1 = dictionary_create(free);
  tests_result &=test_assert("se creó el diccionario 1", dict1!=NULL);
  set_malloc_status(false);
  dictionary_t *dict2 = dictionary_create(free);
  tests_result &=test_assert("NO se creó el diccionario 2", !dict2);
  set_malloc_status(true);
  bool and = dictionary_and(dict1,dict2);
  tests_result &=  test_assert("dictionary_and falla", !and );
  dictionary_destroy(dict1);
  return tests_result;
}

bool test_update(size_t n, unsigned int seed) {
  printf("\n========== %s (n=%lu, seed=%u) ==========\n",
         __PRETTY_FUNCTION__, n, seed);
  srand(seed);
  bool tests_result = true;
  bool err;
  dictionary_t *dict1 = dictionary_create(free);
  tests_result &= test_assert("El diccionario 1 fue creado", dict1 != NULL);
  dictionary_t *dict2 = dictionary_create(free);
  tests_result &= test_assert("El diccionario 2 fue creado", dict2 != NULL);
  bool insert = true;
  for (size_t i = 0; i < n; i++) {
    int *random_number_copy = malloc(sizeof(int));
    *random_number_copy = (int)n+12;
    char *str = generate_random_string(5);
    insert &= dictionary_put(dict1, str, random_number_copy);
    free(str);
  }
  tests_result &= test_assert("Todas las inserciones en dict1 fueron exitosas", insert);
  for (size_t i = 0; i < n; i++) {
    char *str = generate_random_string(5);
    int *random_number_copy = malloc(sizeof(int));
    *random_number_copy = (int)n;
    insert &= dictionary_put(dict2, str, random_number_copy);
    free(str);
  }
  tests_result &= test_assert("Todas las inserciones en dict2 fueron exitosas", insert);
  
  bool correct_copy = true;
  correct_copy &= dictionary_update(dict1,dict2);
  tests_result &= test_assert("Se updateo el diccionario 1.", correct_copy);
  
  bool correct_value =true;
  bool contains_key = true;
  for (size_t i = 0; i < dict2->size; i++) {
    if (dict2->buckets[i].key && dict2->buckets[i].value) {
      if (dictionary_get(dict1, dict2->buckets[i].key, &err) != dict2->buckets[i].value) {
        correct_value = false;
      }
      contains_key &= dictionary_contains(dict1, dict2->buckets[i].key);
    }
  } 
  tests_result &= test_assert("Todas las llaves de dict2 están en dict1.", contains_key);
  tests_result &= test_assert("Todos los valores de dict2 están en dict1", correct_value);
  
  dictionary_destroy(dict1);
  delete_keys(dict2);
  dictionary_destroy(dict2);
  return tests_result;
}
bool test_update_error(){
  printf("\n========== %s ==========\n", __PRETTY_FUNCTION__);
  bool tests_result = true;
  set_malloc_status(true);
  dictionary_t *dict1 = dictionary_create(free);
  tests_result &=test_assert("se creó el diccionario 1", dict1!=NULL);
  set_malloc_status(false);
  dictionary_t *dict2 = dictionary_create(free);
  tests_result &=test_assert("NO se creó el diccionario 2", !dict2);
  set_malloc_status(true);
  bool update = dictionary_update(dict1,dict2);
  tests_result &=  test_assert("dictionary_update falla", !update );
  dictionary_destroy(dict1);
  return tests_result;
}

bool test_update_different_size(size_t n, unsigned int seed){
printf("\n========== %s (n=%lu, seed=%u) ==========\n",
         __PRETTY_FUNCTION__, n, seed);
  srand((unsigned int)time(NULL));
  bool tests_result = true;
  bool err;
  dictionary_t *dict1 = dictionary_create(free);
  tests_result &= test_assert("El diccionario 1 fue creado", dict1 != NULL);
  dictionary_t *dict2 = dictionary_create(free);
  tests_result &= test_assert("El diccionario 2 fue creado", dict2 != NULL);
  for (size_t i = 0; i < 1000; i++) {
    char *str = generate_random_string(5);
    int *random_number_copy = malloc(sizeof(int));
    *random_number_copy = (int)n+12;
    dictionary_put(dict1, str, random_number_copy);
    free(str);
  }
  tests_result &= test_assert("El diccionario 1 tiene 1000 elementos", (dict1->used_buckets == 1000));
  for (size_t i = 0; i < n; i++) {
    int random_number = rand();
    char *str = generate_random_string(5);
    int *random_number_copy = malloc(sizeof(int));
    *random_number_copy = random_number;
    dictionary_put(dict2, str, random_number_copy);
    free(str);
  }
  tests_result &= test_assert("El diccionario2 tiene 512 elementos", (dict2->used_buckets ==512));
  bool correct_copy = true;
  correct_copy &= dictionary_update(dict1,dict2);
  tests_result &= test_assert("Se updateo el diccionario 1.", correct_copy);
  tests_result &= test_assert ("EL diccionario1 tiene 1512 elementos", (dict1->used_buckets==1512));
  bool correct_value =true;
  bool contains_key = true;
  for (size_t i = 0; i < dict2->size; i++) {
    if (dict2->buckets[i].key && dict2->buckets[i].value) {
      if (dictionary_get(dict1, dict2->buckets[i].key, &err) != dict2->buckets[i].value) {
        correct_value = false;
      }
      contains_key &= dictionary_contains(dict1, dict2->buckets[i].key);
    }
  } 
  tests_result &= test_assert("Todas las llaves de dict2 están en dict1.", contains_key);
  tests_result &= test_assert("Todos los valores de dict2 están en dict1", correct_value);
  
  dictionary_destroy(dict1);
  delete_keys(dict2);
  dictionary_destroy(dict2);
  return tests_result;
}

int main_operable_dict(void) {
  srand(117);
  int return_code = 0;
  return_code += !test_update(536, 117);
  return_code += !test_update_different_size(512,117);
  return_code += !test_update_error();

  return_code += !test_and(512,117);
  return_code += !test_and_null();
  return_code += !test_and_error();

  return_code += !test_or_same_keys(512,117);
  return_code += !test_or_different_keys(512,117);
  return_code += !test_or_null ();
  return_code += !test_or_error();

  return_code += !test_equals(512,117);
  return_code += !test_equals_null();
  return_code += !test_equals_different();
  return_code += !test_equals_error();

  return_code += !test_update(2048, 117);
  return_code += !test_and(2048,117);
  return_code += !test_or_different_keys(2048,117);
  return_code += !test_equals(2048,117);
  exit(return_code);
  return return_code;
}
