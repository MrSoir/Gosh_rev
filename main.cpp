#include "mainwindow.h"

#include <QApplication>
#include <QThread>
#include <QDir>
#include <QString>
#include <QVector>
#include <QSize>

#include <string>
#include <vector>
#include <set>
#include <unordered_set>
#include <map>
#include <unordered_map>

#include "file_handling_src/fibdviewer.h"
#include "file_handling_src/dirmanagerinfo.h"
#include "file_handling_src/dirdeepsearchworker.h"

#include "orderby.h"
#include "staticfunctions_files.h"

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

Q_DECLARE_METATYPE(Order);

Q_DECLARE_METATYPE(DeepSearchResult);
Q_DECLARE_METATYPE(std::vector<DeepSearchResult>);


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

    qRegisterMetaType<Order>();

    qRegisterMetaType<DeepSearchResult>();
    qRegisterMetaType<std::vector<DeepSearchResult>>();

//    QPixmap pm = STATIC_FUNCTIONS::getPixmapFromPDF(QString("/home/hippo/Documents/stones.pdf"), QSize(50,50));
//    qDebug() << "pixmap-size: " << pm.size();

    a.thread()->setObjectName(QString("MAIN_APP_THREAD"));

    setStyles(a);

    MainWindow w;
    w.show();

    return a.exec();
}
