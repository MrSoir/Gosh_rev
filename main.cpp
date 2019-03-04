#include "mainwindow.h"

#include <QApplication>
#include <QThread>
#include <QDir>
#include <QString>
#include <QVector>
#include <QSize>
#include <QDebug>
#include <QMetaType>

#include <string>
#include <vector>
#include <set>
#include <unordered_set>
#include <map>
#include <unordered_map>
#include <stack>

#include "file_handling_src/fibdviewer.h"
#include "file_handling_src/dirmanagerinfo.h"
#include "file_handling_src/dirdeepsearchworker.h"
#include "file_handling_src/viewerdata.h"

#include "file_manipulation/copyfiles.h"
#include "file_manipulation/createentry.h"
#include "file_manipulation/createfile.h"
#include "file_manipulation/createfolder.h"
#include "file_manipulation/renamefile.h"
#include "file_manipulation/zipfiles.h"

#include "orderby.h"
#include "staticfunctions_files.h"

#include "file_handling_src/filemimetypehandler.h"
//#include "file_handling_src/appinfo.h"

#define int_bd long long

//Q_DECLARE_SEQUENTIAL_CONTAINER_METATYPE(std::vector);
//Q_DECLARE_SEQUENTIAL_CONTAINER_METATYPE(std::list);
Q_DECLARE_SEQUENTIAL_CONTAINER_METATYPE(std::deque);
Q_DECLARE_SEQUENTIAL_CONTAINER_METATYPE(std::set);
Q_DECLARE_SEQUENTIAL_CONTAINER_METATYPE(std::unordered_set);
Q_DECLARE_SEQUENTIAL_CONTAINER_METATYPE(std::queue);
Q_DECLARE_SEQUENTIAL_CONTAINER_METATYPE(std::stack);

//Q_DECLARE_ASSOCIATIVE_CONTAINER_METATYPE(std::map);
Q_DECLARE_ASSOCIATIVE_CONTAINER_METATYPE(std::multimap);
Q_DECLARE_ASSOCIATIVE_CONTAINER_METATYPE(std::unordered_multimap);
Q_DECLARE_ASSOCIATIVE_CONTAINER_METATYPE(std::unordered_map);

//Q_DECLARE_METATYPE(AppInfo);
//Q_DECLARE_OPAQUE_POINTER(AppInfo*);
//Q_DECLARE_METATYPE(AppInfo*);

//Q_DECLARE_METATYPE(DirectoryStack);
//Q_DECLARE_OPAQUE_POINTER(DirectoryStack*);
//Q_DECLARE_METATYPE(DirectoryStack*);

Q_DECLARE_METATYPE(FileInfoBD);
Q_DECLARE_OPAQUE_POINTER(FileInfoBD*);
Q_DECLARE_METATYPE(FileInfoBD*);

Q_DECLARE_METATYPE(DirManagerInfo);
Q_DECLARE_OPAQUE_POINTER(DirManagerInfo*);
Q_DECLARE_METATYPE(DirManagerInfo*);

Q_DECLARE_METATYPE(FiBDViewer);
Q_DECLARE_OPAQUE_POINTER(FiBDViewer*);
Q_DECLARE_METATYPE(FiBDViewer*);

//Q_DECLARE_METATYPE(QFileInfo);
Q_DECLARE_OPAQUE_POINTER(QFileInfo*);
Q_DECLARE_METATYPE(QFileInfo*);

//Q_DECLARE_METATYPE(FileManager);
//Q_DECLARE_OPAQUE_POINTER(FileManager*);
//Q_DECLARE_METATYPE(FileManager*);

Q_DECLARE_METATYPE(FileMimeTypeHandler);
Q_DECLARE_OPAQUE_POINTER(FileMimeTypeHandler*);
Q_DECLARE_METATYPE(FileMimeTypeHandler*);

Q_DECLARE_METATYPE(FileQueue);
Q_DECLARE_OPAQUE_POINTER(FileQueue*);
Q_DECLARE_METATYPE(FileQueue*);

// QueueTask is abstract - not possible to register - fucked up qt...
//Q_DECLARE_METATYPE(QueueTask);
//Q_DECLARE_OPAQUE_POINTER(QueueTask*);
//Q_DECLARE_METATYPE(QueueTask*);

Q_DECLARE_METATYPE(OpenFiles);
Q_DECLARE_OPAQUE_POINTER(OpenFiles*);
Q_DECLARE_METATYPE(OpenFiles*);

