#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include "settingsdialog.h"
#include <QPainter>
#include <QColor>
#include <QFile>
#include <QFileDialog>
#include <QMessageBox>
#include <QRegularExpression>
#include <QTimer>
#include <QFontDatabase>
#include <QPushButton>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent),
    ui(new Ui::MainWindow)
{

    // QFontDatabase::addApplicationFont(":/Resources/fonts/seguisym.ttf");
    // QApplication::setFont(QFont(QStringLiteral("Segoe UI Symbol")));

    ui->setupUi(this);

    // locations of the top left corners of the squares
    // to understand this, look at the comment above MainWindow::paintEvent()
    top_left_corners[0][0] = QPoint(-150,-145);
    top_left_corners[0][1] = QPoint(-50,-145);
    top_left_corners[0][2] = QPoint(50,-145);
    top_left_corners[1][0] = QPoint(-150,-45);
    top_left_corners[1][1] = QPoint(-50,-45);
    top_left_corners[1][2] = QPoint(50,-45);
    top_left_corners[2][0] = QPoint(-150,55);
    top_left_corners[2][1] = QPoint(-50,55);
    top_left_corners[2][2] = QPoint(50,55);

    // size of one tic tac toe square
    square_size = QSize(100, 100);


    connect(ui->actionNew, SIGNAL(triggered()), this, SLOT(NewGame()));
    connect(ui->actionLoad, SIGNAL(triggered()), this, SLOT(LoadGame()));
    connect(ui->actionSave, SIGNAL(triggered()), this, SLOT(SaveGame()));
    connect(ui->actionSettings, SIGNAL(triggered()), this, SLOT(ChangeSettings()));
    connect(ui->actionQuit, &QAction::triggered, this, &QApplication::quit);

    connect(ui->actionHint, SIGNAL(triggered()), this, SLOT(Hint()));
    connect(ui->actionUndo_move, SIGNAL(triggered()), this, SLOT(Undo()));
    connect(ui->actionAbout, SIGNAL(triggered()), this, SLOT(About()));

    // if this returns false, the file exists and is corrupted
    if (!s.load_settings(nullptr)){

        QString msg = "The file \'settings.txt\' is corrupted and/or cannot be read. What would you like to do?\n\n\
Option A: Close the program, fix or delete the file, and then relaunch. If you delete the file the program will start afresh\
with the default settings.\n\
Option B: Overwrite the file now with default settings and continue.\n";

        QMessageBox msgBox;
        msgBox.setWindowTitle("Question");
        msgBox.setText(msg);
        QAbstractButton *optionA = msgBox.addButton("Option A", QMessageBox::YesRole);
        msgBox.addButton("Option B", QMessageBox::NoRole);

        msgBox.exec();

        if (msgBox.clickedButton() == optionA){
            exit(0);
        }

        // in case our settings got corrupted while failing to read the file
        s = settings();
    }

    s.save_settings(nullptr);

    computers_pen.setColor(s.computers_color);
    humans_pen.setColor(s.humans_color);

    e = new engine(s);

    // clear the board
    for (int i=0; i<3; i++)
        for (int j=0; j<3; j++)
            board[i][j] = QChar::Null;

    playing = true;
    giving_hint = false;

    c_turn = s.computer_first;

    if (c_turn){
        ComputersTurn();
    }

    resize(s.x_size, s.y_size);
}

MainWindow::~MainWindow(){

    if (!on_loaded_settings && !on_loaded_game_settings)
        s.save_settings(nullptr);

    delete ui;
    delete e;
}

/*
 * Our virtual window is 318 x something
 * We center the painter. Around the center we draw the board
 * Each square is 100 x 100
 * Top left: -150 - -50 in the x direction, -145 - -45 in the y direction
 * Top center: -50 - 50 in the x direction, -145 - -45 in the y direction
 * Top right: 50 - 150 in the x direction, -145 - -45 in the y direction
 * Center left: -150 - -50 in the x direction, -45 - 55 in the y direction
 * Center Center: -50 - 50 in the x direction, -45 - 55 in the y direction
 * Center right: 50 - 150 in the x direction, -45 - 55 in the y direction
 * Bottom left: -150 - -50 in the x direction, 55 - 155 in the y direction
 * Bottom center: -50 - 50 in the x direction, 55 - 155 in the y direction
 * Bottom right: 50 - 150 in the x direction, 55 - 155 in the y direction
 */

