#include "engine.h"
#include <ctime>
#include <cstdio>

engine::engine(settings &s){
    this->s = s;

    // clear our board
    for (int i=0; i<3; i++)
        for (int j=0; j<3; j++)
            board[i][j] = QChar::Null;

    // clear our move history
    for (int i=0; i<9; i++)
        move_history[i] = -1;
    move_number = 0;

    // levels 0 and 1 use the random generator
    if (s.level != 2)
        srand(time(NULL));

    tree = original_tree = new node;
    compute_game(tree, 0, s.computer_first);
}

engine::engine(const engine &otherEngine){
    s = otherEngine.s;

    if (s.level != 2)
        srand(time(NULL));

    // copy the board
    for (int i=0; i<3; i++)
        for (int j=0; j<3; j++)
            board[i][j] = otherEngine.board[i][j];

    tree = original_tree = new node;
    compute_game(tree, 0, s.computer_first);

    // put in all the moves the other engine recorded
    move_number = 0;
    bool c_turn = s.computer_first;

    for (int i=0; i<otherEngine.move_number; i++){
        make_move(otherEngine.move_history[i], c_turn);
        c_turn = !c_turn;
    }
}

void engine::operator=(const engine &otherEngine){

    delete_tree(original_tree);

    s = otherEngine.s;

    if (s.level != 2)
        srand(time(NULL));

    // copy the board
    for (int i=0; i<3; i++)
        for (int j=0; j<3; j++)
            board[i][j] = otherEngine.board[i][j];

    tree = original_tree = new node;
    compute_game(tree, 0, s.computer_first);

    // put in all the moves the other engine recorded
    move_number = 0;
    bool c_turn = s.computer_first;

    for (int i=0; i<otherEngine.move_number; i++){
        make_move(otherEngine.move_history[i], c_turn);
        c_turn = !c_turn;
    }
}

// ends current game and starts a new one
// (note: the constructor already starts a game)
void engine::new_game() {

    if (s.alternate_for_each_game)
        s.computer_first = !s.computer_first;

    delete_tree(original_tree);

    // clear the board
    for (int i=0; i<3; i++)
        for (int j=0; j<3; j++)
            board[i][j] = QChar::Null;

    // clear our move history
    for (int i=0; i<9; i++)
        move_history[i] = -1;
    move_number = 0;

    tree = original_tree = new node;
    compute_game(tree, 0, s.computer_first);
}

engine::~engine(){
    delete_tree(original_tree);
}

int engine::did_someone_win(QChar special_char) const{

    bool won;

    // did the player win by getting three in a row?
    for (int i=0; i<3; i++){
        won=true;
        for (int j=0; j<3; j++)
            if (board[i][j]!=special_char) won=false;
        if (won) return 1;
    }

    // did the player win by getting three in a column?
    for (int j=0; j<3; j++){
        won=true;
        for (int i=0; i<3; i++)
            if (board[i][j]!=special_char) won=false;
        if (won) return 2;
    }

    // did the player win by getting the diagonals?
    won=true;
    for (int a=0; a<3; a++)
        if (board[a][a]!=special_char) won=false;
    if (won) return 3;

    won=true;
    for (int a=0; a<3; a++)
        if (board[a][2-a]!=special_char) won=false;
    if (won) return 4;

    // player didn't win
    return 0;
}

int engine::evaluate_board() const {

    int value;

    // evaluate the board for the computer
    value=did_someone_win(s.computers_symbol);

    if (value==1) return s.computer_wins_by_getting_a_row;
    else if (value==2) return s.computer_wins_by_getting_a_column;
    else if (value==3) return s.computer_wins_by_getting_diagonal_l_to_r;
    else if (value==4) return s.computer_wins_by_getting_diagonal_r_to_l;

    // evaluate the board for the human
    value=did_someone_win(s.humans_symbol);

    if (value==1) return s.human_wins_by_getting_a_row;
    else if (value==2) return s.human_wins_by_getting_a_column;
    else if (value==3) return s.human_wins_by_getting_diagonal_l_to_r;
    else if (value==4) return s.human_wins_by_getting_diagonal_r_to_l;

    // nobody won
    return s.draw;

}


