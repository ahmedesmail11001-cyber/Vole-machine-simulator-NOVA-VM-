#include <QApplication>
#include "MainWindow.h"

/*
 Entry point of the NOVA VM GUI.
 (The old console main file is NOT part of this project anymore.)
 */
int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    QApplication::setApplicationName("NOVA VM");

    MainWindow window;
    window.show();

    return app.exec();
}