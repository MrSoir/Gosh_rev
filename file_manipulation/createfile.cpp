#include "createfile.h"


CreateFile::CreateFile(const std::string& baseDir)
    : CreateEntry(baseDir,
                  QString("select a file name"),
                  QString("new file"),
                  CREATION_TYPE::CREATE_FILE)
{
}

CreateFile::CreateFile(const CreateFile &cf)
    : CreateEntry(cf)
{
}

CreateFile::CreateFile()
    : CreateEntry()
{
}

CreateFile &CreateFile::operator=(const CreateFile &cf)
{
    CreateEntry::operator=(cf);
    return *this;
}

CreateFile::~CreateFile()
{
}
