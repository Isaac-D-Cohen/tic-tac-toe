#pragma once

#include <QDialog>
#include <QPen>

#include "settings.h"
#include "boardview.h"

namespace Ui {
class SettingsDialog;
}

class SettingsDialog : public QDialog
{
    Q_OBJECT

public:
    explicit SettingsDialog(QWidget *parent=nullptr, settings *s=nullptr, bool *on_loaded_settings=nullptr, bool *on_loaded_game_settings=nullptr);
    ~SettingsDialog();

private slots:
    void on_computers_symbol_clicked();
    void on_humans_symbol_clicked();
    void on_computers_color_clicked();
    void on_computers_font_clicked();
    void on_humans_color_clicked();
    void on_humans_font_clicked();
    void on_more_info_clicked();
    void on_board_color_clicked();
    void on_hint_blink_time_clicked();
    void on_engine_opts_clicked();
    void on_ld_symbols_stateChanged(int arg1);
    void on_ld_engine_settings_stateChanged(int arg1);
    void on_ld_visual_settings_stateChanged(int arg1);
    void on_save_settings_clicked();
    void on_load_settings_clicked();
    void on_convert_to_normal_clicked();
    void accept();

private:
    Ui::SettingsDialog *ui;
    settings *s;
    bool *on_loaded_settings, *on_loaded_game_settings;
    QGraphicsScene *scene;
    BoardView *b;
};
