#include "renamefile.h"

RenameFile::RenameFile(const std::string& absPathToRename)
    : TextDialogWorker(QString("select a new file name"),
                       QFileInfo(QString::fromStdString(absPathToRename)).fileName()),
      m_absPathToRename(absPathToRename)
{
}

RenameFile::RenameFile()
    : TextDialogWorker(QString("select a new file name"),
                       ""),
      m_absPathToRename("")
{
}

RenameFile::RenameFile(const RenameFile &rf)
    : TextDialogWorker(rf),
      m_absPathToRename(rf.m_absPathToRename)
{
}

RenameFile &RenameFile::operator=(const RenameFile &rf)
{
    TextDialogWorker::operator=(rf);
    this->m_absPathToRename = rf.m_absPathToRename;
    return *this;
}


RenameFile::~RenameFile()
{
    qDebug() << "~RenameFile";
}

bool RenameFile::renameFile(const std::string &absFilePath, const std::string &newFilename)
{
    QString q_absFilePath = QString::fromStdString(absFilePath);

    QString q_basePath = PATH::getBasePath(q_absFilePath);

    QString q_absTarPath = PATH::join(q_basePath, QString::fromStdString(newFilename));
    return QFile().rename(q_absFilePath, q_absTarPath);
}

void RenameFile::run()
{
    renameFile(m_absPathToRename, m_receivedTextFromDialog.toStdString());
    finish();
}
