#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
typedef struct node node_t;

typedef struct list list_t;
// struct node{
//     int val;
//     struct node* left;
//     struct node* right;
// };


// node_t* create_node (int val){
//     node_t* node= (node_t*) malloc(sizeof(node_t));
//     node->val=val;
//     node->left=NULL;
//     node->right=NULL;
//     return node;
//  }
// // bool is_bst(node_t* node){
// //     if (!node) return true;
// //     if (node->right && node->right->val<node->val) return false;
// //     if (node->left && node->left->val>=node->val ) return false;
// //     return (is_bst(node->left)&&is_bst(node->right));
// // }

// bool is_heap(node_t* node){
//     if (!node) return true;
//     if(node->left && node->left->val >node->val) return false;
//     if(node->right && node->right->val> node ->val) return false;
//     return (is_heap(node->right) && is_heap(node->left));
// }

// int main(void){
//     node_t* nodo1 = create_node(10);
//     nodo1->left = create_node(6);
//     nodo1->right = create_node(13);
//     nodo1->left->left=create_node(2);
//     nodo1->left->right = create_node(7);
//     nodo1->right->left = create_node(11);
//     nodo1->right->right=create_node(24);
//   //  printf("es un binary search tree? -> %d\n",is_bst(nodo1));
//     printf("es un heap? -> %d\n",is_heap(nodo1));

// }


//---------------------------------------------------------------------------------------------------


// Dada una lista simplemente enlazada crear una función que elimine el “n-ésimo” nodo
// contando desde el último elemento. (por ejemplo si n =0 tiene que eliminar el último nodo, si
// n=1 tiene que eliminar el ante-último nodo, etc).
// Codificar la función bool remove_nth_last(struct Node* head) en O(n).
// Muchos puntos extras: Hacer la función recorriendo la lista una sola vez

// struct node{
//     int val;
//     node_t* next;
// };

// struct list{
//     node_t* head;
//     int size;
// };

// node_t* create_node(int val){
//     node_t* node= (node_t*) malloc(sizeof(node_t));
//     node->val=val;
//     node->next=NULL;
//     return node;
// }

// list_t* list_new() {
//     list_t* new_list = (list_t*)malloc(sizeof(list_t));
//     if (!new_list)return NULL;
//     new_list->head = NULL;
//     new_list->size = 0;
//     return new_list;
// }

// void remove_nth_last(node_t* head,int n){
//     node_t* aux = head;
//     node_t* curr = head;
//     node_t* prev;
//     for(int i=0;i<n;i++){
//         aux=aux->next;
//     }
//     while(aux->next){
//         prev = curr;
//         curr = curr->next;
//         aux = aux->next;
//     }
//     prev->next=curr->next;
//     free(curr);
// }

// void print_list(node_t* node){
//     while(node->next){
//         printf("%i  ",node->val);
//         node=node->next;
//     }
//     printf("\n");
// }
// int main(void){
//     node_t* nodo1 = create_node(5);
//     node_t* nodo2 = create_node(6);
//     nodo1->next=nodo2;
//     node_t* nodo3 = create_node(3);
//     nodo2->next=nodo3;
//     node_t* nodo4 = create_node(8);
//     nodo3->next=nodo4;
//     node_t* nodo5 = create_node(4);
//     nodo4->next=nodo5;
//     node_t* nodo6 = create_node(2);
//     nodo5->next = nodo6;
//     print_list(nodo1);
//     remove_nth_last(nodo1,3);
//     print_list(nodo1);
// }
//------------------------------------------------------------------------------------------------
// Implementar una cola utilizando un arreglo dinámico. Pensar esta
// estructura como una interfaz amigable a un arreglo en memoria dinámica, de modo que los
// llamados a malloc y realloc sean transparentes al usuario.
// int* insert(int* cola, int size, int num) {
//     cola = (int*)realloc(cola, (size ) * sizeof(int));
//     cola[size] = num;
//     return cola;
// }

// void print_cola(int* cola, int size) {
//     printf("[");
//     for (int i = 0; i < size; i++) {
//         printf("%i ", cola[i]);
//     }
//     printf("]");
//     printf("\n");
// }

// int main(void){
//     int* cola = (int*)malloc(sizeof(int));
//     int size =1;
//     int num=1;
//     print_cola(cola, size);
//     while(num>0){
//         printf("Ingrese un número: ");
//         scanf("%d", &num);
//         cola = insert(cola, size, num);
//         size++;
//         print_cola(cola, size);
//     }
//     free(cola);
// }

//----------------------------------------------------------------------------------------------
// Implementar el algoritmo heapify y dar su complejidad temporal.
// 1. Utilizando heapify up

struct node{
    int val;
    struct node* left;
    struct node* right;
};

void print_cola(int* cola, int size) {
    printf("[");
    for (int i = 0; i < size; i++) {
        printf("%i ", cola[i]);
    }
    printf("]");
    printf("\n");
}
int* heapify_down(int* array,int num,int size){
    int root = num;
    int largest= root;
    int left = 2*root+1;
    int right = 2*root+2;

    if (left <size && array[largest]<array[left]){
            largest=left;
    }
    if (right < size && array[largest]<array[right]){
        largest=right;
    }
    if(root!= largest){
        int temp=array[root];
        array[root]= array[largest];
        array[largest]=temp;
        heapify_down(array,largest,size);
    }
}
void heap_sort(int* array, int size) {
    // Construye el árbol heap
    for (int i = size / 2 - 1; i >= 0; i--) {
        heapify_down(array, i, size);
    }

    // Extrae los elementos del heap
    for (int i = size - 1; i >= 0; i--) {
        // Intercambia el primer y último elemento del heap
        int temp = array[0];
        array[0] = array[i];
        array[i] = temp;

        // Restaura la propiedad del heap llamando heapify_down en la raíz
        heapify_down(array, 0, i);
    }
}


int main (void){
    int* array = (int*)malloc(sizeof(int)*10);
    int i=0;
    while(i<10){
        if(i%2==0){
            array[i]=2*i+5;
        }
        else{ array[i]=4*i-6;}
        i++;
    }
    print_cola(array,10);
    heap_sort(array,10);
    print_cola(array,10);
}