// DialogWorker is abstract - not possible to register - fucked up qt...
//Q_DECLARE_METATYPE(DialogWorker);
//Q_DECLARE_OPAQUE_POINTER(DialogWorker*);
//Q_DECLARE_METATYPE(DialogWorker*);
//-------
// TextDialogWorker is abstract - not possible to register - fucked up qt...
//Q_DECLARE_METATYPE(TextDialogWorker);
//Q_DECLARE_OPAQUE_POINTER(TextDialogWorker*);
//Q_DECLARE_METATYPE(TextDialogWorker*);

Q_DECLARE_METATYPE(CreateEntry);
Q_DECLARE_OPAQUE_POINTER(CreateEntry*);
Q_DECLARE_METATYPE(CreateEntry*);

Q_DECLARE_METATYPE(RenameFile);
Q_DECLARE_OPAQUE_POINTER(RenameFile*);
Q_DECLARE_METATYPE(RenameFile*);
//-------
// ProgressDialogWorker is abstract - not possible to register - fucked up qt...
//Q_DECLARE_METATYPE(ProgressDialogWorker);
//Q_DECLARE_OPAQUE_POINTER(ProgressDialogWorker*);
//Q_DECLARE_METATYPE(ProgressDialogWorker*);

Q_DECLARE_METATYPE(COPY_MOVE::CopyFiles);
Q_DECLARE_OPAQUE_POINTER(COPY_MOVE::CopyFiles*);
Q_DECLARE_METATYPE(COPY_MOVE::CopyFiles*);

Q_DECLARE_METATYPE(COPY_MOVE::CopyFilesHelper);
Q_DECLARE_OPAQUE_POINTER(COPY_MOVE::CopyFilesHelper*);
Q_DECLARE_METATYPE(COPY_MOVE::CopyFilesHelper*);

Q_DECLARE_METATYPE(COPY_MOVE::DuplicateFiles);
Q_DECLARE_OPAQUE_POINTER(COPY_MOVE::DuplicateFiles*);
Q_DECLARE_METATYPE(COPY_MOVE::DuplicateFiles*);

Q_DECLARE_METATYPE(COPY_MOVE::MoveFiles);
Q_DECLARE_OPAQUE_POINTER(COPY_MOVE::MoveFiles*);
Q_DECLARE_METATYPE(COPY_MOVE::MoveFiles*);

Q_DECLARE_METATYPE(COPY_MOVE::DeleteFiles);
Q_DECLARE_OPAQUE_POINTER(COPY_MOVE::DeleteFiles*);
Q_DECLARE_METATYPE(COPY_MOVE::DeleteFiles*);

Q_DECLARE_METATYPE(ZIP::ZipFiles);
Q_DECLARE_OPAQUE_POINTER(ZIP::ZipFiles*);
Q_DECLARE_METATYPE(ZIP::ZipFiles*);

Q_DECLARE_METATYPE(ZIP::UnZipFile);
Q_DECLARE_OPAQUE_POINTER(ZIP::UnZipFile*);
Q_DECLARE_METATYPE(ZIP::UnZipFile*);
//-------
//Q_DECLARE_METATYPE(FileSearcher);
//Q_DECLARE_OPAQUE_POINTER(FileSearcher*);
//Q_DECLARE_METATYPE(FileSearcher*);

//Q_DECLARE_METATYPE(FileSelector);
//Q_DECLARE_OPAQUE_POINTER(FileSelector*);
//Q_DECLARE_METATYPE(FileSelector*);

//Q_DECLARE_METATYPE(FileWatcher);
//Q_DECLARE_OPAQUE_POINTER(FileWatcher*);
//Q_DECLARE_METATYPE(FileWatcher*);

Q_DECLARE_METATYPE(QSize);
Q_DECLARE_OPAQUE_POINTER(QSize*);
Q_DECLARE_METATYPE(QSize*);

Q_DECLARE_METATYPE(QPixmap);
Q_DECLARE_OPAQUE_POINTER(QPixmap*);
Q_DECLARE_METATYPE(QPixmap*);
Q_DECLARE_METATYPE(QImage);
Q_DECLARE_OPAQUE_POINTER(QImage*);
Q_DECLARE_METATYPE(QImage*);

//-----------------------------------
Q_DECLARE_METATYPE(QThread::Priority);
Q_DECLARE_OPAQUE_POINTER(QThread::Priority*);
Q_DECLARE_METATYPE(QThread::Priority*);

Q_DECLARE_METATYPE(std::string);
Q_DECLARE_OPAQUE_POINTER(std::string*);
Q_DECLARE_METATYPE(std::string*);

