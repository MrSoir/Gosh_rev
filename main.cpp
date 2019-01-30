#include "mainwindow.h"

#include <QApplication>
#include <QThread>

Q_DECLARE_METATYPE(QThread::Priority);

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    qRegisterMetaType<QThread::Priority>();

    MainWindow w;
    w.show();

    return a.exec();
}
