#include <stdio.h>
#include <stdlib.h>


/* Cómo crear una lista*/

typedef struct nodo{
    struct nodo* Next;
    int size;
    char* nombre;
}Lista;

Lista* listaLista(Lista* lista){
    lista=NULL;
    return lista;
}

void print_Lista(Lista* lista){
    Lista* aux= lista;
    while(aux){
        printf("%s \n",lista->nombre);
        aux=aux->Next;
    }
}

Lista* agregarLista(Lista* lista, char* nombre){
    Lista* nuevoLista= (Lista*) malloc(sizeof(Lista));
    nuevoLista->nombre=nombre;
    nuevoLista->Next=NULL;
    if(NULL== lista){//paso para agregar el primer Lista
        lista=nuevoLista;
        lista->size++;
    }
    else{//paso para cuando ya tenemos Listas
        Lista* aux=lista;
        while(aux->Next!=NULL){//si no apunta a nulo, entonces movete al próximo elemento
            aux=aux->Next;
        }
        aux->Next=nuevoLista;//estamos parados en el último[porque tiene puntero a NULL]
                             //le agregamos al último, un siguiente nodo(que ya tiene nombre).
        lista->size++;
    }
    return lista;
}

Lista* BuscarPosicion(Lista* lista, int n){
    if(NULL==lista){
        return NULL;
    }
    else{
        Lista* aux=lista;
        int posicion=0;
        while(posicion<n && aux->Next){
            aux=aux->Next;
            posicion++;
        }
        return aux;
    }
}

Lista* insertarLista (Lista* lista,int n,char* nombre){
    Lista* nuevoLista= (Lista*) malloc(sizeof(Lista*));
    nuevoLista->nombre=nombre;
    if(NULL==lista){
        return NULL;
    }
    else{
        int posicion=0;
        Lista* aux=lista;//guardamos la lista como está
        while(posicion<n && aux->Next){//recorremos hasta llegar a la posición que me dieron
            aux=aux->Next;
            posicion++;
        }
        nuevoLista->Next=aux->Next;//el Lista que queremos colocar apunta al que ya estaba en esta posición
        aux->Next=nuevoLista;//apuntamos el anterior al libro nuevo
        lista->size++;
        return aux;//devolvemos la lista con el orden que queremos
    }
}

void unir_dos_listas(Lista* l1, Lista* l2){
    for(int i=0; i<l2->size;i++){
        agregarLista(l1,BuscarPosicion(l2,i)->nombre);
    }
    return;
}

int main(void){
    Lista* lista1 =listaLista(lista1);
    lista1 =agregarLista(lista1,"Harry Potter");
    lista1=agregarLista(lista1,"Lord of The Rings");
    // while (lista!=NULL){
    //     printf("%s\n",lista->nombre);//empezamos en el primer Lista
    //     lista=lista->Next;//pedimos que el puntero cambie al próximo elemento
    // }
    lista1= insertarLista(lista1,1,"Call me by your name");
    print_Lista(lista1);
    // Lista* lista2= listaLista(lista2);
    // lista2=  agregarLis
    // unir_dos_listas(lista1,);
    
}