Q_DECLARE_METATYPE(QString);
Q_DECLARE_OPAQUE_POINTER(QString*);
Q_DECLARE_METATYPE(QString*);

Q_DECLARE_METATYPE(QDir);
Q_DECLARE_OPAQUE_POINTER(QDir*);
Q_DECLARE_METATYPE(QDir*);

Q_DECLARE_METATYPE(Order);
Q_DECLARE_OPAQUE_POINTER(Order*);
Q_DECLARE_METATYPE(Order*);

Q_DECLARE_METATYPE(DeepSearchResult);
Q_DECLARE_OPAQUE_POINTER(DeepSearchResult*);
Q_DECLARE_METATYPE(DeepSearchResult*);

Q_DECLARE_METATYPE(FiBDDeletor);
Q_DECLARE_OPAQUE_POINTER(FiBDDeletor*);
Q_DECLARE_METATYPE(FiBDDeletor*);

Q_DECLARE_METATYPE(QDateTime);
Q_DECLARE_OPAQUE_POINTER(QDateTime*);
Q_DECLARE_METATYPE(QDateTime*);

Q_DECLARE_METATYPE(ViewerData);
Q_DECLARE_OPAQUE_POINTER(ViewerData*);
Q_DECLARE_METATYPE(ViewerData*);

// ProgressDialogWorker is abstract - not possible to register - fucked up qt...
//Q_DECLARE_METATYPE(DirManagerWorker);
//Q_DECLARE_OPAQUE_POINTER(DirManagerWorker*);
//Q_DECLARE_METATYPE(DirManagerWorker*);

Q_DECLARE_METATYPE(DirDeepSearchWorker);
Q_DECLARE_OPAQUE_POINTER(DirDeepSearchWorker*);
Q_DECLARE_METATYPE(DirDeepSearchWorker*);

// QThread - no copy-constructor declared -> not possible to use in signal-slot!
//Q_DECLARE_METATYPE(QThread);
//Q_DECLARE_OPAQUE_POINTER(QThread*);
//Q_DECLARE_METATYPE(QThread*);


// declare containers:
Q_DECLARE_METATYPE(std::vector<DirectoryStack>);
Q_DECLARE_METATYPE(std::vector<DirectoryStack*>);
Q_DECLARE_METATYPE(std::vector<FileInfoBD>);
Q_DECLARE_METATYPE(std::vector<FileInfoBD*>);
Q_DECLARE_METATYPE(std::vector<DirManagerInfo>);
Q_DECLARE_METATYPE(std::vector<DirManagerInfo*>);

Q_DECLARE_METATYPE(std::vector<FiBDViewer>);
Q_DECLARE_METATYPE(std::vector<FiBDViewer*>);
typedef std::unordered_map<long long,FiBDViewer>  Map_LongLong_FiBDViewer;
typedef std::unordered_map<long long,FiBDViewer*> Map_LongLong_FiBDViewerPntr;
Q_DECLARE_METATYPE(Map_LongLong_FiBDViewer);
Q_DECLARE_METATYPE(Map_LongLong_FiBDViewerPntr);

Q_DECLARE_METATYPE(std::vector<QFileInfo*>);
Q_DECLARE_METATYPE(std::vector<FileMimeTypeHandler>);
Q_DECLARE_METATYPE(std::vector<FileMimeTypeHandler*>);
Q_DECLARE_METATYPE(std::vector<FileQueue>);
Q_DECLARE_METATYPE(std::vector<FileQueue*>);
Q_DECLARE_METATYPE(std::vector<OpenFiles>);
Q_DECLARE_METATYPE(std::vector<OpenFiles*>);
Q_DECLARE_METATYPE(std::vector<CreateEntry>);
Q_DECLARE_METATYPE(std::vector<CreateEntry*>);
Q_DECLARE_METATYPE(std::vector<RenameFile>);
Q_DECLARE_METATYPE(std::vector<RenameFile*>);
Q_DECLARE_METATYPE(std::vector<COPY_MOVE::CopyFiles>);
Q_DECLARE_METATYPE(std::vector<COPY_MOVE::CopyFiles*>);
Q_DECLARE_METATYPE(std::vector<COPY_MOVE::CopyFilesHelper>);
Q_DECLARE_METATYPE(std::vector<COPY_MOVE::CopyFilesHelper*>);
Q_DECLARE_METATYPE(std::vector<COPY_MOVE::DuplicateFiles>);
Q_DECLARE_METATYPE(std::vector<COPY_MOVE::DuplicateFiles*>);
Q_DECLARE_METATYPE(std::vector<COPY_MOVE::MoveFiles>);
Q_DECLARE_METATYPE(std::vector<COPY_MOVE::MoveFiles*>);
Q_DECLARE_METATYPE(std::vector<COPY_MOVE::DeleteFiles>);
Q_DECLARE_METATYPE(std::vector<COPY_MOVE::DeleteFiles*>);
Q_DECLARE_METATYPE(std::vector<ZIP::ZipFiles>);
Q_DECLARE_METATYPE(std::vector<ZIP::ZipFiles*>);
Q_DECLARE_METATYPE(std::vector<ZIP::UnZipFile>);
Q_DECLARE_METATYPE(std::vector<ZIP::UnZipFile*>);
Q_DECLARE_METATYPE(std::vector<QSize>);
Q_DECLARE_METATYPE(std::vector<QSize*>);
Q_DECLARE_METATYPE(std::vector<QPixmap>);
Q_DECLARE_METATYPE(std::vector<QPixmap*>);
Q_DECLARE_METATYPE(std::vector<QImage>);
Q_DECLARE_METATYPE(std::vector<QImage*>);