// this function finds the nth square that is not taken (i.e. the 4th square that is suitable to go in)
int engine::find_nth_suitable_square(int n) const {

    int square=-1;

    for (int i=0; i<3; i++)
        for (int j=0; j<3; j++){
            if (board[i][j] == QChar::Null) square++;
            if (square == n) return i*3+j;
        }

    // this can't happen, but just to make the compiler happy
    return -1;
}

void engine::compute_game(node * cur_node, int turn_number, bool c_turn){

    // base case: if the board is full, or someone won, the game is over and the node takes on the value of the situation
    if (did_someone_win(s.computers_symbol) || did_someone_win(s.humans_symbol) || turn_number==9){
        cur_node->value=evaluate_board();
        return;
    }

    // otherwise, make branches, go somewhere for each branch and call this function for the rest of the board

    node *branch_node;
    int a, b;

    for (int i=0; i<9-turn_number; i++){

        branch_node=add_branch(cur_node);

        a=find_nth_suitable_square(i);

        b=a%3;
        a=a/3;
        if (c_turn) board[a][b]=s.computers_symbol;
        else board[a][b]=s.humans_symbol;

        compute_game(branch_node, turn_number+1, !c_turn);

        // undo the move on the board
        board[a][b] = QChar::Null;
    }

    // then give this node a value based on all its branches
    // the computer is always the maximizer in this program
    // (if you want the computer to try to make you win set
    // the numbers for human winning to be higher than for computer)
    // if it's the computer's turn, we maximize
    if (c_turn) cur_node->value = find_max(cur_node);
    else cur_node->value = find_min(cur_node);
}


// this function takes in a raw number of a square
// and gives us the matching branch number
int engine::translate_square(int square) const {

    int branch_number=0, x, y;

    for (int i=0; i<square; i++){
        x=i/3;
        y=i%3;
        if (board[x][y] == QChar::Null) branch_number++;
    }

    return branch_number;
}



// returns 0 if computer won
// 1 if human won
// 2 nobody (potentially draw)
int engine::check_for_winner() const {

    if (s.victory_by_board){
        if (did_someone_win(s.computers_symbol)) return 0;
        else if (did_someone_win(s.humans_symbol)) return 1;
        else return 2;
    } else {
        if (tree->value > 0) return 0;
        else if (tree->value < 0) return 1;
        else return 2;
    }
}

// pass in the human's move
// we assume the move is legal and it is the
// human's turn (up to the caller to ensure)
void engine::make_move(int square, bool is_human){

    // record our current position in the tree and our move
    tree_history[move_number] = tree;
    move_history[move_number++] = square;

    tree=get_branch(tree, translate_square(square));
    if (is_human)
        board[square/3][square%3] = s.humans_symbol;
    else
        board[square/3][square%3] = s.computers_symbol;
}

// returns the computer's move
// we assume it's the computer's turn
int engine::get_move(){

    int square = 0;

    switch(s.level) {
        case 0:
            square = rand() % get_number_of_branches(tree);
            break;
        case 1:
            {
                std::vector <int> partial_sum;
                get_positive_partial_sum_vec(tree, partial_sum, s.multiplier);
                int sum = partial_sum[partial_sum.size()-1];

                // all branches are equal
                if (sum == 0) {
                    square = rand() % get_number_of_branches(tree);
                    break;
                }

                int rand_num = rand() % (sum+1);

                for (; square<partial_sum.size(); square++)
                    if (rand_num <= partial_sum[square])
                        break;
            }
            break;
        case 2:
            // get optimal move from the tree
            square=get_number_of_max_branch(tree);
            break;
    }

    // record our current position in the tree
    tree_history[move_number] = tree;

    // go down the tree
    tree=get_branch(tree, square);

    // convert our move to the actual square number
    square=find_nth_suitable_square(square);

    board[square/3][square%3] = s.computers_symbol;

    // record our move
    move_history[move_number++] = square;

    return square;
}

// returns the optimal move from the tree, playing as the human
int engine::get_hint() const {
    // get optimal move from the tree
    int square=get_number_of_min_branch(tree);

    // convert it to the actual square number
    square=find_nth_suitable_square(square);

    // suggest it to the user
    return square;
}

void engine::undo(){
    tree = tree_history[--move_number];
    int square = move_history[move_number];
    board[square/3][square%3] = QChar::Null;
}
