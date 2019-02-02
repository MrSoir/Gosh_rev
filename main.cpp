#include "mainwindow.h"

#include <QApplication>
#include <QThread>
#include <QDir>
#include <QString>
#include <QVector>

#include <string>
#include <vector>
#include <set>
#include <unordered_set>
#include <map>
#include <unordered_map>

#include "file_handling_src/fibdviewer.h"
#include "file_handling_src/dirmanagerinfo.h"

Q_DECLARE_METATYPE(QThread::Priority);

Q_DECLARE_METATYPE(std::string);
Q_DECLARE_METATYPE(QString);
Q_DECLARE_METATYPE(std::vector<std::string>);
Q_DECLARE_METATYPE(std::vector<QString>);
Q_DECLARE_METATYPE(QVector<QString>);

Q_DECLARE_METATYPE(FiBDViewer);
Q_DECLARE_METATYPE(std::vector<FiBDViewer>);

Q_DECLARE_METATYPE(DirManagerInfo);
Q_DECLARE_METATYPE(std::vector<DirManagerInfo>);

Q_DECLARE_METATYPE(QDir);
Q_DECLARE_METATYPE(std::vector<QDir>);
Q_DECLARE_METATYPE(QVector<QDir>);

void setStyles(QApplication& app)
{
    QFile styleFile(QString("styles%1%2").arg(QDir::separator()).arg("stylesheet.qss"));
    qDebug() << "styleFile.exists: " << styleFile.exists();
    if(styleFile.open(QIODevice::ReadOnly))
    {
        QTextStream textStream(&styleFile);
        QString styleSheet = textStream.readAll();
        styleFile.close();
        qDebug() << "styleSheet.length: " << styleSheet.length();
//        qDebug() << styleSheet;
        app.setStyleSheet(styleSheet);
    }
}

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    qRegisterMetaType<QThread::Priority>();

    qRegisterMetaType<std::string>();
    qRegisterMetaType<QString>();
    qRegisterMetaType<std::vector<std::string>>();
    qRegisterMetaType<std::vector<QString>>();
    qRegisterMetaType<QVector<QString>>();

    qRegisterMetaType<FiBDViewer>();
    qRegisterMetaType<std::vector<FiBDViewer>>();

    qRegisterMetaType<DirManagerInfo>();
    qRegisterMetaType<std::vector<DirManagerInfo>>();

    qRegisterMetaType<QDir>();
    qRegisterMetaType<std::vector<QDir>>();
    qRegisterMetaType<QVector<QDir>>();

    setStyles(a);

    MainWindow w;
    w.show();

    return a.exec();
}
