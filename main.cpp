#include "mainwindow.h"

#include <QApplication>
#include <QThread>

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
Q_DECLARE_METATYPE(std::vector<std::string>);
Q_DECLARE_METATYPE(FiBDViewer);
Q_DECLARE_METATYPE(std::vector<FiBDViewer>);
Q_DECLARE_METATYPE(DirManagerInfo);
Q_DECLARE_METATYPE(std::vector<DirManagerInfo>);

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    qRegisterMetaType<QThread::Priority>();
    qRegisterMetaType<std::string>();
    qRegisterMetaType<std::vector<std::string>>();
    qRegisterMetaType<FiBDViewer>();
    qRegisterMetaType<std::vector<FiBDViewer>>();
    qRegisterMetaType<DirManagerInfo>();
    qRegisterMetaType<std::vector<DirManagerInfo>>();

    MainWindow w;
    w.show();

    return a.exec();
}
