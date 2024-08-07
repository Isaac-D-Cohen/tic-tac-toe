#pragma once

#include <QMainWindow>
#include <QMouseEvent>
#include <QPen>
#include <QResizeEvent>
#include <QVector>
#include "settings.h"
#include "engine.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    virtual void paintEvent(QPaintEvent *event);
    virtual void mousePressEvent(QMouseEvent *event);
    virtual void resizeEvent(QResizeEvent *event);

private slots:
    void NewGame();
    void LoadGame();
    void SaveGame();
    void ChangeSettings();
    void Hint();
    void About();
    void RemoveHint();
    void Undo();

private:
    Ui::MainWindow *ui;
    settings s, backup;
    engine *e;

    bool on_loaded_game_settings = false;
    bool on_loaded_settings = false;
    bool c_turn;

    void ComputersTurn();
    void GameOver(QString title, QString message);
    void StartNewGame();

    QVector <int> move_history;
    bool playing;

    QPen computers_pen, humans_pen;

    QChar board[3][3];
    QPoint top_left_corners[3][3];
    QSize square_size;

    int hint_square;
    bool giving_hint;
};
