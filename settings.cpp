#include "settings.h"
#include <QRegularExpression>
#include <QMessageBox>
#include <QFile>
#include <QFileInfo>

QString default_settings_name = "settings.txt";

// these settings take up a total of 7 lines
// 1 for the symbols, 3 for color, 2 for fonts, and 1 for hint blink time
bool settings::load_visual_settings(QTextStream &in){

    static QRegularExpression one_or_more_spaces("\\s+");
    QString input;
    QStringList tokens;
    bool ok, all_ok = true;

    input = in.readLine();
    tokens = input.split(one_or_more_spaces, Qt::SkipEmptyParts);

    if (tokens.size() != 2){
        // go to the end of the section this function is supposed to read
        for (int j=0; j<6; j++)
            in.readLine();
        return false;
    }

    computers_symbol = tokens[0][0];
    humans_symbol = tokens[1][0];


    // read in the colors
    int red, green, blue, alpha;

    // three lines of colors for board color, human's color, and computer's color
    for (int i=0; i<3; i++){
        input = in.readLine();
        tokens = input.split(one_or_more_spaces, Qt::SkipEmptyParts);

        if (tokens.size() != 4){
            for (int j=i+2; j<7; j++)
                in.readLine();
            return false;
        }

        red = tokens[0].toInt(&ok);
        all_ok &= ok;
        green = tokens[1].toInt(&ok);
        all_ok &= ok;
        blue = tokens[2].toInt(&ok);
        all_ok &= ok;
        alpha = tokens[3].toInt(&ok);
        all_ok &= ok;

        if (!all_ok){
            for (int j=i+2; j<7; j++)
                in.readLine();
            return false;
        }

        switch(i){
        case 0:
            board_color = QColor::fromRgb(red, green, blue, alpha);
            break;
        case 1:
            humans_color = QColor::fromRgb(red, green, blue, alpha);
            break;
        case 2:
            computers_color = QColor::fromRgb(red, green, blue, alpha);
            break;
        }
    }

    // read the fonts
    QFont fhuman, fcomputer;

    input = in.readLine();

    if (!fhuman.fromString(input)){
        for (int j=0; j<2; j++)
            in.readLine();
        return false;
    }
    else
        humans_font = fhuman;

    input = in.readLine();

    if (!fcomputer.fromString(input)){
        in.readLine();
        return false;
    }
    else
        computers_font = fcomputer;

    // hint blink time
    input = in.readLine();
    double h = input.toDouble(&ok);
    if (!ok) return false;
    hint_blink_time = h;

    return true;
}

// these settings take up a total of 4 lines
// one determines who goes first and whether you alternate
// one for engine dials, one for level info, and one for how to determine the winner
bool settings::load_engine_settings(QTextStream &in){

    static QRegularExpression one_or_more_spaces("\\s+");
    QString input;
    QStringList tokens;
    bool ok, all_ok = true;

    input = in.readLine();
    tokens = input.split(one_or_more_spaces, Qt::SkipEmptyParts);

    if (tokens.size() != 2){
        for (int j=0; j<3; j++)
            in.readLine();
        return false;
    }

    computer_first = tokens[0].toInt(&ok);
    all_ok &= ok;
    alternate_for_each_game = tokens[1].toInt(&ok);
    all_ok &= ok;

    if (!all_ok){
        for (int j=0; j<3; j++)
            in.readLine();
        return false;
    }

    input = in.readLine();
    tokens = input.split(one_or_more_spaces, Qt::SkipEmptyParts);

    if (tokens.size() != 9){
        for (int j=0; j<2; j++)
            in.readLine();
        return false;
    }

    int dials[9];

    for (int i=0; i<9; i++){
        dials[i] = tokens[i].toInt(&ok);
        all_ok &= ok;
    }

    if (!all_ok){
        for (int j=0; j<2; j++)
            in.readLine();
        return false;
    }

    computer_wins_by_getting_a_row = dials[0];
    computer_wins_by_getting_a_column = dials[1];
    computer_wins_by_getting_diagonal_l_to_r = dials[2];
    computer_wins_by_getting_diagonal_r_to_l = dials[3];
    human_wins_by_getting_a_column = dials[4];
    human_wins_by_getting_a_row = dials[5];
    human_wins_by_getting_diagonal_l_to_r = dials[6];
    human_wins_by_getting_diagonal_r_to_l = dials[7];
    draw = dials[8];

    input = in.readLine();
    tokens = input.split(one_or_more_spaces, Qt::SkipEmptyParts);

    if (tokens.size() != 2){
        in.readLine();
        return false;
    }

    int l = tokens[0].toInt(&ok);
    all_ok &= ok;
    float m = tokens[1].toFloat(&ok);
    all_ok &= ok;

    if (!all_ok){
        in.readLine();
        return false;
    }

    level = l;
    multiplier = m;

    // victory by value
    input = in.readLine();
    int v = input.toInt(&ok);
    if (!ok) return false;
    victory_by_board = v;

    return true;
}

// these settings take up a total of 2 lines - but if it fails
// it just stops where it failed. These settings are intended
// to be the last thing you read from the input file.
// The first line determines which settings are loaded along with games
// the second, the size of the window
bool settings::load_other_settings(QTextStream &in){

    static QRegularExpression one_or_more_spaces("\\s+");
    QString input;
    QStringList tokens;
    bool ok, all_ok = true;

    input = in.readLine();
    tokens = input.split(one_or_more_spaces, Qt::SkipEmptyParts);

    if (tokens.size() != 3){
        return false;
    }

    use_loaded_game_symbols = tokens[0].toInt(&ok);
    all_ok &= ok;
    use_loaded_game_engine_settings = tokens[1].toInt(&ok);
    all_ok &= ok;
    use_loaded_game_visual_settings = tokens[2].toInt(&ok);
    all_ok &= ok;


    input = in.readLine();
    tokens = input.split(one_or_more_spaces, Qt::SkipEmptyParts);

    if (tokens.size() != 2){
        return false;
    }

    x_size = tokens[0].toInt(&ok);
    all_ok &= ok;
    y_size = tokens[1].toInt(&ok);
    all_ok &= ok;

    return all_ok;
}

