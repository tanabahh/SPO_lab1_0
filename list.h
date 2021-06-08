//
// Created by taisia on 31.03.2021.
//

#ifndef SPO_LAB1_0_LIST_H
#define SPO_LAB1_0_LIST_H

typedef struct node {
    fm_xfs_dir_entry_t val;
    struct node * next;
} node_t;

node_t* create_list_node(fm_xfs_dir_entry_t val);
void push_to_list(node_t * head, fm_xfs_dir_entry_t val);
void print_list(node_t * head);
#endif //SPO_LAB1_0_LIST_H