// paint the board
// this function is invoked whenever we get a paint event on the main window
void MainWindow::paintEvent(QPaintEvent *event){

    int w = width(), h = height();
    int shorter_side = w < h? w: h;

    // set up the painter
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.translate(w/2, h/2);
    painter.scale(shorter_side / 318.0, shorter_side / 318.0);
    painter.setPen(s.board_color);

    /* draw the board */

    // vertical lines
    painter.drawLine(-50,-145,-50,155);
    painter.drawLine(50,-145,50,155);

    // horizontal lines
    painter.drawLine(-150,-45,150,-45);
    painter.drawLine(-150,55,150,55);


    // add the symbols
    for (int i=0; i<3; i++)
        for (int j=0; j<3; j++){
            if (board[i][j] == s.computers_symbol) {
                painter.setPen(computers_pen);
                painter.setFont(s.computers_font);
                painter.drawText(QRect(top_left_corners[i][j], square_size), Qt::AlignVCenter | Qt::AlignHCenter, QString(s.computers_symbol));
            }
            if (board[i][j] == s.humans_symbol){
                painter.setPen(humans_pen);
                painter.setFont(s.humans_font);
                painter.drawText(QRect(top_left_corners[i][j], square_size), Qt::AlignVCenter | Qt::AlignHCenter, QString(s.humans_symbol));
            }
        }

}

void MainWindow::mousePressEvent(QMouseEvent *event) {

    // don't let the user input moves while we are giving a hint
    // the board isn't consistent with the move_history
    if (event->button() == Qt::LeftButton && playing && !giving_hint) {

        // get click location
        QPoint move_pos = event->pos();

        int w = width(), h = height();
        int shorter_side = w < h? w: h;

        int adjusted_x = ((move_pos.x() - w/2) * 318.0 / shorter_side);
        int adjusted_y = ((move_pos.y() - h/2) * 318.0 / shorter_side);

        // out of range
        if (adjusted_x <= -150 || adjusted_x > 150)
            return;
        if (adjusted_y <= -145 || adjusted_y > 155)
            return;

        int move_column, move_row;

        // translate point to a square number
        if (adjusted_x <= -50)
            move_column = 0;
        else if (adjusted_x <= 50)
            move_column = 1;
        else
            move_column = 2;

        if (adjusted_y <= -45)
            move_row = 0;
        else if (adjusted_y <= 55)
            move_row = 1;
        else
            move_row = 2;

        // if something is already there on the board,
        // this isn't a valid move
        if (board[move_row][move_column] != QChar::Null)
            return;

        int square = move_row*3 + move_column;
        e->make_move(square);
        move_history.push_back(square);

        board[move_row][move_column] = s.humans_symbol;

        c_turn = true;

        this->update();

        int winner;
        winner = e->check_for_winner();

        if (winner == 0) {
            GameOver("I win", "It looks like I won this one. Better luck next time... Wanna play again now?");
            return;
        } else if (winner == 1) {
            GameOver("You win!", "Congratulations! You won! Wanna play again?");
            return;
        }

        if (move_history.size() == 9){
            GameOver("It's a draw!", "It's a draw! Wanna play again?");
            return;
        }

        ComputersTurn();

        winner = e->check_for_winner();

        if (winner == 0) {
            GameOver("I win", "It looks like I won this one. Better luck next time... Wanna play again now?");
            return;
        } else if (winner == 1) {
            GameOver("You win!", "Congratulations! You won! Wanna play again?");
            return;
        }

        if (move_history.size() == 9){
            GameOver("It's a draw!", "It's a draw! Wanna play again?");
            return;
        }
    }

}

void MainWindow::resizeEvent(QResizeEvent *event){
    s.x_size = width();
    s.y_size = height();
}


// displays a message telling the user the outcome of the game
// and asking if they want to play again
void MainWindow::GameOver(QString title, QString message){
    int ret = QMessageBox::information(this, title, message, QMessageBox::Yes | QMessageBox::No);

    if (ret == QMessageBox::Yes){
        StartNewGame();
    } else {
        playing = false;
    }
}

void MainWindow::StartNewGame() {

    move_history.clear();

    for (int i=0; i<3; i++)
        for (int j=0; j<3; j++)
            board[i][j] = QChar::Null;


    // if we were playing a loaded game before
    // reset our settings from the backup
    if (on_loaded_game_settings){

        // if the user changed the engine settings during this game
        // load them into the regular settings
        if (s.engine_settings_changed)
            backup.copy_engine_settings(s);

        s = backup;
        on_loaded_game_settings = false;
    }

    if (s.alternate_for_each_game)
        s.computer_first = !s.computer_first;

    if (s.engine_settings_changed){
        delete e;
        e = new engine(s);
        s.engine_settings_changed = false;
    } else {
        e->new_game();
    }

    c_turn = s.computer_first;

    playing = true;

    if (c_turn)
        ComputersTurn();
    else
        this->update();
}

void MainWindow::NewGame() {
    int ret = QMessageBox::question(this, "Please Confirm", "Are you sure you want to leave this game and start a new one?",
                QMessageBox::Yes | QMessageBox::No);

    if (ret == QMessageBox::Yes)
        StartNewGame();
}

