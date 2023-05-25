#include "internal_iterator.h"

/*
 * Punto extra de internal iterator, suma 1 punto como máximo.
 */


/*
 * Itera cada par clave-valor del diccionario pasandoselo a la función de iteración.
 * Recibe un parámetro extra que puede contener cualquier cosa para permitirle a la función guardar resultados.
 */
void iterate(dictionary_t* dict, iterate_f f, void* extra){
    if(!dict) return NULL;
    size_t i=0;
    while(i<dict->size && dict->buckets[i].key&&!f(dict->buckets[i].key,dict->buckets[i].value,extra)){
        i++;
    }
}
