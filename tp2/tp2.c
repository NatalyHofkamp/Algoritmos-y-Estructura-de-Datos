#include "tp2.h"
#include <stdbool.h>
#include <stdlib.h>
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

list_t *list_new() {
    list_t* new_list = (list_t*)malloc(sizeof(list_t));
    if (!new_list)return NULL;
    new_list->tail = NULL;
    new_list->head = NULL;
    new_list->size = 0;
    return new_list;

}
size_t list_length(const list_t *list) { return list->size; }

bool list_is_empty(const list_t *list) {
    if(list_length(list) == 0) return true;
    return false;
}
bool list_insert_head(list_t *list, void *value) {
    node_t* node = (node_t*)malloc(sizeof(node_t));
    if (!node) return false;
    node->value = value;
    node->prev = NULL;
    if(list_is_empty(list)){
        node->next = NULL;
        list->head = node;
        list->tail = node;
        list->size = 1;
    } else {
        node_t* aux=list->head;
        aux->prev = node;
        node->next = aux;
        list->head = node;
        list->size++;
    }
    return true;
}
bool list_insert_tail(list_t *list, void *value) {
    node_t* node = (node_t*)malloc(sizeof(node_t));
    if (!node)return false;
    node->value = value;
    node->next = NULL;
    if(list_is_empty(list)){
        node->prev = NULL;
        list->tail = node;
        list->head = node;
        list->size = 1;
    }
    else{
        node_t* aux = list->tail;
        aux->next = node;
        node->prev = aux;
        list->tail = node;
        list->size++;
    }
    return true;
}
void *list_peek_head(const list_t *list){
    if(!list_is_empty(list)) return list->head->value;
    return NULL;
}
void *list_peek_tail(const list_t *list){
    if(!list_is_empty(list))return list->tail->value;
    return NULL;
}
void *list_pop_head(list_t *list){
    if(list_is_empty(list))return NULL;
    void* value = list->head->value;
    if(list->head->next){
        node_t* next = list->head->next;
        next->prev = NULL;
        free(list->head);
        list->head = next;
        list->size--;
    } else {
        free(list->head);
        list->tail = NULL;
        list->head = NULL;
        list->size = 0;
    }
    return value;
}
void *list_pop_tail(list_t *list){
    if(list_is_empty(list)) return NULL;
    void* value = list->tail->value;
    if(list->tail->prev){
        node_t* prev = list->tail->prev;
        prev->next = NULL;
        free(list->tail);
        list->tail = prev;
        list->size--;
    } else {
        free(list->tail);
        list->tail = NULL;
        list->head = NULL;
        list->size = 0;
    }
    return value;
}
void list_destroy(list_t *list, void destroy_value(void *)){ //preguntar-> tengo que hacer el free al final? caso lista vacía
    if (destroy_value ){
        while(list->tail){
            destroy_value(list_pop_tail(list));
        }
    } else{
        while(list->tail){
            list_pop_tail(list);
        }
    }
    free(list);
}
list_iter_t *list_iter_create_head(list_t *list){
    list_iter_t* new_iter = (list_iter_t*)malloc(sizeof(list_iter_t));
    if (!new_iter)return false;
    new_iter->list = list;
    new_iter->curr = list->head;
    return new_iter;
}
list_iter_t *list_iter_create_tail(list_t *list){
    list_iter_t* new_iter = (list_iter_t*)malloc(sizeof(list_iter_t));
    if (!new_iter)return false;
    new_iter->list = list;
    new_iter->curr = list->tail;
    return new_iter;
}
bool list_iter_forward(list_iter_t *iter){
    if(!(iter->curr) || list_iter_at_last(iter)) return false;
    iter->curr = iter->curr->next;
    return true;
}
bool list_iter_backward(list_iter_t *iter){
    if(!(iter->curr) || list_iter_at_first(iter)) return false;
    iter->curr = iter->curr->prev;
    return true;
}
void *list_iter_peek_current(const list_iter_t *iter){
    if(list_is_empty(iter->list)) return NULL;
    return iter->curr->value;
}
bool list_iter_at_last(const list_iter_t *iter){
    if(list_is_empty(iter->list)||!iter->curr->next) return true;
    return false;
}

bool list_iter_at_first(const list_iter_t *iter){
    if(list_is_empty(iter->list)||!iter->curr->prev) return true;
    return false;
}
void list_iter_destroy(list_iter_t *iter){
    free(iter);
}
bool list_iter_insert_after(list_iter_t *iter, void *value){
    if(list_is_empty(iter->list) || list_iter_at_last(iter) ){
        list_insert_tail(iter->list,value);
        iter->curr = iter->list->tail;       
    } else {
        node_t* new_node = (node_t*)malloc(sizeof(node_t));
        if (!new_node)return false;
        new_node->value = value;
        new_node->prev = iter->curr;
        new_node->next = iter->curr->next;
        iter->curr->next->prev = new_node;
        iter->curr->next = new_node;
        iter->list->size++;
    }
    return true;
}
bool list_iter_insert_before(list_iter_t *iter, void *value){
    if(list_is_empty(iter->list) || list_iter_at_first(iter) ){
        list_insert_head(iter->list,value);
         iter->curr = iter->list->head;
    } else{
        node_t* new_node= (node_t*) malloc (sizeof(node_t));
        if (!new_node)return false;
        new_node->value = value;
        new_node->next = iter->curr;
        new_node->prev = iter->curr->prev;
        iter->curr->prev->next = new_node;
        iter->curr->prev = new_node;
        iter->list->size++;
    }
    return true;
}
void *list_iter_delete(list_iter_t *iter){
     if(list_is_empty(iter->list)) return NULL;
     if(list_iter_at_last(iter)){
        list_iter_backward(iter);
        return list_pop_tail(iter->list);
     }
     else if(list_iter_at_first(iter)){
        list_iter_forward(iter);
        return list_pop_head(iter->list);
     }
     node_t* next = iter->curr->next;
     next->prev = iter->curr->prev;
     iter->curr->prev->next=next;
     void* value = list_iter_peek_current(iter);
     free(iter->curr);
     iter->curr = next;
     return value;
}