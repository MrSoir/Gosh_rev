#ifndef OPENFILES_H
#define OPENFILES_H

#include <QObject>
#include <QString>
#include <QDebug>
#include <QFileInfo>
#include <QDesktopServices>
#include <QUrl>

#include <vector>
#include <string>

#include "queuetask.h"
#include "process.h"
#include "dialogs/openwithdialog.h"

class OpenFiles: public QueueTask
{
public:
    explicit OpenFiles(std::vector<std::string> pathsToOpen,
              bool openWith = false);
    virtual ~OpenFiles() override;

    virtual bool executableInParallel() const override;

public slots:
    virtual void run() override;
private:
    void open();

    std::vector<std::string> m_paths;
    bool m_openWith;
};

#endif // OPENFILES_H
