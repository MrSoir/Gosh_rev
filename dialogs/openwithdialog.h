#ifndef OPENWITHDIALOG_H
#define OPENWITHDIALOG_H

#include <QObject>
#include <QGridLayout>
#include <QWidget>
#include <QDialog>
#include <QPushButton>
#include <QVBoxLayout>
#include <QDebug>
#include <QScrollArea>
#include <QLabel>
#include <QFileInfo>

#include <functional>
#include <memory>

#include "file_handling_src/appinfo.h"
#include "file_handling_src/filemimetypehandler.h"
#include "openwithgraphicsview.h"
#include "staticfunctions_files.h"

class OpenWithDialog : public QDialog
{
    Q_OBJECT
public:
    explicit OpenWithDialog(std::string fileToOpenPath,
                            QSize size = QSize(600,600),
                            QPoint pos = QPoint(500,100),
                            QWidget *parent = nullptr);
    virtual ~OpenWithDialog() override;

public slots:
    void onOkClicked();
    void selected(int id);
signals:
private:
    QFrame* genSeparator(int horizontal);

    void setMarginsOfLayout(QLayout* layout, int padding = 0);

    //-----------------------------

    std::string m_fileToOpenPath;

    FileMimeTypeHandler m_handler;

    int m_selectedProgramId = 0;
};

#endif // OPENWITHDIALOG_H
