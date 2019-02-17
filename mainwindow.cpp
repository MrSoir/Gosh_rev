#include "mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
 TabCoordinator* tabCoordinator = new TabCoordinator(this);
    this->setCentralWidget( tabCoordinator->createWidget() );

    STATIC_FUNCTIONS::setIconToWidget(this);

    this->resize(800,600);
}

MainWindow::~MainWindow()
{

}
