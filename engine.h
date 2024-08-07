#pragma once
#include "tree.h"
#include "settings.h"

// the tic tac toe engine
class engine {
public:
    engine(settings &s);
    engine(const engine &otherEngine);
    void operator=(const engine &otherEngine);

    // ends current game and starts a new one
    // (note: the constructor already starts a game)
    void new_game();

    // returns 0 if computer won
    // 1 if human won
    // 2 nobody (potentially draw)
    int check_for_winner() const;

    // somebody makes the move specified by square
    // usually we use this to pass in the human's move
    // we assume the move is legal and it is the
    // human's turn (up to the caller to ensure)
    // but you can also call this function with is_human = false
    // to force the computer to make a specific move on its turn
    void make_move(int square, bool is_human=true);

    // returns the computer's move
    // we assume it's the computer's turn
    // (up to the caller to ensure)
    // also records the move as part of the game
    int get_move();

    // like get_move() but doesn't actually make
    // the move and doesn't factor in
    // different levels of difficulty
    // this means the human always gets the best
    // possible move as a hint (from the tree)
    int get_hint() const;

    // undoes last (usually two) move(s) regardless of the player who made it/them
    // calling this on move 0 is undefined (but will probably just cause a segfault)
    void undo();

    ~engine();

private:
    // the engine needs this to evaluate different board outcomes
    settings s;

    // the minimax tree
    node *tree;

    // to keep track of the top of the tree
    node *original_tree;

    // move history
    node *tree_history[9];
    int move_history[9];
    int move_number;

    // the engine's local copy of the board
    QChar board[3][3];

    int did_someone_win(QChar special_char) const;

    // this function uses did_someone_win() to evaluate the board for both players
    // it returns a value in accordance with what the engine was told to value in the settings
    int evaluate_board() const;

    // this function finds the nth square that is not taken (i.e. the 4th square that is suitable to go in)
    int find_nth_suitable_square(int n) const;

    // the recursive function that calculates the tree
    void compute_game(node * cur_node, int turn_number, bool c_turn);

    // this function takes in a raw number of a square
    // and gives us the matching branch number given
    // that the board is partly filled
    int translate_square(int square) const;
};



