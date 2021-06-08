#include <stdlib.h>
#include "xfs.h"
#include "list.h"


node_t* create_list_node(fm_xfs_dir_entry_t val) {
    node_t* new_instance = (node_t*) malloc(sizeof(node_t));
    new_instance->val = val;
    new_instance->next = NULL;

    return new_instance;
}

void push_to_list(node_t * head, fm_xfs_dir_entry_t val) {
    node_t * current = head;
    while (current->next != NULL) {
        current = current->next;
    }

    /* now we can add a new variable */
    current->next = create_list_node(val);
}

void print_list(node_t * head) {
    node_t * current = head;

    while (current != NULL) {
        printf("%d\n", current->val);
        current = current->next;
    }
}