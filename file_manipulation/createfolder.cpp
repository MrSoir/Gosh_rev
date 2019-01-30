#include "createfolder.h"

#include "copyfiles.h"

CreateFolder::CreateFolder(const string &baseDir)
    : CreateEntry(baseDir,
                  QString("select a folder name"),
                  QString("new folder"),
                  CREATION_TYPE::CREATE_FOLDER)
{
}

CreateFolder::~CreateFolder()
{
}
