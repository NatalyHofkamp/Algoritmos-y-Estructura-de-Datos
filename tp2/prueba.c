#include "tp2.h"
#include <stdlib.h>
#include <stdbool.h>
#include "test_malloc.h"
#include "testing.h"


struct node;
typedef struct node node_t;

struct node {
    void* value;
    node_t* next;
    node_t* prev;
};

struct list {
    node_t* head;
    node_t* tail;
    size_t size;
};

struct list_iter {
    list_t* list;
    node_t* curr;
};

/*
 * Crea una nueva lista.
 * Devuelve NULL si falla.
 */
list_t *list_new(){
    list_t* aux=(list_t*) malloc (sizeof(list_t));
    if(!aux){
        return NULL;
    }
    return aux;
}
/*
 * Destruye una lista liberando toda su memoria. Recibe por parametro una función para liberar a
   sus miembros.
 * Pre-condiciones: La lista existe.
 * Post-condiciones: La memoria de la lista está liberada junto con la de sus elementos.
 * El puntero a lista ya no puede usarse. Si la función es NULL no la utiliza.
 */
// void list_destroy(list_t *list, void destroy_value(void *)){

//     return;
// }


int main(void) {
    bool tests_result = true;
    list_t* list = list_new();
    tests_result &= test_assert("La lista fue creada", list != NULL);
    // list_destroy(list, NULL);
    return tests_result;
}
