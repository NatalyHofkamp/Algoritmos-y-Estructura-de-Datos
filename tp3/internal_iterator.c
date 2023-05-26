#include <stdbool.h>
#include "internal_iterator.h"
#include <string.h>
#include <stdio.h>
/*
 * Punto extra de internal iterator, suma 1 punto como máximo.
 */


/*
 * Itera cada par clave-valor del diccionario pasandoselo a la función de iteración.
 * Recibe un parámetro extra que puede contener cualquier cosa para permitirle a la función guardar resultados.
 */
void iterate(dictionary_t* dict, iterate_f f, void* extra) {
    size_t i = 0;
    while (dict && i< dict->size) {
        if (dict->buckets[i].key && f(dict->buckets[i].key, dict->buckets[i].value, extra)) {
                break;  
            }
        i++;
    }
}