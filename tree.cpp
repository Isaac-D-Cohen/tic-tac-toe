#include "tree.h"

node * add_branch(node *&main_branch){
    node * new_node = new node;
    main_branch->branches.push_back(new_node);
    return new_node;
}

void remove_branch(node *&main_branch, int n){
    node * branch_node = main_branch->branches[n];
    delete branch_node;
    main_branch->branches.erase(main_branch->branches.begin()+n);
}

int get_number_of_branches(node *&main_branch){
    return main_branch->branches.size();
}


// the following two functions scan all branches of a node and return the maximum or minimum number respectively
int find_max(node *&cur_node){

    int max, n;

    n=get_number_of_branches(cur_node);

    node * branch;

    // start by setting max to the value of the 1st branch (branch 0)
    branch=get_branch(cur_node, 0);
    max = branch->value;

    // if branch i has a higher number than what we already have, update the number in max
    for (int i=1; i<n; i++){
        branch=get_branch(cur_node, i);
        if (branch->value > max) max = branch->value;
    }

    return max;
}

int find_min(node *&cur_node){

    int min, n;

    n=get_number_of_branches(cur_node);

    node * branch;

    // start by setting min to the value of the 1st branch (branch 0)
    branch=get_branch(cur_node, 0);
    min = branch->value;

    // if branch i has a lower number than what we already have, update the number in min
    for (int i=1; i<n; i++){
        branch=get_branch(cur_node, i);
        if (branch->value < min) min = branch->value;
    }

    return min;
}

int get_number_of_max_branch(node *tree){

    node *max_branch;
    int max_val, branch_no, n;

    n=get_number_of_branches(tree);

    // set max_branch to the first branch (number 0) and set max_val to its value
    max_branch=get_branch(tree, 0);
    max_val=max_branch->value;

    branch_no=0;

    // if we come across anything higher, change max_val and max_branch
    for (int i=1; i<n; i++)
        if (get_branch(tree, i)->value > max_val){
            max_branch=get_branch(tree, i);
            max_val=max_branch->value;
            branch_no=i;
        }

    return branch_no;
}

int get_number_of_min_branch(node *tree){

    node *min_branch;
    int min_val, branch_no, n;

    n=get_number_of_branches(tree);

    // set min_branch to the first branch (number 0) and set min_val to its value
    min_branch=get_branch(tree, 0);
    min_val=min_branch->value;

    branch_no=0;

    // if we come across anything lower, change min_val and min_branch
    for (int i=1; i<n; i++)
        if (get_branch(tree, i)->value < min_val){
            min_branch=get_branch(tree, i);
            min_val=min_branch->value;
            branch_no=i;
        }

    return branch_no;
}

void get_positive_partial_sum_vec(node *tree, std::vector <int> &partial_sum, float multiplier){

    int n=get_number_of_branches(tree);

    partial_sum.reserve(n);

    // we want to shift everything by the min to ensure we have all positive numbers
    int min = find_min(tree);
    min = min < 0? -min: min;   // get the absolute value of min

    int running_sum = 0;

    for (int i=0; i<n; i++){
        running_sum += (get_branch(tree, i)->value + min) * multiplier;
        partial_sum.push_back(running_sum);
    }
}

node * get_branch(node *&main_branch, int n){
    return main_branch->branches[n];
}

bool is_leaf(node *&the_node){
    if (the_node->branches.size()==0) return true;
    return false;
}

void delete_tree(node *root){
    
    int n=get_number_of_branches(root);
    
    for (int i=0; i<n; i++)
        delete_tree(get_branch(root, i));
    
    // base case: delete the node
    delete root;
    
}
