#include "mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    TabCoordinator* tabCoordinator = new TabCoordinator(this);
    this->setCentralWidget( tabCoordinator->createWidget() );

    this->resize(800,600);
}

MainWindow::~MainWindow()
{

}
