/*
  Marleen de Jonge - 11987502
  Datastructuren & Algoritmen
   -----------------------
  This program implements a dynamically
  allocated hash table containing words
  and the indices where they can be found.
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "array.h"
#include "hash_table.h"

/* Returns pointer to new node containing a given key */
struct node *new_node(char *key);

/* Frees node and its array of values */
void free_node(struct node *n);

/* Enlargens given hash table to a specified size */
int resize(struct table *t, unsigned long new_size);

/* Appends a node to an existing linked list */
int append_to_bucket(struct node **arr, unsigned long index, struct node *n);

/* Copies an array to a new hash table */
int copy(struct node **new_arr, struct table *t);

/* ---------------   FUNCTIONS  ------------------ */

struct table {
  // The (simple) array used to index the table
  struct node **array;
  // The function used for computing the hash values in this table
  unsigned long (*hash_func)(unsigned char *);

  // Maximum load factor after which the table array should be resized
  double max_load;
  // Capacity of the array used to index the table
  unsigned long capacity;
  // Current number of elements stored in the table
  unsigned long load;
};

struct node {
  // The string of characters that is the key for this node
  char *key;
  // A resizing array, containing the all the integer values for this key
  struct array *value;

  // Next pointer
  struct node *next;
};

/*
Initalizes new node containing a string key and a pointer
to an array of values.
*/
struct node *new_node (char *key){
    struct node *new_node = malloc(sizeof(struct node));

    if (new_node != NULL) {
        new_node->next = NULL;
        new_node->key = key;
        new_node->value= array_init(1);
        return new_node;
    }
        return NULL;
}

/*
Frees node in linked list.
*/
void free_node(struct node *n) {
  array_cleanup(n->value);
  free(n);
}

/*
Initializes hash table with a given hash function, 
*/
struct table *table_init(unsigned long capacity, double max_load,
                         unsigned long (*hash_func)(unsigned char *)) 
{
    // Allocate space for hash table
    struct table *t = malloc(sizeof(struct table));

    t->capacity = capacity;
    t->max_load = max_load;
    t->hash_func = hash_func;
    t->load = 0;

    // Allocate space for array of values
    t->array = malloc(sizeof(struct node) * capacity);
    
    // Check if array was initialized correctly
    if (t->array == NULL) {
        return NULL;
    } 

    for(unsigned i = 0; i < capacity; i++ ) {
		  t->array[i] = NULL;
	  }

    return t;
}

/*
Checks if load is still under the max_load limit.
*/
int check_capacity(struct table *t) {
  if ((t->load / t->capacity) > t->max_load) {
    return 1;
  }
  return 0;
}

/*
Copies existing hash table into new array for 
resizing purposes.
*/
int copy(struct node **new_array, struct table *t) {
  // loop over every node linked list in the array
  for (unsigned i = 0; i < t->capacity; i++) {
    struct node *current = t->array[i];

    // loop over every node in the linked list
    while (current != NULL) {
      struct node *previous = current;

      unsigned long new_index =
          t->hash_func((unsigned char *)current->key) % (t->capacity * 2);
      
      if (append_to_bucket(new_array, new_index, current) == -1) {
        return -1;
      }

      current = current->next;
      previous->next = NULL;
    }
  }
  return 0;
}

/*
Appends node to existing array of indexed hash bucket.
*/
int append_to_bucket(struct node **array, unsigned long index, struct node *new_node) {
  if (new_node == NULL || array == NULL) {
    return -1;
  }

  if (array[index] != NULL) {
    struct node *current = array[index];

    // If bucket is non-empty, loop through list to append at end
    while (current != NULL) {
      struct node *next = current->next;

      if (next == NULL) {
        current->next = new_node;
        return 0;
      }
      current = next;
    }
  }

  // if hash bucket is empty, append directly
  array[index] = new_node;
  return 0;
}


int resize(struct table *t, unsigned long new_size) {
  if (t != NULL) {
    // Check if hash table should be resized
    if (check_capacity(t) == 0) {
      return 0;
    }
  
    // Initialize new array twice the original size
    struct node **new_array = malloc(sizeof(struct node) * new_size);
    if (new_array == NULL) {
      return 1;
    }

    // Loop over hash table array to set all heads to NULL
    for (unsigned int i = 0; i < new_size; i++) {
      new_array[i] = NULL;
    }

    copy(new_array, t);

    free(t->array);
    t->array = new_array;
    t->capacity = new_size;
    return 0;
    }

  // Return if hash table was NULL
  return 1;
}

int table_insert(struct table *t, char *key, int value) {
  if (t == NULL || key == NULL) {
    return 1;
  }

  unsigned long index = t->hash_func((unsigned char *)key) % t->capacity;
  
  struct node *current = t->array[index];
  struct node *last = NULL;

  // If linkedlist is empty
  if (current == NULL) {
    t->array[index] = new_node(key);
    array_append(t->array[index]->value, value);
    t->load++;
  }

  // If linkedlist is non-empty
  else {
    while (current != NULL && strcmp(current->key, key) != 0) {
      last = current;
      current = current->next;
    }

    if (last == NULL) {
      array_append(t->array[index]->value, value);
    }

    // key was not found
    else if (last->next == NULL) {
      last->next = new_node(key);
      array_append(last->next->value, value);
      t->load++;
    }
    else {
      array_append(last->next->value, value);
    }
  }
  return resize(t, (t->capacity * 2));
}

/*
Looks up the value corresponding to a specified key in the hash
table.
*/
struct array *table_lookup(struct table *t, char *key) {
  if (t == NULL || key == NULL) {
    return NULL;
  }

  unsigned long index = t->hash_func((unsigned char *)key) % t->capacity;
  struct node *current = t->array[index];

  // find the node with the right key
  while (current != NULL) {
    if (strcmp(current->key, key) == 0) {
      break;
    }
    current = current->next;
  }

  if (current != NULL) {
    return current->value;
  }
  return NULL;
}


/*
Deletes a specified key out of the hash table.
*/
int table_delete(struct table *t, char *key) {
  if (t == NULL || key == NULL) {
    return 1;
  }

  unsigned long index = t->hash_func((unsigned char *)key) % t->capacity;
  struct node *current = t->array[index];
  struct node *last = NULL;

  struct node *save_curr;
  struct node *save_last;


  // Loop through linked list to find key to delete
  while (current != NULL) {
    if (strcmp(current->key, key) == 0) {
        save_curr = current;
        save_last = last;
    }
    last = current;
    current = current->next;
  }

  // If node to delete is first node in list
  if (save_curr != NULL && save_last == NULL) {
    t->array[index] = save_curr->next;
    free_node(save_curr);
    t->load--;
     return 0;
  }
  // If node to delete is in the middle of list
  else if (save_curr != NULL) {
    save_last->next = save_curr->next;
    free_node(save_curr);
    t->load--;
     return 0;
  }

  return 1;
}

void table_cleanup(struct table *t) {
  // Loop over linked list to free all nodes and arrays
  for (unsigned int i = 0; i < t->capacity; i++) {
    struct node *current = t->array[i];
    struct node *next = current;

    while (current != NULL) {
      next = current->next;
      free_node(current);
      current = next;
    }
  }

  free(t->array);
  free(t);
}