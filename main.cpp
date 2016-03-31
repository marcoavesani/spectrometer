#include "mainwindow.h"
#include <QApplication>
#include "stepper_motor.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.show();
    stepper_motor motor;
    //motor.parse_command("go 1 10");

    return a.exec();
}
