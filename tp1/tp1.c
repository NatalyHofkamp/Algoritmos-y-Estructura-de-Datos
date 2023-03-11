#include "tp1.h"
#include <stdlib.h>
/*
 * Determina si un número es primo.
 */
bool is_prime(int x){
    for (size_t i=2; i<x;i++){
        if ((x<2) ||(x%i==0)){
            return false;
        }
    }
    return true;
}
/*
 * Dado el volumen de un deposito D y el volumen de un producto V
 * la función calcula cuantos productos entran en el depósito.
 */

int storage_capacity(float d, float v){
    if(d>=v && (d*v>0) && d>=0 ){
        return (int) (d/v);
    }
    return 0;
}

/*
 * Intercambia dos valores de enteros.
 */
void swap(int *x, int *y){
    int aux=*x;
    *x=*y;
    *y=aux;
    return;
}
/*
 * Devuelve el máximo de un arreglo de enteros.
 */
int array_max(const int *array, int length){
    int max=array[0];
    for(size_t i=1;i<length;i++){
        if(array[i]>max){
            max=array[i];
        }
    }
    return max;
}
/*
 * Aplica la función a cada elemento de un arreglo de enteros.
 */
void array_map(int *array, int length, int f(int)){
    if (f!=NULL){
        for(size_t x=0;x<length;x++){
            array[x]=f(array[x]);
        }
    }
    return;
}
/*
 * Copia un arreglo de enteros en memoria dinámica.
 * Si el arreglo es NULL devuelve NULL.
 */

int *copy_array(const int *array, int length){
    if(array!=NULL){
        int* aux =(int*) malloc(length*sizeof(int));
        if (NULL==aux){
            return NULL;
        }
        for(size_t i=0;i<length;i++){
                aux[i]=array[i];    
        }
        return aux;
    }
    
    return NULL;
}
/*
 * Hace bubble sort sobre un arreglo de enteros ascendentemente.
 * Si el arreglo es NULL, no hace nada.
 */
void bubble_sort(int *array, int length){
    if(array!=NULL){
        for (size_t x=0; x<length-1;x++){
            for(size_t y=0;y<length-x-1;y++){
                int aux = array[y];
                if (array[y]>array[y+1]){
                    array[y]=array[y+1];
                    array[y+1]=aux;
                }
            }
        }
    }
    return;
}
/*
 * Determina si dos arreglos de enteros son identicamente iguales.
 * En el caso de que alguno sea NULL solo devuelve true si el otro tambien lo es.
 */
bool array_equal(const int *array1, int length1,
                 const int *array2, int length2){
    if((length1 ==length2)&&(array1!=NULL && array2!=NULL)){
        for(size_t i=0;i<length1;i++){
            if(array1[i]!=array2[i]){
                return false;
            }
        }
        return true;
    }
    if (array1==NULL && array2==NULL){
        return true;
    }
    return false;

}
/*
 * Determina si dos arrays de enteros son análogos a un anagrama para textos (en algun orden particular, son el mismo arreglo).
 * Si alguno es NULL devuelve false.
 */
bool integer_anagrams(const int *array1, int length1,
                      const int *array2, int length2){
    int* aux1= copy_array(array1,length1);
    int* aux2= copy_array(array2,length2);
    bubble_sort(aux1,length1);
    bubble_sort(aux2,length2);
    bool result= array_equal(aux1,length1,aux2,length2);
    free(aux1);
    free(aux2);
    return result;

    
}

/*
 * Copia un arreglo de arreglos de enteros en memoria dinámica.
 * Si alguno de ellos en NULL, continua siendo NULL.
 * Si el arreglo de arreglos es NULL, devuelve NULL.
 *
 * array_of_arrays: un arreglo de punteros a arreglos de enteros
 * array_lenghts: un arreglo con los largos de cada arreglo en array_of_arrays
 * array_amount: la cantidad de arreglos
 */
int **copy_array_of_arrays(const int **array_of_arrays, const int *array_lenghts, int array_amount){
    if((array_of_arrays!=NULL) && (array_amount>0)){
        int **aux= malloc(sizeof(int *) * array_amount);
        if (NULL==aux){
            return NULL;
        }
        for(size_t x=0;x<array_amount;x++){
            if(array_of_arrays[x]==NULL){
                aux[x]=NULL;
            }
            else{
                aux[x]= malloc(sizeof(int)*array_lenghts[x]);
                if (NULL==aux[x]){
                    return NULL;
                }
                for(size_t y=0; y<array_lenghts[x];y++){
                     aux[x][y]=array_of_arrays[x][y];
                }
            }
        }
        return aux;
    }
    return NULL;
}

/*
 * Libera toda la memoria asociada a un arreglo de arreglos.
 *
 * array_of_arrays: un arreglo de punteros a arreglos de enteros
 * array_lenghts: un arreglo con los largos de cada arreglo en array_of_arrays
 * array_amount: la cantidad de arreglos
 */
void free_array_of_arrays(int **array_of_arrays, int *array_lenghts, int array_amount){
    for (size_t x=0; x<array_amount;x++){
            free(array_of_arrays[x]);
        }
    free(array_lenghts);
    free(array_of_arrays);
    return;
}