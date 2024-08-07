#pragma once

#include <QColor>
#include <QFont>

// holds the program settings
struct settings {

    settings(){
        computers_font = humans_font = QFont("Andale Mono", 50);
    }

    // load settings from a file
    bool load_settings(QString *path);
    bool save_settings(QString *path) const;

    bool load_visual_settings(QTextStream &in);
    bool load_engine_settings(QTextStream &in);
    bool load_other_settings(QTextStream &in);

    void save_visual_settings(QTextStream &out) const;
    void save_engine_settings(QTextStream &out) const;
    void save_other_settings(QTextStream &out) const;

    void copy_engine_settings(settings &other_settings);
    bool engine_settings_identical(settings &other_settings) const;


    /* VISUAL SETTINGS */
    QColor board_color = QColorConstants::Black;
    QColor humans_color = QColorConstants::Red;
    QColor computers_color = QColorConstants::Blue;

    QChar humans_symbol = QChar('X'), computers_symbol = QChar('O');

    QFont humans_font, computers_font;

    double hint_blink_time = .3;

    /* ENGINE SETTINGS */

    bool computer_first = false, alternate_for_each_game = true;

    // Values for different victories
    int computer_wins_by_getting_a_row = 1;
    int computer_wins_by_getting_a_column = 1;
    int computer_wins_by_getting_diagonal_l_to_r = 1;
    int computer_wins_by_getting_diagonal_r_to_l = 1;

    int human_wins_by_getting_a_column = -1;
    int human_wins_by_getting_a_row = -1;
    int human_wins_by_getting_diagonal_l_to_r = -1;
    int human_wins_by_getting_diagonal_r_to_l = -1;

    int draw = 0;

    // judge victory by whether a positive value was attained
    bool victory_by_board = true;

    // 0 - easy, 1 - intermediate, 2 - impossible
    int level = 1;

    // a knob used in level 1
    float multiplier = 2;


    /* OTHER SETTINGS */

    // when we load a game, load the game's symbols/settings along with it
    bool use_loaded_game_symbols = true;
    bool use_loaded_game_engine_settings = true;
    bool use_loaded_game_visual_settings = true;

    // window size
    int x_size=450, y_size=500;

    // this is an indicator, not a setting
    bool engine_settings_changed = false;
    bool engine_settings_changed_this_session = false;
};