//Q_DECLARE_METATYPE(std::vector<PreviewIcon>);
//Q_DECLARE_METATYPE(std::vector<PreviewIcon*>);
//typedef std::unordered_map<std::string, PreviewIcon>  Map_string_PreviewIcon;
//typedef std::unordered_map<std::string, PreviewIcon*> Map_string_PreviewIconPntr;
//Q_DECLARE_METATYPE(Map_string_PreviewIcon);
//Q_DECLARE_METATYPE(Map_string_PreviewIconPntr);

Q_DECLARE_METATYPE(std::vector<QThread::Priority>);
Q_DECLARE_METATYPE(std::vector<QThread::Priority*>);
Q_DECLARE_METATYPE(std::vector<std::string>);
Q_DECLARE_METATYPE(std::vector<std::string*>);
Q_DECLARE_METATYPE(std::vector<QString>);
Q_DECLARE_METATYPE(std::vector<QString*>);
Q_DECLARE_METATYPE(std::vector<QDir>);
Q_DECLARE_METATYPE(std::vector<QDir*>);
Q_DECLARE_METATYPE(std::vector<Order>);
Q_DECLARE_METATYPE(std::vector<Order*>);
Q_DECLARE_METATYPE(std::vector<DeepSearchResult>);
Q_DECLARE_METATYPE(std::vector<DeepSearchResult*>);
Q_DECLARE_METATYPE(std::vector<FiBDDeletor>);
Q_DECLARE_METATYPE(std::vector<FiBDDeletor*>);
Q_DECLARE_METATYPE(std::vector<QDateTime>);
Q_DECLARE_METATYPE(std::vector<QDateTime*>);
Q_DECLARE_METATYPE(std::vector<DirDeepSearchWorker>);
Q_DECLARE_METATYPE(std::vector<DirDeepSearchWorker*>);




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

