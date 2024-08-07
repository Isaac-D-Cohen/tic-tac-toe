#pragma once

#include <QDialog>
#include "settings.h"

namespace Ui {
class ChangeEngineOptions;
}

class ChangeEngineOptions : public QDialog
{
    Q_OBJECT

public:
    explicit ChangeEngineOptions(QWidget *parent=nullptr, settings *s=nullptr);
    ~ChangeEngineOptions();

private slots:
    void accept();

private:
    settings *s;
    Ui::ChangeEngineOptions *ui;
};
