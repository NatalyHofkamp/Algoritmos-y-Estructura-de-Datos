#include "tp2.h"
#include <stdbool.h>
#include <stdlib.h>

//HACER ANTES DE ENTREGAR: 
//* cambiar los list->size por la función list_lenght
//*sacar todos los comentarios de tp2.h

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

list_t *list_new(){
    list_t* new_list= (list_t*)malloc(sizeof(list_t));
    if(!new_list) return NULL;
    new_list->tail=NULL;
    new_list->head=NULL;
    new_list->size=0;
    return new_list;

}
/*
 * Devuelve el largo de la lista.
 * Pre-condiciones: La lista existe.
 */
size_t list_length(const list_t *list){
    return list->size;
}

bool list_is_empty(const list_t *list){
    if(list->size==0) return true;
    return false;
}
/*
 * Inserta un elemento al principio de la lista.
 * Pre-condiciones: La lista existe.
 * Post-condiciones: Devuelve TRUE si funcionó e insertó un elemento.
 */
bool list_insert_head(list_t *list, void *value){
    node_t* node= (node_t*)malloc(sizeof(node_t));
    if (!node)return NULL;
    node->value=value;
    node->prev=NULL;
    node_t* aux=list->head;
    if(list_is_empty(list)){
        node->next= NULL;
        list->head=node;
        list->tail=node;
        list->size=1;
        return true;
    }
    else{
        aux->prev=node;
        node->next=aux;
        list->head=node;
        list->size++;
        return true;
    }
    return false;
}

/*
 * Inserta un elemento al final de la lista.
 * Pre-condiciones: La lista existe.
 * Post-condiciones: Devuelve TRUE si funcionó e insertó un elemento.
 */
bool list_insert_tail(list_t *list, void *value){
    node_t* node= (node_t*)malloc(sizeof(node_t));
    if (!node)return NULL;
    node->value=value;
    node->next=NULL;
    node_t* aux=list->tail;
    if(list_is_empty(list)){
        node->prev=NULL;
        list->tail=node;
        list->head=node;
        list->size=1;
        return true;
    }
    else{
        aux->next=node;
        node->prev=aux;
        list->tail=node;
        list->size++;
        return true;
    }
    return false;
}
/*
 * Devuelve el puntero al dato que está en el principio de la lista.
 * Pre-condiciones: La lista existe.
 * Post-condiciones: Devuelve el primer valor de la lista, NULL si la lista está vacia.
 */
void *list_peek_head(const list_t *list){
    if(!list_is_empty(list)) return list->head->value;
    return NULL;
}

void *list_peek_tail(const list_t *list){
    if(!list_is_empty(list))return list->tail->value;
    return NULL;
}

/*
 * Elimina un elemento del principio de la lista y devuelve el puntero al dato que almacenaba.
 * Pre-condiciones: La lista existe.
 * Post-condiciones: Devuelve el dato y saca un nodo del principio de la lista
 * si la lista no está vacia. Devuelve NULL si está vacia.
 */
void *list_pop_head(list_t *list){
    if(list_is_empty(list))return NULL;
    void* value=list->head->value;
    if(list->head->next){
        node_t* next=list->head->next;
        free(list->head);
        list->head=next;
        list->head->prev=NULL;
        list->size--;
    }
    else{
        free(list->head);
        list->tail=NULL;
        list->head=NULL;
        list->size=0;
    }
    return value;
}
void *list_pop_tail(list_t *list){
    if(list_is_empty(list)) return NULL;
    void* value=list->tail->value;
    if(list->tail->prev){
        node_t* previous=list->tail->prev;
        free(list->tail);
        list->tail=previous;
        list->size--;
        list->tail->next=NULL;
    }
    else{
        free(list->tail);
        list->tail=NULL;
        list->head=NULL;
        list->size=0;
    }
    return value;
}
/*
 * Destruye una lista liberando toda su memoria. Recibe por parametro una función para liberar a sus miembros.
 * Pre-condiciones: La lista existe.
 * Post-condiciones: La memoria de la lista está liberada junto con la de sus elementos.
 * El puntero a lista ya no puede usarse. Si la función es NULL no la utiliza.
 */