void registerTypes()
{
//    qRegisterMetaType<DirectoryStack>();
//    qRegisterMetaType<DirectoryStack*>();
    qRegisterMetaType<FileInfoBD>();
    qRegisterMetaType<FileInfoBD*>();
    qRegisterMetaType<DirManagerInfo>();
    qRegisterMetaType<DirManagerInfo*>();
    qRegisterMetaType<FiBDViewer>();
    qRegisterMetaType<FiBDViewer*>();
    qRegisterMetaType<QFileInfo*>();
    qRegisterMetaType<FileMimeTypeHandler>();
    qRegisterMetaType<FileMimeTypeHandler*>();
    qRegisterMetaType<FileQueue>();
    qRegisterMetaType<FileQueue*>();
    qRegisterMetaType<OpenFiles>();
    qRegisterMetaType<OpenFiles*>();
    qRegisterMetaType<CreateEntry>();
    qRegisterMetaType<CreateEntry*>();
    qRegisterMetaType<RenameFile>();
    qRegisterMetaType<RenameFile*>();
    qRegisterMetaType<COPY_MOVE::CopyFiles>();
    qRegisterMetaType<COPY_MOVE::CopyFiles*>();
    qRegisterMetaType<COPY_MOVE::CopyFilesHelper>();
    qRegisterMetaType<COPY_MOVE::CopyFilesHelper*>();
    qRegisterMetaType<COPY_MOVE::DuplicateFiles>();
    qRegisterMetaType<COPY_MOVE::DuplicateFiles*>();
    qRegisterMetaType<COPY_MOVE::MoveFiles>();
    qRegisterMetaType<COPY_MOVE::MoveFiles*>();
    qRegisterMetaType<COPY_MOVE::DeleteFiles>();
    qRegisterMetaType<COPY_MOVE::DeleteFiles*>();
    qRegisterMetaType<ZIP::ZipFiles>();
    qRegisterMetaType<ZIP::ZipFiles*>();
    qRegisterMetaType<ZIP::UnZipFile>();
    qRegisterMetaType<ZIP::UnZipFile*>();
    qRegisterMetaType<QSize>();
    qRegisterMetaType<QSize*>();
    qRegisterMetaType<QPixmap>();
    qRegisterMetaType<QPixmap*>();
    qRegisterMetaType<QImage>();
    qRegisterMetaType<QImage*>();
    qRegisterMetaType<QThread::Priority>();
    qRegisterMetaType<QThread::Priority*>();
    qRegisterMetaType<std::string>();
    qRegisterMetaType<std::string*>();
    qRegisterMetaType<QString>();
    qRegisterMetaType<QString*>();
    qRegisterMetaType<QDir>();
    qRegisterMetaType<QDir*>();
    qRegisterMetaType<Order>();
    qRegisterMetaType<Order*>();
    qRegisterMetaType<DeepSearchResult>();
    qRegisterMetaType<DeepSearchResult*>();
    qRegisterMetaType<FiBDDeletor>();
    qRegisterMetaType<FiBDDeletor*>();
    qRegisterMetaType<QDateTime>();
    qRegisterMetaType<QDateTime*>();
    qRegisterMetaType<ViewerData>();
    qRegisterMetaType<ViewerData*>();
    qRegisterMetaType<DirDeepSearchWorker>();
    qRegisterMetaType<DirDeepSearchWorker*>();

    // register containers:
    qRegisterMetaType<std::vector<std::string>>();
    qRegisterMetaType<std::vector<DirectoryStack>>();
    qRegisterMetaType<std::vector<DirectoryStack*>>();
    qRegisterMetaType<std::vector<FileInfoBD>>();
    qRegisterMetaType<std::vector<FileInfoBD*>>();
    qRegisterMetaType<std::vector<DirManagerInfo>>();
    qRegisterMetaType<std::vector<DirManagerInfo*>>();
    qRegisterMetaType<std::vector<FiBDViewer>>();
    qRegisterMetaType<std::vector<FiBDViewer*>>();
    qRegisterMetaType<std::vector<QFileInfo*>>();
    qRegisterMetaType<std::vector<FileMimeTypeHandler>>();
    qRegisterMetaType<std::vector<FileMimeTypeHandler*>>();
    qRegisterMetaType<std::vector<FileQueue>>();
    qRegisterMetaType<std::vector<FileQueue*>>();
    qRegisterMetaType<std::vector<OpenFiles>>();
    qRegisterMetaType<std::vector<OpenFiles*>>();
    qRegisterMetaType<std::vector<CreateEntry>>();
    qRegisterMetaType<std::vector<CreateEntry*>>();
    qRegisterMetaType<std::vector<RenameFile>>();
    qRegisterMetaType<std::vector<RenameFile*>>();
    qRegisterMetaType<std::vector<COPY_MOVE::CopyFiles>>();
    qRegisterMetaType<std::vector<COPY_MOVE::CopyFiles*>>();
    qRegisterMetaType<std::vector<COPY_MOVE::CopyFilesHelper>>();
    qRegisterMetaType<std::vector<COPY_MOVE::CopyFilesHelper*>>();
    qRegisterMetaType<std::vector<COPY_MOVE::DuplicateFiles>>();
    qRegisterMetaType<std::vector<COPY_MOVE::DuplicateFiles*>>();
    qRegisterMetaType<std::vector<COPY_MOVE::MoveFiles>>();
    qRegisterMetaType<std::vector<COPY_MOVE::MoveFiles*>>();
    qRegisterMetaType<std::vector<COPY_MOVE::DeleteFiles>>();
    qRegisterMetaType<std::vector<COPY_MOVE::DeleteFiles*>>();
    qRegisterMetaType<std::vector<ZIP::ZipFiles>>();
    qRegisterMetaType<std::vector<ZIP::ZipFiles*>>();
    qRegisterMetaType<std::vector<ZIP::UnZipFile>>();
    qRegisterMetaType<std::vector<ZIP::UnZipFile*>>();
    qRegisterMetaType<std::vector<QSize>>();
    qRegisterMetaType<std::vector<QSize*>>();
    qRegisterMetaType<std::vector<QPixmap>>();
    qRegisterMetaType<std::vector<QPixmap*>>();
    qRegisterMetaType<std::vector<QImage>>();
    qRegisterMetaType<std::vector<QImage*>>();
//    qRegisterMetaType<std::vector<PreviewIcon>>();
//    qRegisterMetaType<std::vector<PreviewIcon*>>();
    qRegisterMetaType<std::vector<QThread::Priority>>();
    qRegisterMetaType<std::vector<QThread::Priority*>>();
    qRegisterMetaType<std::vector<std::string>>();
    qRegisterMetaType<std::vector<std::string*>>();
    qRegisterMetaType<std::vector<QString>>();
    qRegisterMetaType<std::vector<QString*>>();
    qRegisterMetaType<std::vector<QDir>>();
    qRegisterMetaType<std::vector<QDir*>>();
    qRegisterMetaType<std::vector<Order>>();
    qRegisterMetaType<std::vector<Order*>>();
    qRegisterMetaType<std::vector<DeepSearchResult>>();
    qRegisterMetaType<std::vector<DeepSearchResult*>>();
    qRegisterMetaType<std::vector<FiBDDeletor>>();
    qRegisterMetaType<std::vector<FiBDDeletor*>>();
    qRegisterMetaType<std::vector<QDateTime>>();
    qRegisterMetaType<std::vector<QDateTime*>>();
    qRegisterMetaType<std::vector<DirDeepSearchWorker>>();
    qRegisterMetaType<std::vector<DirDeepSearchWorker*>>();

    // register maps:
//    qRegisterMetaType<Map_string_PreviewIcon>();
//    qRegisterMetaType<Map_string_PreviewIconPntr>();
    qRegisterMetaType<Map_LongLong_FiBDViewer>();
    qRegisterMetaType<Map_LongLong_FiBDViewerPntr>();
}