void settings::save_visual_settings(QTextStream &out) const {

    out << computers_symbol << " " << humans_symbol << "\n";

    int red, green, blue, alpha;

    board_color.getRgb(&red, &green, &blue, &alpha);
    out << red << " " << green << " " << blue << " " << alpha << "\n";

    humans_color.getRgb(&red, &green, &blue, &alpha);
    out << red << " " << green << " " << blue << " " << alpha << "\n";

    computers_color.getRgb(&red, &green, &blue, &alpha);
    out << red << " " << green << " " << blue << " " << alpha << "\n";

    out << humans_font.toString() << "\n";
    out << computers_font.toString() << "\n";

    out << hint_blink_time << "\n";
}

void settings::save_engine_settings(QTextStream &out) const {

    out << int(computer_first) << " " << int(alternate_for_each_game) << "\n";

    out << computer_wins_by_getting_a_row << " ";
    out << computer_wins_by_getting_a_column << " ";
    out << computer_wins_by_getting_diagonal_l_to_r << " ";
    out << computer_wins_by_getting_diagonal_r_to_l << " ";
    out << human_wins_by_getting_a_column << " ";
    out << human_wins_by_getting_a_row << " ";
    out << human_wins_by_getting_diagonal_l_to_r << " ";
    out << human_wins_by_getting_diagonal_r_to_l << " ";
    out << draw << "\n";

    out << level << " ";
    out << multiplier << "\n";

    out << int(victory_by_board) << "\n";
}


void settings::save_other_settings(QTextStream &out) const {
    out << int(use_loaded_game_symbols) << " ";
    out << int(use_loaded_game_engine_settings) << " ";
    out << int(use_loaded_game_visual_settings) << "\n";
    out << int(x_size) << " " << int(y_size) << "\n";
}

bool settings::load_settings(QString *path){

    if (path == nullptr){
        path = &default_settings_name;

        // if we are loading from the default file and it doesn't exist
        // well, just keep the default settings
        if (!QFileInfo(*path).isFile()) return true;
    }

    QFile file(*path);

    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
        return false;

    QTextStream in(&file);
    if (!load_visual_settings(in)) return false;
    if (!load_engine_settings(in)) return false;
    if (!load_other_settings(in)) return false;

    return true;
}

// this funciton will overwrite the file if it exists the caller
// is responsible for ensuring that this is what the user intends
bool settings::save_settings(QString *path) const {

    if (path == nullptr)
        path = &default_settings_name;

    QFile file(*path);

    if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
        return false;

    QTextStream out(&file);
    save_visual_settings(out);
    save_engine_settings(out);
    save_other_settings(out);
    return true;
}

void settings::copy_engine_settings(settings &other_settings){

    computer_first = other_settings.computer_first;
    alternate_for_each_game = other_settings.alternate_for_each_game;
    computer_wins_by_getting_a_row = other_settings.computer_wins_by_getting_a_row;
    computer_wins_by_getting_a_column = other_settings.computer_wins_by_getting_a_column;
    computer_wins_by_getting_diagonal_l_to_r = other_settings.computer_wins_by_getting_diagonal_l_to_r;
    computer_wins_by_getting_diagonal_r_to_l = other_settings.computer_wins_by_getting_diagonal_r_to_l;
    human_wins_by_getting_a_column = other_settings.human_wins_by_getting_a_column;
    human_wins_by_getting_a_row = other_settings.human_wins_by_getting_a_row;
    human_wins_by_getting_diagonal_l_to_r = other_settings.human_wins_by_getting_diagonal_l_to_r;
    human_wins_by_getting_diagonal_r_to_l = other_settings.human_wins_by_getting_diagonal_r_to_l;
    draw = other_settings.draw;
    victory_by_board = other_settings.victory_by_board;
    level = other_settings.level;
    multiplier = other_settings.multiplier;

    // this is not a setting, but copy nevertheless
    engine_settings_changed = other_settings.engine_settings_changed;
}

bool settings::engine_settings_identical(settings &other_settings) const {
    return (
        computer_first == other_settings.computer_first &&
        alternate_for_each_game == other_settings.alternate_for_each_game &&
        computer_wins_by_getting_a_row == other_settings.computer_wins_by_getting_a_row &&
        computer_wins_by_getting_a_column == other_settings.computer_wins_by_getting_a_column &&
        computer_wins_by_getting_diagonal_l_to_r == other_settings.computer_wins_by_getting_diagonal_l_to_r &&
        computer_wins_by_getting_diagonal_r_to_l == other_settings.computer_wins_by_getting_diagonal_r_to_l &&
        human_wins_by_getting_a_row == other_settings.human_wins_by_getting_a_row &&
        human_wins_by_getting_a_column == other_settings.human_wins_by_getting_a_column &&
        human_wins_by_getting_diagonal_l_to_r == other_settings.human_wins_by_getting_diagonal_l_to_r &&
        human_wins_by_getting_diagonal_r_to_l == other_settings.human_wins_by_getting_diagonal_r_to_l &&
        draw == other_settings.draw &&
        victory_by_board == other_settings.victory_by_board &&
        level == other_settings.level &&
        multiplier == other_settings.multiplier
        );
}





