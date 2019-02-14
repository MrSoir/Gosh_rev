#ifndef CREATEFOLDER_H
#define CREATEFOLDER_H

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

#include "createentry.h"
#include "staticfunctions_files.h"
#include "pathmanipulation.h"

class CreateFolder: public CreateEntry
{
    Q_OBJECT
public:
    explicit CreateFolder(const std::string& baseDir); // baseDir == dir to create folder in
    explicit CreateFolder();
    explicit CreateFolder(const CreateFolder& cf);

    CreateFolder& operator=(const CreateFolder& cf);

    virtual ~CreateFolder() override;
};

#endif // CREATEFOLDER_H
