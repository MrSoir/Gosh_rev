#ifndef CREATEENTRY_H
#define CREATEENTRY_H

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

enum CREATION_TYPE
{
    CREATE_FOLDER,
    CREATE_FILE
};

class CreateEntry: public TextDialogWorker
{
    Q_OBJECT
public:
    explicit CreateEntry(const std::string& baseDir,
                         QString dialog_message,
                         QString dialog_init_text,
                         CREATION_TYPE type); // baseDir == dir to create folder in
    explicit CreateEntry();
    explicit CreateEntry(const CreateEntry& ce);

    CreateEntry& operator=(const CreateEntry& ce);

    virtual ~CreateEntry() override;

public slots:
    virtual void run() override;
protected:
    virtual std::function<bool (const QString& txt)> getValidator() override;
private:
    void createEntry();

    std::string m_baseDir;
    CREATION_TYPE m_type;
};

#endif // CREATEENTRY_H
