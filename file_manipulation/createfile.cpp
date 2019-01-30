#include "createfile.h"


CreateFile::CreateFile(const std::string& baseDir)
    : CreateEntry(baseDir,
                  QString("select a file name"),
                  QString("new file"),
                  CREATION_TYPE::CREATE_FILE)
{
}

CreateFile::~CreateFile()
{
}
