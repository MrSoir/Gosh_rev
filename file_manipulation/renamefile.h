#ifndef RENAMEFILE_H
#define RENAMEFILE_H

#include <QObject>
#include <QDebug>
#include <QFile>
#include <QFileInfo>
#include <QFileInfoList>
#include <QDir>
#include <QString>
#include <QLineEdit>
#include <QInputDialog>
#include <QMessageBox>

#include <vector>
#include <string>
#include <unordered_set>
#include <functional>

#include "textdialogworker.h"
#include "staticfunctions_files.h"
#include "pathmanipulation.h"

class RenameFile: public TextDialogWorker
{
    Q_OBJECT
public:
    explicit RenameFile(const std::string& absPathToRename);
    virtual ~RenameFile() override;

    static bool renameFile(const std::string& absFilePath,
                           const std::string& newFilename);
public slots:
    virtual void run() override;
private:
    std::string m_absPathToRename;
};

#endif // RENAMEFILE_H
