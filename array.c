/* Marleen de Jonge - 11987502
*  Datastructuren en Algoritmen
*  This program implements a dynamically allocated 
*  array to save indices */

#include <stdlib.h>
#include "array.h"

struct array {
    int *array;
    unsigned long size;
    unsigned int used;
};

/*
Initializes array with given capacity
*/
struct array* array_init(unsigned long initial_capacity) {
    struct array *a = malloc (sizeof (struct array) );

    // Assign parameters
    a->array = malloc(sizeof(int) * initial_capacity);
    a->used = 0;
    a->size = initial_capacity; 

    // Check if array was initialized correctly
    if (a->array != NULL) {
        return a;
    }
    return NULL;
}

/*
Frees an array and the corresponding values.
*/
void array_cleanup(struct array* a) {
    free(a->array);
    free(a);
}


/*
Returns element in array at given index.
*/
int array_get(struct array *a, unsigned long index) {
    if (a != NULL) {
        // If index is not a valid position, return -1
        if (index > (a->size - 1)){
            return -1;
        }

        // Else return element at given index
        int element = *(a->array + index);
        return element;
    }
    return -1;
}

/* Note: Although this operation might require the array to be resized and
 * copied, in order to make room for the added element, it is possible to do
 * this in such a way that the amortized complexity is still O(1).
 * Make sure your code is implemented in such a way to guarantee this. */
int array_append(struct array *a, int elem) {
    if (a == NULL || a->array == NULL) {
        return 1;
    }
    // If array is maximum size, resize it.
    if (a->used == a->size){
        a->size += 1;

        // Store result of realloc
        int* tmp = realloc(a->array, a->size * sizeof(int));
        if (tmp == NULL) {
            return 1;
        }
        a->array = tmp;
    }
    a->array[a->used++] = elem;
    return 0;

}

unsigned long array_size(struct array *a) {
    unsigned long size = a->used;
    return size;
}