std::vector<std::string> getCopyPaths()
{
    return {"/home/hippo/Documents/tests_src"};
//    return {"/home/hippo/Documents/tests_src/.hiddenFile.txt",
//        "/home/hippo/Documents/tests_src/test zip.zip",
//        "/home/hippo/Documents/tests_src/Effective_DevOps.pdf",
//        "/home/hippo/Documents/tests_src/file.txt",
//        "/home/hippo/Documents/tests_src/script.py",
//        "/home/hippo/Documents/tests_src/sncDir",
//        "/home/hippo/Documents/tests_src/.hiddenFolder",
//        "/home/hippo/Documents/tests_src/Gosh_Notes.html"};
}
std::string getTarDir()
{
    return "/home/hippo/Documents/tests_tar";
}

void moveTest()
{
    qDebug() << "moving starts...";
    auto tar_dir = "/home/hippo/Documents/tests_tar/tests_src";
    COPY_MOVE::MoveFiles* mf = new COPY_MOVE::MoveFiles({tar_dir}, getTarDir());
    QObject::connect(mf, &COPY_MOVE::CopyFiles::finished, [=](){
        qDebug() << "moveing finished";
    });
    mf->execute();
}

void copyTest(int cntr = 0)
{
    if(cntr > 1)
    {
        moveTest();
        return;
    }
    qDebug() << "copying starts...";

    auto src_paths = getCopyPaths();
    auto tar_path = getTarDir();
    COPY_MOVE::CopyFiles* cf = new COPY_MOVE::CopyFiles(src_paths, tar_path, false);
    QObject::connect(cf, &COPY_MOVE::CopyFiles::finished, [=](){
        qDebug() << "copying finished";
        copyTest(cntr+1);
    });
    cf->execute();
}
void deleteTest()
{
    qDebug() << "deletion starts...";
    COPY_MOVE::DeleteFiles* df = new COPY_MOVE::DeleteFiles({getTarDir()});
    QObject::connect(df, &COPY_MOVE::DeleteFiles::finished, [](){
        qDebug() << "deletion finished";
        copyTest();
    });
    df->execute();

}

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    registerTypes();

//    deleteTest();

    a.thread()->setObjectName(QString("MAIN_APP_THREAD"));
    STATIC_FUNCTIONS::MAIN_THREAD = a.thread();

    setStyles(a);

    MainWindow w;
    w.show();

    return a.exec();
}