void MainWindow::LoadGame() {

    QString filename = QFileDialog::getOpenFileName(this, "Load from...");

    if (filename.isNull())
        return;

    QFile file(filename);

    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)){
        QMessageBox::critical(this, "Error", "Cannot read the input file.");
        return;
    }

    QTextStream in(&file);

    QChar symbol_one, symbol_two;
    QString input;

    static QRegularExpression one_or_more_spaces("\\s+");

    input = in.readLine();

    QStringList tokens;
    tokens = input.split(one_or_more_spaces, Qt::SkipEmptyParts);

    if (tokens.size() != 3){
        QMessageBox::critical(this, "Error", "Cannot read the input file. It appears to be incorrectly formatted.");
        return;
    }

    symbol_one = tokens[0][0];
    input = tokens[1];
    symbol_two = tokens[2][0];

    if (input != "vs."){
        QMessageBox::critical(this, "Error", "Cannot read the input file. It appears to be incorrectly formatted.");
        return;
    }

    if (s.use_loaded_game_symbols || s.use_loaded_game_engine_settings || s.use_loaded_game_visual_settings){
        backup = s;
        on_loaded_game_settings = true;         // this tells us to eventually restore our backed up settings
    }

    // if we are using the symbols in our settings, we need to know which
    // will correspond to the user at this early point
    if (!s.use_loaded_game_symbols) {
        QMessageBox msgBox;
        msgBox.setWindowTitle("Question");
        msgBox.setText("Would you like to be the player who goes first or second?");
        QAbstractButton *first = msgBox.addButton("First", QMessageBox::YesRole);
        msgBox.addButton("Second", QMessageBox::NoRole);

        msgBox.exec();

        // if the user wants to go first, symbol one is theirs
        if (msgBox.clickedButton() == first) {
            symbol_one = s.humans_symbol;
            symbol_two = s.computers_symbol;
            s.computer_first = false;
        } else {
            symbol_one = s.computers_symbol;
            symbol_two = s.humans_symbol;
            s.computer_first = true;
        }
    }

    QVector <int> temp_history;

    QChar temp_board[3][3];

    for (int i=0; i<3; i++)
        for (int j=0; j<3; j++)
            temp_board[i][j] = QChar::Null;

    // for each move, add it to the history and board

    bool ok, first_player = true;
    int square;

    input = in.readLine();
    tokens = input.split(one_or_more_spaces, Qt::SkipEmptyParts);

    for (QString &t: tokens) {

        square = t.toInt(&ok);

        if (!ok){
            QMessageBox::critical(this, "Error", "Cannot read the input file. It appears to be incorrectly formatted.");
            return;
        }

        temp_history.push_back(square);

        int row = square/3, column = square%3;

        // this ensures we never end up with an illegal game that is somehow out of the engine's tree
        if (temp_board[row][column] != QChar::Null){
            QMessageBox::critical(this, "Error", "The input file appears to contain an illegal move. Aborting.");
            return;
        }

        if (first_player)
            temp_board[row][column] = symbol_one;
        else
            temp_board[row][column] = symbol_two;

        first_player = !first_player;
    }

    if (s.use_loaded_game_engine_settings && !s.load_engine_settings(in)){

        int ret = QMessageBox::information(this, "Error", "Couldn't load engine settings from the file. Continue loading?", QMessageBox::Yes | QMessageBox::No);
        if (ret == QMessageBox::No) {
            s = backup;
            on_loaded_game_settings = false;
            return;
        }

    } else if (!s.use_loaded_game_engine_settings){
        // skip 3 lines
        for (int i=0; i<3; i++)
            in.readLine();
    }

    if (s.use_loaded_game_visual_settings && !s.load_visual_settings(in)){
        int ret = QMessageBox::information(this, "Error", "Couldn't load visual settings from the file. Continue loading?", QMessageBox::Yes | QMessageBox::No);
        if (ret == QMessageBox::No) {
            s = backup;
            on_loaded_game_settings = false;
            return;
        }
    } else if (s.use_loaded_game_visual_settings) {
        computers_pen.setColor(s.computers_color);
        humans_pen.setColor(s.humans_color);
    }


    // let's renew our engine to match this game
    delete e;

    // copy the board
    for (int i=0; i<3; i++)
        for (int j=0; j<3; j++)
            board[i][j] = temp_board[i][j];

    // copy move history
    move_history = temp_history;

    if (s.use_loaded_game_symbols) {
        QString prompt = QString("Do you want to be the first player, ") + symbol_one + QString(", or the second player, ") + symbol_two + QString("?");
        QMessageBox msgBox;
        msgBox.setWindowTitle("Question");
        msgBox.setText(prompt);
        QAbstractButton *first_player = msgBox.addButton(QString(symbol_one), QMessageBox::YesRole);
        msgBox.addButton(QString(symbol_two), QMessageBox::NoRole);
        msgBox.exec();

        // if user wants to be symbol_one
        if (msgBox.clickedButton() == first_player){
            s.computer_first = false;
            s.humans_symbol = symbol_one;
            s.computers_symbol = symbol_two;
        } else {
            s.computer_first = true;
            s.computers_symbol = symbol_one;
            s.humans_symbol = symbol_two;
        }
    }

    e = new engine(s);

    c_turn = s.computer_first;

    for (int i: move_history){
        e->make_move(i, c_turn);
        c_turn = !c_turn;
    }

    this->update();

    int winner = e->check_for_winner();

    if (winner == 0) {
        QMessageBox::information(this, "Information", "This game previously ended in a victory for the human player.");
        playing = false;
    } else if (winner == 1) {
        QMessageBox::information(this, "Information", "This game previously ended in a victory for the computer.");
        playing = false;
    } else if (move_history.size() == 9){
        QMessageBox::information(this, "Information", "This game previously ended in a draw.");
        playing = false;
    } else {
        playing = true;
    }
}

