// #include "internal_iterator.h"
// #include "test_malloc.h"
// #include "testing.h"
// #include <stdbool.h>
// #include <stdio.h>
// #include <stdlib.h>
// #include <string.h>

// bool (*iterate_f)(const char* key, void* value, void* extra){
//   if(!key) return false;
  
// }

// // Acá implementa tus tests
// bool test_equals_null(){
//   printf("\n========== %s ==========\n", __PRETTY_FUNCTION__);
//   bool tests_result = true;
//   dictionary_t *dict1 = dictionary_create(free);
//   tests_result &= test_assert("El diccionario fue creado", dict1 != NULL);
//   bool test_iterate = true;
//   test_iterate = iterate(dict1,f,extra);
//   tests_result &= test_assert("Los diccionarios son iguales", test_iterate);
//   dictionary_destroy(dict1);
//   return tests_result;
// }


// int main(void) {
//   srand(117);
//   int return_code = 0;
//   return_code += 1; // Agrega tus propios tests acá
//   return return_code;
// }

