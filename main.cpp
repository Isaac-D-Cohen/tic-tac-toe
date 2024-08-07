#include "mainwindow.h"

#include <QApplication>

void messageHandler(QtMsgType type, const QMessageLogContext &context, const QString &msg) {
    // when Qt logs a message, we want to catch it and...
    // do nothing at all with it
    // this prevents logging to the console
    return;
}

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    // qInstallMessageHandler(messageHandler);
    MainWindow w;
    w.show();
    return a.exec();
}
