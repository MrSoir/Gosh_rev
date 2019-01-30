#ifndef CREATEFILE_H
#define CREATEFILE_H

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

class CreateFile: public CreateEntry
{
    Q_OBJECT
public:
    explicit CreateFile(const std::string& baseDir); // baseDir == dir to create folder in
    virtual ~CreateFile() override;
};

#endif // CREATEFILE_H
