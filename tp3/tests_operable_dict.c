#include "test_malloc.h"
#include "testing.h"
#include "operable_dict.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char* generate_random_string(int length) {
    static const char charset[] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
    char* random_string = malloc(length + 1);
    
    if (random_string == NULL) {
        printf("Error: No se pudo asignar memoria para el string aleatorio.\n");
        return NULL;
    }

    srand(117);
    
    for (int i = 0; i < length; i++) {
        size_t index = rand() % (sizeof(charset) - 1);
        random_string[i] = charset[index];
    }
    
    random_string[length] = '\0';
    
    return random_string;
}

bool test_dictionary_equals(size_t n, unsigned int seed){
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
  tests_result &= test_assert("El diccionario 2 fue creado", dict2 != NULL);
  
  bool test_equals = true;
  test_equals = dictionary_equals(dict1,dict2);
  tests_result &= test_assert("Los diccionarios son iguales", test_equals);

  dictionary_destroy(dict1);
  dictionary_delete_keys(dict2);
  dictionary_destroy(dict2);
  return tests_result;
}

bool test_dictionary_or(size_t n,unsigned int seed){
  printf("\n========== %s (n=%lu, seed=%u) ==========\n",
         __PRETTY_FUNCTION__, n, seed);
  srand(seed);
  bool tests_result = true;
  dictionary_t *dict1 = dictionary_create(free);
  tests_result &= test_assert("El diccionario 1 fue creado", dict1 != NULL);
  dictionary_t *dict2 = dictionary_create(free);
  tests_result &= test_assert("El diccionario 2 fue creado", dict2 != NULL);
  bool insert = true;
  for (size_t i = 0; i < n; i++) {
    int random_number = rand();
    char *str = generate_random_string(5);
    int *random_number_copy = malloc(sizeof(int));
    *random_number_copy = random_number;
    insert &= dictionary_put(dict1, str, random_number_copy);
    free(str);
  }
  tests_result &= test_assert("Todas las inserciones en dict1 fueron exitosas", insert);
  for (size_t i = 0; i < n; i++) {
    srand(seed);
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
          if(rst_val != dict1->buckets[i].value){
              test_val = false;
          }
      }
    }    
  }
  tests_result &= test_assert("Todos los elementos pertencen a algún diccionario", test_or);
  tests_result &= test_assert("Los elementos repetidos tienen el valor del diccionario1.", test_val);

  dictionary_destroy(dict1);
  dictionary_destroy(dict2);
  dictionary_delete_keys(result);
  free(result->buckets);
  free(result);
  return tests_result; 
}

bool test_dictionary_and(size_t n, unsigned int seed){
  printf("\n========== %s (n=%lu, seed=%u) ==========\n",
         __PRETTY_FUNCTION__, n, seed);
  srand(seed);
  bool tests_result = true;
  dictionary_t *dict1 = dictionary_create(free);
  tests_result &= test_assert("El diccionario 1 fue creado", dict1 != NULL);
  dictionary_t *dict2 = dictionary_create(free);
  tests_result &= test_assert("El diccionario 2 fue creado", dict2 != NULL);
  bool insert = true;
  for (size_t i = 0; i < n; i++) {
    int random_number = rand();
    char *str = generate_random_string(5);
    int *random_number_copy = malloc(sizeof(int));
    *random_number_copy = random_number;
    insert &= dictionary_put(dict1, str, random_number_copy);
    free(str);
  }
  tests_result &= test_assert("Todas las inserciones en dict1 fueron exitosas", insert);
  for (size_t i = 0; i < n; i++) {
    srand(seed);
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
    if(dict1->buckets[i].key&&
      dictionary_contains(dict2,dict1->buckets[i].key) &&
      result->buckets[i].value != dict1->buckets[i].value){
        test_and =false;
    }
  }
  tests_result &= test_assert("Se guardaron todos los valores del diccionario1.", test_and);

  dictionary_destroy(dict1);
  dictionary_destroy(dict2);
  dictionary_delete_keys(result);
  free(result->buckets);
  free(result);
  return tests_result;
}
bool test_dictionary_equals_failed(size_t n, unsigned int seed) {
  printf("\n========== %s (n=%lu, seed=%u) ==========\n",
         __PRETTY_FUNCTION__, n, seed);
  srand(seed);
  bool tests_result = true;
  dictionary_t *dict1 = dictionary_create(free);
  tests_result &= test_assert("El diccionario 1 fue creado", dict1 != NULL);
  dictionary_t *dict2 = dictionary_create(free);
  tests_result &= test_assert("El diccionario 2 fue creado", dict2 != NULL);
   for (size_t i = 0; i < n; i++) {
    int random_number = rand();
    char *str = generate_random_string(5);
    int *random_number_copy = malloc(sizeof(int));
    *random_number_copy = random_number;
    free(str);
  }
  dictionary_t* copy = dictionary_copy(dict1);
  for (size_t i = 0; i < n; i++) {
    srand(seed+259);
    int random_number = rand();
    char *str = generate_random_string(5);
    int *random_number_copy = malloc(sizeof(int));
    *random_number_copy = random_number;
    free(str);
  }

  set_malloc_status(false);
  bool correct_copy = true;
  correct_copy &= dictionary_update(dict1,dict2);
  tests_result &= test_assert("Los arreglos no se copiaron.",!correct_copy);
  set_malloc_status(true);

  bool test_equals = true;
  test_equals&= dictionary_equals(dict1,copy);
  tests_result &= test_assert("El diccionario 1 sigue igual.", test_equals);
  
  dictionary_destroy(dict1);
  dictionary_destroy(dict2);
  dictionary_delete_keys(copy);
  free(copy);
  return tests_result;
}
bool test_dictionary_update(size_t n, unsigned int seed) {
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
    int random_number = rand();
    char *str = generate_random_string(5);
    int *random_number_copy = malloc(sizeof(int));
    *random_number_copy = random_number;
    insert &= dictionary_put(dict1, str, random_number_copy);
    free(str);
  }
  tests_result &= test_assert("Todas las inserciones en dict1 fueron exitosas", insert);
  for (size_t i = 0; i < n; i++) {
    srand(seed+259);
    int random_number = rand();
    char *str = generate_random_string(5);
    int *random_number_copy = malloc(sizeof(int));
    *random_number_copy = random_number;
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
  dictionary_delete_keys(dict2);
  dictionary_destroy(dict2);
  return tests_result;
}
int main_operable_dict(void) {
  srand(117);
  int return_code = 0;
  return_code += !test_dictionary_update(536, 117);
  return_code += !test_dictionary_update(2048, 117);
  // return_code += !test_dictionary_equals_failed (512,117);
  return_code += !test_dictionary_and(512,117);
  return_code += !test_dictionary_and(2048,117);
  return_code += !test_dictionary_or(512,117);
  return_code += !test_dictionary_or(2048,117);
  return_code += !test_dictionary_equals(512,117);
  return_code += !test_dictionary_equals(2048,117);
  exit(return_code);
  return return_code;
}
