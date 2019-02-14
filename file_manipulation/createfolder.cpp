#include "createfolder.h"

#include "copyfiles.h"

CreateFolder::CreateFolder(const string &baseDir)
    : CreateEntry(baseDir,
                  QString("select a folder name"),
                  QString("new folder"),
                  CREATION_TYPE::CREATE_FOLDER)
{
}

CreateFolder::CreateFolder()
    : CreateEntry()
{
}

CreateFolder::CreateFolder(const CreateFolder &cf)
    : CreateEntry(cf)
{
}

CreateFolder &CreateFolder::operator=(const CreateFolder &cf)
{
    CreateEntry::operator=(cf);
    return *this;
}

CreateFolder::~CreateFolder()
{
}
