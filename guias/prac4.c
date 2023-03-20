#include <stdio.h>
#include <stdlib.h>
/*
typedef struct Persona{
    char* nombre;
    char* apellido;
    char* domicilio;
    int edad;
}Persona;

char* MasGrande (Persona** personas, int n){
    Persona* max=personas[0];
    for(int i=0; i<n;i++){
        if((personas[i]->edad)>(max->edad)){
            max=personas[i];
        }
    }
    return max->nombre;
}


int main(void){
    Persona** personas= (Persona**)malloc(sizeof(Persona*)*2);
    Persona* p1 =(Persona*) malloc (sizeof(Persona));
    Persona* p2=(Persona*) malloc (sizeof(Persona));
    p1->edad=56;
    p1->nombre="carlos";
    p2->edad=12;
    p2->nombre="maria";
    personas[0]=p1;
    personas[1]=p2;
    printf("%s",MasGrande(personas,2));
}

*/

//hacer una matriz transpuesta.

/*
void print_mat(int** mat,int len){
    for(int i=0;i<len;i++){
        for(int j=0;j<len;j++){
            printf("%i ",mat[i][j]);
        }
        printf("\n");
    }
}

void trans (int** mat,int n){
    int** aux;
    aux = malloc(n*sizeof(int*));
    for(int i=0; i<n;i++){
        aux[i]=malloc(n*sizeof(int));
        for(int j=0;j<n;j++){
            aux[i][j]=mat[j][i];
        }
    }
    aux[n] = NULL;
   // print_mat(mat,n);
    print_mat(aux,n);
}

int main(void){
    int** mat= (int**) malloc (sizeof(int*)*3);
    int h=0;
    for(int i=0; i<3;i++){
        mat[i]=(int*)malloc(sizeof(int)*3);
        for(int j=0;j<3;j++){
            mat[i][j]=h++;
        }
    }
    trans(mat,3);
}
*/

/*Implementar una función que permita verificar si una lista es
un palíndromo (es decir, los elementos son los mismos cuando se lee de principio a fin o al
revés). No se pueden usar estructuras de datos adicionales para resolver este problema.
1. ¿Qué complejidad temporal tiene la solución al problema anterior?*/


typedef struct Lista{
    Lista* next;
    int elem;
    int cant;
}Lista;

void recorrer (Lista* l1){
    for (int i=0;i<l1->cant;i++){

    }
}
int main(void){
    Lista* l1= malloc(sizeof(Lista*));
    l1->elem=0;
    l1->next=malloc(sizeof(Lista*));
    l1->next->elem=1;
    l1->next->next=malloc(sizeof(Lista*));
    l1->next->next->next->elem=2;
    l1->next->next->next->next=malloc(sizeof(Lista*));
    l1->next->next->next->next->elem=1;
    l1->next->next->next->next->next=malloc(sizeof(Lista*));
    l1->next->next->next->next->next->elem=0;
    l1->cant=5;
    recorrer(l1);
}



