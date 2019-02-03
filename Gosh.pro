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
         orderby.cpp \
         pathmanipulation.cpp \
         process.cpp \
         staticfunctions.cpp \
         staticfunctions_files.cpp \
         stringops.cpp \
         file_handling_src/appinfo.cpp \
         file_handling_src/dirdeepsearchworker.cpp \
         file_handling_src/dirdeleteworker.cpp \
         file_handling_src/direlapseworker.cpp \
         file_handling_src/dirfilesystemwatcher.cpp \
         file_handling_src/dirmanager.cpp \
         file_handling_src/dirmanagerqueue.cpp \
         file_handling_src/dirmanagerworker.cpp \
         file_handling_src/dirrevalworker.cpp \
         file_handling_src/exec.cpp \
         file_handling_src/fibdviewer.cpp \
         file_handling_src/fileinfobd.cpp \
         file_handling_src/filemanager.cpp \
         file_handling_src/filemimetypehandler.cpp \
         file_handling_src/filequeue.cpp \
         file_handling_src/filesearcher.cpp \
         file_handling_src/fileselector.cpp \
         file_handling_src/filewatcher.cpp \
         file_handling_src/listfiles.cpp \
         file_handling_src/openfiles.cpp \
         file_handling_src/queuetask.cpp \
         file_handling_src/queuetasks.cpp \
         file_handling_src/searchable.cpp \
         file_handling_src/selectable.cpp \
         file_handling_src/taskshowhiddenfiles.cpp \
         file_manipulation/copyfiles.cpp \
         file_manipulation/createentry.cpp \
         file_manipulation/createfile.cpp \
         file_manipulation/createfolder.cpp \
         file_manipulation/progressdialog.cpp \
         file_manipulation/progressdialogworker.cpp \
         file_manipulation/renamefile.cpp \
         file_manipulation/sourcetargetiterator.cpp \
         file_manipulation/textdialog.cpp \
         file_manipulation/textdialogworker.cpp \
         file_manipulation/zipfiles.cpp \
         tab_coordiantor/tabcoordinator.cpp \
         tab_coordiantor/tabcoordinatorpane.cpp \
         threaded_dialog/dialogworker.cpp \
         threaded_dialog/threadeddialog.cpp \
         tree_viewer/directoryselectordialog.cpp \
         tree_viewer/directoryselectordialogviewer.cpp \
         tree_viewer/elapsemenubd.cpp \
         tree_viewer/filemanagerinfo.cpp \
         tree_viewer/graphicsfile.cpp \
         tree_viewer/graphicsview.cpp \
         tree_viewer/menubar.cpp \
         tree_viewer/rectcolor.cpp \
         tree_viewer/searchfiledialog.cpp \
         tree_viewer/searchmenubd.cpp \
         tree_viewer/waitingbargraphicsitem.cpp \
         tree_viewer/windowselector.cpp \
         window_coordinator/helpdialog.cpp \
         window_coordinator/infodialog.cpp \
         window_coordinator/removewindowdialog.cpp \
         window_coordinator/windowcoordinator.cpp \
         window_coordinator/windowcoordinatorpane.cpp \
        file_handling_src/dirmanagerinfo.cpp \
        file_handling_src/filedata.cpp \
        file_handling_src/dirreplacerootworker.cpp \
        file_handling_src/dircd_upworker.cpp \
        file_handling_src/dirincludehiddenfilesworker.cpp \
        file_handling_src/dirsortworker.cpp \
        tree_viewer/graphicsviewinfo.cpp \
        tree_viewer/directoryselectionpane.cpp \
        file_handling_src/directorystack.cpp \
        tab_coordiantor/tabselectorpane.cpp \
        customgraphicitems.cpp \
        tab_coordiantor/tabrect.cpp \
    widgetcreator.cpp \
    file_handling_src/image_retriever.cpp

HEADERS += \
         mainwindow.h \
         orderby.h \
         orientation.h \
         pathmanipulation.h \
         process.h \
         staticfunctions.h \
         staticfunctions_files.h \
         stringops.h \
         file_handling_src/appinfo.h \
         file_handling_src/dirdeepsearchworker.h \
         file_handling_src/dirdeleteworker.h \
         file_handling_src/direlapseworker.h \
         file_handling_src/dirfilesystemwatcher.h \
         file_handling_src/dirmanager.h \
         file_handling_src/dirmanagerqueue.h \
         file_handling_src/dirmanagerworker.h \
         file_handling_src/dirrevalworker.h \
         file_handling_src/exec.h \
         file_handling_src/fibdviewer.h \
         file_handling_src/fileinfobd.h \
         file_handling_src/filemanager.h \
         file_handling_src/filemimetypehandler.h \
         file_handling_src/filequeue.h \
         file_handling_src/filesearcher.h \
         file_handling_src/fileselector.h \
         file_handling_src/filewatcher.h \
         file_handling_src/listfiles.h \
         file_handling_src/openfiles.h \
         file_handling_src/queuetask.h \
         file_handling_src/queuetasks.h \
         file_handling_src/searchable.h \
         file_handling_src/selectable.h \
         file_handling_src/taskfolderelapser.h \
         file_handling_src/taskshowhiddenfiles.h \
         file_manipulation/copyfiles.h \
         file_manipulation/createentry.h \
         file_manipulation/createfile.h \
         file_manipulation/createfolder.h \
         file_manipulation/progressdialog.h \
         file_manipulation/progressdialogworker.h \
         file_manipulation/renamefile.h \
         file_manipulation/sourcetargetiterator.h \
         file_manipulation/textdialog.h \
         file_manipulation/textdialogworker.h \
         file_manipulation/zipfiles.h \
         tab_coordiantor/tabcoordinator.h \
         tab_coordiantor/tabcoordinatorpane.h \
         threaded_dialog/dialogworker.h \
         threaded_dialog/threadeddialog.h \
         tree_viewer/directoryselectordialog.h \
         tree_viewer/directoryselectordialogviewer.h \
         tree_viewer/dynamicfunctioncaller.h \
         tree_viewer/elapsemenubd.h \
         tree_viewer/filemanagerinfo.h \
         tree_viewer/graphicsfile.h \
         tree_viewer/graphicsview.h \
         tree_viewer/menubar.h \
         tree_viewer/rectcolor.h \
         tree_viewer/searchfiledialog.h \
         tree_viewer/searchmenubd.h \
         tree_viewer/waitingbargraphicsitem.h \
         tree_viewer/windowselector.h \
         window_coordinator/helpdialog.h \
         window_coordinator/infodialog.h \
         window_coordinator/removewindowdialog.h \
         window_coordinator/windowcoordinator.h \
         window_coordinator/windowcoordinatorpane.h \
        file_handling_src/dirmanagerinfo.h \
        file_handling_src/filedata.h \
        file_handling_src/dirreplacerootworker.h \
        file_handling_src/dircd_upworker.h \
        file_handling_src/dirincludehiddenfilesworker.h \
        file_handling_src/dirsortworker.h \
        tree_viewer/graphicsviewinfo.h \
        tree_viewer/directoryselectionpane.h \
        tree_viewer/graphicsviewupdater.h \
        file_handling_src/directorystack.h \
        tab_coordiantor/tabselectorpane.h \
        customgraphicitems.h \
        tab_coordiantor/tabrect.h \
        widgetcreator.h \
    file_handling_src/image_retriever.h

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
