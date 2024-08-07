#include "settingsdialog.h"
#include "ui_settingsdialog.h"
#include "changeengineoptions.h"
#include <stdexcept>
#include <QInputDialog>
#include <QColorDialog>
#include <QFontDialog>
#include <QFileDialog>
#include <QMessageBox>

SettingsDialog::SettingsDialog(QWidget *parent, settings *s, bool *on_loaded_settings, bool *on_loaded_game_settings)
    : QDialog(parent)
    , ui(new Ui::SettingsDialog)
{
    ui->setupUi(this);

    if (s == nullptr || on_loaded_settings == nullptr || on_loaded_game_settings == nullptr) {
        throw std::invalid_argument("recieved a nullptr in place of a required argument");
    }

    this->s = s;
    this->on_loaded_settings = on_loaded_settings;
    this->on_loaded_game_settings = on_loaded_game_settings;

    // initial settings
    ui->computer_goes_first->setChecked(s->computer_first);
    ui->human_goes_first->setChecked(!s->computer_first);
    ui->alternate_every_game->setChecked(s->alternate_for_each_game);
    ui->level_easy->setChecked(s->level == 0);
    ui->level_intermediate->setChecked(s->level == 1);
    ui->level_impossible->setChecked(s->level == 2);

    ui->ld_symbols->setChecked(s->use_loaded_game_symbols);
    ui->ld_engine_settings->setChecked(s->use_loaded_game_engine_settings);
    ui->ld_visual_settings->setChecked(s->use_loaded_game_visual_settings);

    ui->convert_to_normal->setDisabled(!(*on_loaded_settings | *on_loaded_game_settings));

    // so far at least...
    s->engine_settings_changed_this_session = false;

    scene = new QGraphicsScene(this);
    ui->graphicsView->setScene(scene);

    // BoardView takes the size of the bounding rectangle
    // i.e. the rectangle, in local coordinates, that will actually
    // be drawn
    // note: no matter what happens, the entire board will be "drawn"
    // but if the rectangle is too big for the QGraphicsView, we would get scrollbars
    b = new BoardView(s, 200, 200);

    scene->addItem(b);
}

SettingsDialog::~SettingsDialog(){
    delete ui;
    delete b;
}


void SettingsDialog::on_computers_symbol_clicked() {
    QInputDialog dlg;
    dlg.setTextValue(s->computers_symbol);

    QObject::connect(&dlg, &QInputDialog::textValueChanged, this,[&dlg](const QString text){
        if (text.length() > 1){
            dlg.setTextValue(text[0]);
        }
    });

    int ret = dlg.exec();

    if (ret == QDialog::DialogCode::Accepted){
        QString text = dlg.textValue();
        if (text.length() != 0)
            s->computers_symbol = text[0];
    }

    scene->update();
}


void SettingsDialog::on_humans_symbol_clicked() {
    QInputDialog dlg;
    dlg.setTextValue(s->humans_symbol);

    QObject::connect(&dlg, &QInputDialog::textValueChanged, this,[&dlg](const QString text){
        if (text.length() > 1){
            dlg.setTextValue(text[0]);
        }
    });

    int ret = dlg.exec();

    if (ret == QDialog::DialogCode::Accepted){
        QString text = dlg.textValue();
        if (text.length() != 0)
            s->humans_symbol = text[0];
    }

    scene->update();
}


void SettingsDialog::on_computers_color_clicked() {
    QColor selected_color = QColorDialog::getColor(s->computers_color, this);
    if (selected_color.isValid()){
        s->computers_color = selected_color;
        scene->update();
    }
}


void SettingsDialog::on_computers_font_clicked() {
    bool ok;
    QFont font = QFontDialog::getFont(&ok, s->computers_font, this);
    if (ok){
        s->computers_font = font;
        scene->update();
    }
}


void SettingsDialog::on_humans_color_clicked() {
    QColor selected_color = QColorDialog::getColor(s->humans_color, this);
    if (selected_color.isValid()){
        s->humans_color = selected_color;
        scene->update();
    }

}


void SettingsDialog::on_humans_font_clicked() {
    bool ok;
    QFont font = QFontDialog::getFont(&ok, s->humans_font, this);
    if (ok){
        s->humans_font = font;
        scene->update();
    }
}

