#include <stdio.h>
#include <stdlib.h>


/* Cómo crear una lista*/

typedef struct nodo{
    struct nodo* Next;
    char* nombre;
}Libro;

Libro* listaLibro(Libro* lista){
    lista=NULL;
    return lista;
}

Libro* agregarLibro(Libro* lista, char* nombre){
    Libro* nuevoLibro= (Libro*) malloc(sizeof(Libro));
    nuevoLibro->nombre=nombre;
    nuevoLibro->Next=NULL;
    if(NULL== lista){//paso para agregar el primer libro
        lista=nuevoLibro;
    }
    else{//paso para cuando ya tenemos libros
        Libro* aux=lista;
        while(aux->Next!=NULL){//si no apunta a nulo, entonces movete al próximo elemento
            aux=aux->Next;
        }
        aux->Next=nuevoLibro;//estamos parados en el último[porque tiene puntero a NULL]
                             //le agregamos al último, un siguiente nodo(que ya tiene nombre).
    }
    return lista;
}

Libro* BuscarPosicion(Libro* lista, int n){
    if(NULL==lista){
        return NULL;
    }
    else{
        Libro* aux=lista;
        int posicion=0;
        while(posicion<n && lista->Next){
            aux=aux->Next;
            posicion++;
        }
        return aux;
    }
}

Libro* insertarLibro (Libro* lista,int n,char* nombre){
    Libro* nuevoLibro= (Libro*) malloc(sizeof(Libro*));
    nuevoLibro->nombre=nombre;
       if(NULL==lista){
        return NULL;
    }
    else{
        int posicion=0;
        Libro* aux=lista;//guardamos la lista como está
        while(posicion<n && lista->Next){//recorremos hasta llegar a la posición que me dieron
            aux=aux->Next;
            posicion++;
        }
        nuevoLibro->Next=aux->Next;//el libro que queremos colocar apunta al que ya estaba en esta posición
        lista->Next=nuevoLibro;//apuntamos el anterior al libro nuevo
        return lista;//devolvemos la lista con el orden que queremos
    }
}

int main(void){
    Libro* lista =listaLibro(lista);
    lista =agregarLibro(lista,"Harry Potter");
    lista =agregarLibro(lista,"Lord of The Rings");
    // while (lista!=NULL){
    //     printf("%s\n",lista->nombre);//empezamos en el primer libro
    //     lista=lista->Next;//pedimos que el puntero cambie al próximo elemento
    // }
    lista= insertarLibro(lista,1,"Call me by your name");
    printf("%s\n",BuscarPosicion(lista,1)->nombre);
    
}