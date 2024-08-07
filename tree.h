#pragma once
#include <vector>

struct node {
    int value;
    std::vector <node*> branches;
};

node * add_branch(node *&main_branch);
void remove_branch(node *&main_branch, int n);
int get_number_of_branches(node *&main_branch);
int find_max(node *&cur_node);
int find_min(node *&cur_node);
int get_number_of_max_branch(node *tree);
int get_number_of_min_branch(node *tree);
void get_positive_partial_sum_vec(node *tree, std::vector <int> &partial_sum, float multiplier);
node * get_branch(node *&main_branch, int n);
bool is_leaf(node *&the_node);
void delete_tree(node *root);