void SettingsDialog::on_more_info_clicked(){
    QMessageBox::information(this, "About levels", "\
Easy: Computer chooses squares at random\n\
Intermediate: Computer uses a combination of random and optimal play\n\
Impossible: Computer plays optimally\n");
}


void SettingsDialog::on_board_color_clicked(){
    QColor selected_color = QColorDialog::getColor(s->board_color, this);
    if (selected_color.isValid()){
        s->board_color = selected_color;
        scene->update();
    }
}


void SettingsDialog::on_hint_blink_time_clicked(){

    bool ok;

    double hint_blink_time = QInputDialog::getDouble(this, "Hint Blink Time", "Enter new hint blink time: ", s->hint_blink_time,
                                                     .002, 10, 6, &ok);

    if (ok)
        s->hint_blink_time = hint_blink_time;
}


void SettingsDialog::on_engine_opts_clicked(){
    ChangeEngineOptions *dlg = new ChangeEngineOptions(this,s);
    dlg->exec();
}


void SettingsDialog::on_ld_symbols_stateChanged(int arg1){
    if (arg1 == 0)
        s->use_loaded_game_symbols = false;
    else
        s->use_loaded_game_symbols = true;
}


void SettingsDialog::on_ld_engine_settings_stateChanged(int arg1){
    if (arg1 == 0)
        s->use_loaded_game_engine_settings = false;
    else
        s->use_loaded_game_engine_settings = true;
}


void SettingsDialog::on_ld_visual_settings_stateChanged(int arg1){
    if (arg1 == 0)
        s->use_loaded_game_visual_settings = false;
    else
        s->use_loaded_game_visual_settings = true;
}


void SettingsDialog::on_save_settings_clicked(){
    QString filename = QFileDialog::getSaveFileName(this, "Save to...");

    if (filename.isNull()) return;

    if (!s->save_settings(&filename)){
        QMessageBox::critical(this, "Error", "Couldn't write the file.");
        return;
    }
}


void SettingsDialog::on_load_settings_clicked(){
    QString filename = QFileDialog::getOpenFileName(this, "Load from...");

    if (filename.isNull()) return;

    settings temp;

    if (!temp.load_settings(&filename)){
        QMessageBox::critical(this, "Error", "Couldn't read the settings file.");
        return;
    }

    *s = temp;
    *on_loaded_settings = true;
    scene->update();
    ui->convert_to_normal->setDisabled(false);
}


void SettingsDialog::on_convert_to_normal_clicked(){

    int ret = QMessageBox::question(this, "Please Confirm", "Would you like to adopt the current settings as your normal settings?\n\
If you do, the program will default to them next time you launch it.",
                                    QMessageBox::Yes | QMessageBox::No);

    if (ret == QMessageBox::Yes){
        *on_loaded_settings = false;
        *on_loaded_game_settings = false;
        ui->convert_to_normal->setDisabled(true);
    }
}

void SettingsDialog::accept(){

    settings temp = *s;

    temp.computer_first = ui->computer_goes_first->isChecked();
    temp.alternate_for_each_game = ui->alternate_every_game->isChecked();

    if (ui->level_easy->isChecked())
        temp.level = 0;
    else if (ui->level_intermediate->isChecked())
        temp.level = 1;
    else
        temp.level = 2;

    temp.engine_settings_changed_this_session |= !(s->engine_settings_identical(temp));

    if (temp.engine_settings_changed_this_session){
        *s = temp;
        s->engine_settings_changed = true;
    }


    if (*on_loaded_game_settings && s->engine_settings_changed_this_session){
        QString prompt = "You modified the engine settings. Since you are currently playing a loaded game, any visual settings you set now \
you temporary and will only be applied to this game. But any engine settings you set now will only take affect on your next game. This \
means the engine settings you set now may be \"permanent.\" Do you still want to apply them?";
        QMessageBox msgBox;
        msgBox.setWindowTitle("Note");
        msgBox.setText(prompt);
        msgBox.addButton("Save engine changes and exit", QMessageBox::YesRole);
        QAbstractButton *return_to_settings_dialog = msgBox.addButton("Return to settings dialog", QMessageBox::NoRole);
        msgBox.exec();

        if (msgBox.clickedButton() == return_to_settings_dialog){
            return;
        }
    }
    QDialog::accept();
}
