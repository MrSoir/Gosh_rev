#-------------------------------------------------
#
# Project created by QtCreator 2019-01-06T12:57:28
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Gosh
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

CONFIG += c++17

SOURCES += \
        main.cpp \
        mainwindow.cpp \
        file_handling_src/fileinfobd.cpp \
        file_handling_src/filemanager.cpp \
        file_handling_src/fileselector.cpp \
        file_handling_src/filesearcher.cpp \
        stringops.cpp \
        file_handling_src/fibdviewer.cpp \
        file_handling_src/filequeue.cpp \
        file_handling_src/filewatcher.cpp \
        file_handling_src/filemimetypehandler.cpp \
        file_handling_src/listfiles.cpp \
        file_handling_src/exec.cpp \
        file_handling_src/queuetasks.cpp \
        file_handling_src/appinfo.cpp \
        tab_coordiantor/tabcoordinator.cpp \
    tab_coordiantor/tabcoordinatorpane.cpp \
    window_coordinator/windowcoordinator.cpp \
    window_coordinator/windowcoordinatorpane.cpp \
    staticfunctions.cpp \
    window_coordinator/removewindowdialog.cpp \
    window_coordinator/helpdialog.cpp \
    window_coordinator/infodialog.cpp \
    tree_viewer/graphicsview.cpp \
    tree_viewer/searchfiledialog.cpp \
    tree_viewer/customgraphicitems.cpp \
    tree_viewer/menubar.cpp \
    tree_viewer/waitingbargraphicsitem.cpp \
    tree_viewer/rectcolor.cpp \
    tree_viewer/directoryselectordialog.cpp \
    tree_viewer/directoryselectordialogviewer.cpp \
    tree_viewer/graphicsfile.cpp \
    tree_viewer/searchmenubd.cpp \
    tree_viewer/elapsemenubd.cpp \
    tree_viewer/windowselector.cpp \
    tree_viewer/filemanagerinfo.cpp \
    file_handling_src/searchable.cpp \
    file_handling_src/selectable.cpp \
    orderby.cpp \
    file_handling_src/queuetask.cpp \
    file_manipulation/copyfiles.cpp \
    file_manipulation/sourcetargetiterator.cpp \
    threaded_dialog/threadeddialog.cpp \
    threaded_dialog/dialogworker.cpp \
    file_manipulation/zipfiles.cpp \
    pathmanipulation.cpp \
    staticfunctions_files.cpp \
    file_handling_src/openfiles.cpp \
    process.cpp \
    file_manipulation/renamefile.cpp \
    file_manipulation/progressdialog.cpp \
    file_manipulation/progressdialogworker.cpp \
    file_manipulation/textdialog.cpp \
    file_manipulation/textdialogworker.cpp \
    file_manipulation/createfolder.cpp \
    file_manipulation/createentry.cpp \
    file_manipulation/createfile.cpp

HEADERS += \
        mainwindow.h \
        file_handling_src/fileinfobd.h \
        file_handling_src/filemanager.h \
        file_handling_src/fileselector.h \
        file_handling_src/filesearcher.h \
        stringops.h \
        file_handling_src/fibdviewer.h \
        file_handling_src/filequeue.h \
        file_handling_src/filewatcher.h \
        file_handling_src/filemimetypehandler.h \
        file_handling_src/listfiles.h \
        file_handling_src/exec.h \
        file_handling_src/queuetasks.h \
        file_handling_src/appinfo.h \
        tab_coordiantor/tabcoordinator.h \
    tab_coordiantor/tabcoordinatorpane.h \
    window_coordinator/windowcoordinator.h \
    window_coordinator/windowcoordinatorpane.h \
    orientation.h \
    orderby.h \
    staticfunctions.h \
    window_coordinator/removewindowdialog.h \
    window_coordinator/helpdialog.h \
    window_coordinator/infodialog.h \
    tree_viewer/graphicsview.h \
    tree_viewer/searchfiledialog.h \
    tree_viewer/customgraphicitems.h \
    tree_viewer/menubar.h \
    tree_viewer/dynamicfunctioncaller.h \
    tree_viewer/waitingbargraphicsitem.h \
    tree_viewer/rectcolor.h \
    tree_viewer/directoryselectordialog.h \
    tree_viewer/directoryselectordialogviewer.h \
    tree_viewer/graphicsfile.h \
    tree_viewer/searchmenubd.h \
    tree_viewer/elapsemenubd.h \
    tree_viewer/windowselector.h \
    tree_viewer/filemanagerinfo.h \
    file_handling_src/searchable.h \
    file_handling_src/selectable.h \
    file_handling_src/queuetask.h \
    file_manipulation/copyfiles.h \
    file_manipulation/sourcetargetiterator.h \
    threaded_dialog/threadeddialog.h \
    threaded_dialog/dialogworker.h \
    file_manipulation/zipfiles.h \
    pathmanipulation.h \
    staticfunctions_files.h \
    file_handling_src/openfiles.h \
    process.h \
    file_manipulation/renamefile.h \
    file_manipulation/progressdialogworker.h \
    file_manipulation/progressdialog.h \
    file_manipulation/textdialog.h \
    file_manipulation/textdialogworker.h \
    file_manipulation/createfolder.h \
    file_manipulation/createentry.h \
    file_manipulation/createfile.h

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
