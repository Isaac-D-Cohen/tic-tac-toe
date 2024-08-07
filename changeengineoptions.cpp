#include "changeengineoptions.h"
#include "ui_changeengineoptions.h"
#include <stdexcept>
#include <QMessageBox>

ChangeEngineOptions::ChangeEngineOptions(QWidget *parent, settings *s)
    : QDialog(parent)
    , ui(new Ui::ChangeEngineOptions)
{
    ui->setupUi(this);

    if (s == nullptr){
        throw std::invalid_argument("recieved a nullptr in place of a required argument");
    }

    this->s = s;

    ui->human_row->setText(QString::number(s->human_wins_by_getting_a_row));
    ui->human_column->setText(QString::number(s->human_wins_by_getting_a_column));
    ui->human_l_r->setText(QString::number(s->human_wins_by_getting_diagonal_l_to_r));
    ui->human_r_l->setText(QString::number(s->human_wins_by_getting_diagonal_r_to_l));
    ui->comp_row->setText(QString::number(s->computer_wins_by_getting_a_row));
    ui->comp_column->setText(QString::number(s->computer_wins_by_getting_a_column));
    ui->comp_l_r->setText(QString::number(s->computer_wins_by_getting_diagonal_l_to_r));
    ui->comp_r_l->setText(QString::number(s->computer_wins_by_getting_diagonal_r_to_l));
    ui->draw->setText(QString::number(s->draw));

    ui->victory_by_board->setChecked(s->victory_by_board);
}

ChangeEngineOptions::~ChangeEngineOptions()
{
    delete ui;
}

void ChangeEngineOptions::accept(){

    int dials[9];
    bool ok, all_ok = true;

    dials[0] = ui->human_row->text().toInt(&ok);
    all_ok &= ok;
    dials[1] = ui->human_column->text().toInt(&ok);
    all_ok &= ok;
    dials[2] = ui->human_l_r->text().toInt(&ok);
    all_ok &= ok;
    dials[3] = ui->human_r_l->text().toInt(&ok);
    all_ok &= ok;
    dials[4] = ui->comp_row->text().toInt(&ok);
    all_ok &= ok;
    dials[5] = ui->comp_column->text().toInt(&ok);
    all_ok &= ok;
    dials[6] = ui->comp_l_r->text().toInt(&ok);
    all_ok &= ok;
    dials[7] = ui->comp_r_l->text().toInt(&ok);
    all_ok &= ok;
    dials[8] = ui->draw->text().toInt(&ok);
    all_ok &= ok;

    if (!all_ok){
        QMessageBox::critical(this, "Error", "All values must be integers.");
        // user cannot exit
        return;
    }

    if (s->human_wins_by_getting_a_row != dials[0]){
        s->human_wins_by_getting_a_row = dials[0];
        s->engine_settings_changed_this_session = true;
    }

    if (s->human_wins_by_getting_a_column != dials[1]){
        s->human_wins_by_getting_a_column = dials[1];
        s->engine_settings_changed_this_session = true;
    }

    if (s->human_wins_by_getting_diagonal_l_to_r != dials[2]){
        s->human_wins_by_getting_diagonal_l_to_r = dials[2];
        s->engine_settings_changed_this_session = true;
    }

    if (s->human_wins_by_getting_diagonal_r_to_l != dials[3]){
        s->human_wins_by_getting_diagonal_r_to_l = dials[3];
        s->engine_settings_changed_this_session = true;
    }

    if (s->computer_wins_by_getting_a_row != dials[4]){
        s->computer_wins_by_getting_a_row = dials[4];
        s->engine_settings_changed_this_session = true;
    }

    if (s->computer_wins_by_getting_a_column != dials[5]){
        s->computer_wins_by_getting_a_column = dials[5];
        s->engine_settings_changed_this_session = true;
    }

    if (s->computer_wins_by_getting_diagonal_l_to_r != dials[6]){
        s->computer_wins_by_getting_diagonal_l_to_r = dials[6];
        s->engine_settings_changed_this_session = true;
    }

    if (s->computer_wins_by_getting_diagonal_r_to_l != dials[7]){
        s->computer_wins_by_getting_diagonal_r_to_l = dials[7];
        s->engine_settings_changed_this_session = true;
    }

    if (s->draw != dials[8]){
        s->draw = dials[8];
        s->engine_settings_changed_this_session = true;
    }

    bool vic_by_board_val = ui->victory_by_board->isChecked();

    if (s->victory_by_board != vic_by_board_val){
        s->victory_by_board = vic_by_board_val;
        s->engine_settings_changed_this_session = true;
    }

    // user can exit
    QDialog::accept();
}