void list_destroy(list_t *list, void destroy_value(void *)){
    if(list_is_empty(list)) return free(list);
    node_t* prev=list->tail->prev;
    if(!destroy_value){
        while(list->tail){
            prev=list->tail->prev;
            free(list->tail);
            list->tail=prev;
            list->size--;
        }
    } 
    else if (destroy_value){
        while(list->tail){
            destroy_value(list->tail->value);
            prev=list->tail->prev;
            destroy_value(list->tail);
            list->tail=prev;
            list->size--;
        }
    }
    free(list);
}

/*
 * Primitivas de iterador externo
 */

/*
 * Devuelve un nuevo iterador
 * Pre: La lista está creada
 * Post: El iterador está en el principio de la lista. Devuelve NULL si falla.
 */
list_iter_t *list_iter_create_head(list_t *list){
    list_iter_t* new_iter= (list_iter_t*)malloc(sizeof(list_iter_t));
    if(!new_iter) return NULL;
    new_iter->list=list;
    new_iter->curr=new_iter->list->head;
    return new_iter;
    
    
}

list_iter_t *list_iter_create_tail(list_t *list){
     list_iter_t* new_iter= (list_iter_t*)malloc(sizeof(list_iter_t));
    if(!new_iter) return NULL;
    new_iter->list=list;
    new_iter->curr=new_iter->list->tail;
    return new_iter;
}

/*
 * Avanza el iterador una posición
 * Pre: El iterador está creado
 * Post: El iterador avanza una posición. Si no puede avanzar más devuelve false, sino true.
 */

bool list_iter_forward(list_iter_t *iter){
    if(!(iter->curr) || !(iter->curr->next)) return false;
    iter->curr=iter->curr->next;
    return true;
}
/*
 * Avanza el iterador una posición hacia atras
 * Pre: El iterador está creado
 * Post: El iterador va una posición hacia atras. Si no puede devuelve false, sino true.
 */
bool list_iter_backward(list_iter_t *iter){
    if(!(iter->curr) || !(iter->curr->prev)) return false;
    iter->curr=iter->curr->prev;
    return true;
}
/*
 * Devuelve el valor en la posición actual.
 * Pre: El iterador está creado
 * Post: Devuelve NULL si la lista está vacia, sino el valor.
 */
void *list_iter_peek_current(const list_iter_t *iter){
    if(list_is_empty(iter->list)) return NULL;
    return iter->curr->value;
}
/*
 * Evalua si el iterador está al final
 * Pre: El iterador está creado
 * Post: Devuelve true si el iterador está al final, sino false.
 * Si la lista está vacia es true.
 */
bool list_iter_at_last(const list_iter_t *iter){
    if(list_is_empty(iter->list)||!(iter->curr->next)) return true;
    return false;
}

bool list_iter_at_first(const list_iter_t *iter){
    if(list_is_empty(iter->list)||!(iter->curr->prev)) return true;
    return false;
}
/*
 * Destruye el iterador externo
 * Pre: El iterador fue creado
 * Post: El iterador está destruido
 */
void list_iter_destroy(list_iter_t *iter){
    free(iter);
}
/*
 * Agrega un elemento despues del elemento actual del iterador.
 * Pre: El iterador fue creado
 * Post: Agrega un nuevo elemento despues del actual en la lista. Devuelve true si tuvo éxito.
 * No altera la posición del iterador. Devuelve false si falla.
 */
bool list_iter_insert_after(list_iter_t *iter, void *value){
    if(list_is_empty(iter->list)) return list_insert_head(iter->list,value);
    if(!iter->curr->next) return list_insert_tail(iter->list,value);
    node_t* new_node= (node_t*)malloc(sizeof(node_t));
    if (!new_node)return false;
    new_node->value=value;
    new_node->prev=iter->curr;
    new_node->next=iter->curr->next;
    iter->curr->next=new_node;
    iter->list->size++;
    return true;

}

bool list_iter_insert_before(list_iter_t *iter, void *value){
    if(list_is_empty(iter->list))return list_insert_head(iter->list,value);
    if(!iter->curr->prev)return list_insert_head(iter->list,value);
    node_t* new_node= (node_t*)malloc(sizeof(node_t));
    if (!new_node)return false;
    new_node->value=value;
    new_node->next=iter->curr;
    new_node->prev=iter->curr->prev;
    iter->curr->prev=new_node;
    iter->list->size++;
    return true;
}

void *list_iter_delete(list_iter_t *iter){
    return NULL;
}