void MainWindow::SaveGame() {

    QString filename = QFileDialog::getSaveFileName(this, "Save to...");

    if (filename.isNull())
        return;

    QFile file(filename);

    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)){
        QMessageBox::critical(this, "Error", "Failed to open the file for writing.");
        return;
    }

    QTextStream out(&file);

    // save game symbols (first symbol in the file is first player)
    if (s.computer_first)
        out << s.computers_symbol << " vs. " << s.humans_symbol << "\n";
    else
        out << s.humans_symbol << " vs. " << s.computers_symbol << "\n";

    // save moves
    for (int i: move_history){
        out << i << " ";
    }

    out << "\n";

    s.save_engine_settings(out);
    s.save_visual_settings(out);
}

void MainWindow::ChangeSettings() {

    settings temp = s;

    bool on_loaded_settings = this->on_loaded_settings;
    bool on_loaded_game_settings = this->on_loaded_game_settings;

    SettingsDialog *settings = new SettingsDialog(this, &temp, &on_loaded_settings, &on_loaded_game_settings);
    int modified = settings->exec();

    // the user didn't click cancel, so settings were changed
    if (modified) {
        s = temp;

        this->on_loaded_settings = on_loaded_settings;
        this->on_loaded_game_settings = on_loaded_game_settings;

        // if we're not on loaded settings, this seems like a good time to hit save
        if (!on_loaded_settings)
            s.save_settings(nullptr);

        computers_pen.setColor(s.computers_color);
        humans_pen.setColor(s.humans_color);

        // if we haven't yet started the game and the settings were changed...
        // start a new one
        if (move_history.empty())
            StartNewGame();
        else {
            // update the board
            for (int i=0; i<3; i++)
                for (int j=0; j<3; j++){
                    if (board[i][j] == s.computers_symbol)
                        board[i][j] = temp.computers_symbol;
                    else if (board[i][j] == s.humans_symbol)
                        board[i][j] = temp.humans_symbol;
                }

            this->update();
        }
    }
}

void MainWindow::Hint() {
    hint_square = e->get_hint();
    giving_hint = true;
    board[hint_square/3][hint_square%3] = s.humans_symbol;
    this->update();
    QTimer::singleShot(s.hint_blink_time*1000, this, &MainWindow::RemoveHint);
}

void MainWindow::RemoveHint(){
    board[hint_square/3][hint_square%3] = QChar::Null;
    giving_hint = false;
    this->update();
}

void MainWindow::About() {
    QMessageBox::about(this, "About this program", "This is a Tic Tac Toe program, written by Isaac Cohen, in C++ with Qt. \
It is licensed under GPLv3. The source code and documentation are available on Github: \
<a href='https://github.com/Isaac-D-Cohen/tic-tac-toe.git'>https://github.com/Isaac-D-Cohen/tic-tac-toe.git</a>");
}

void MainWindow::ComputersTurn() {
    int square = e->get_move();
    move_history.push_back(square);
    board[square/3][square%3] = s.computers_symbol;
    c_turn = false;
    this->update();
}

void MainWindow::Undo(){

    if (move_history.size() == 0) return;

    e->undo();
    int &square = move_history.back();
    board[square/3][square%3] = QChar::Null;
    move_history.pop_back();
    c_turn = !c_turn;

    // if there's been more than 1 move so far and we just undid
    // the computers move, let's undo the human's as well
    if (move_history.size() > 0 && c_turn){
        e->undo();
        int &square = move_history.back();
        board[square/3][square%3] = QChar::Null;
        move_history.pop_back();
        c_turn = false;
        this->update();
    } else if (!c_turn) {
        this->update();
    } else {                // it is the computer's turn, but on the first move
        ComputersTurn();
    }

    // even if the game already ended
    // we are once again playing now
    playing = true;
}

