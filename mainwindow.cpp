#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->doubleSpinBox_2->setRange(0,2000);
}

MainWindow::~MainWindow()
{
    delete ui;
}



void MainWindow::on_pushButton_2_clicked()
{
    //emit ui->doubleSpinBox_2->valueChanged();
    emit valuewl(ui->doubleSpinBox_2->value());
}

double valuewl(double wl){
    return wl;
}

double gotosignal(double pos){
    return pos;
}

void MainWindow::on_pushButton_5_clicked()
{
    emit gotosignal(ui->doubleSpinBox_3->value());
}
