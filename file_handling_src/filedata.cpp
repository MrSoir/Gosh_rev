//#include "filedata.h"

//#include "dirmanagerinfo.h"

//FileData::FileData(std::string absPath, std::string fileName, unsigned long long fileSize, QDateTime lastModified)
//    : absPath(absPath),
//      fileName(fileName),
//      fileSize(fileSize),
//      lastModified(lastModified)
//{
//}

//FileData::FileData(const QFileInfo &fi)
//    : absPath(fi.absoluteFilePath().toStdString()),
//      fileName(fi.fileName().toStdString()),
//      fileSize(static_cast<unsigned long long>(fi.size())),
//      lastModified(fi.lastModified())
//{
//}

//FileData::~FileData()
//{
//}

////--------------

//FileData::FileData(const FileData& fd)
//    : absPath(fd.absPath),
//      fileName(fd.fileName),
//      fileSize(fd.fileSize),
//      lastModified(fd.lastModified)
//{
//}

////--------------

//FileData::FileData(const DirManagerInfo& dmi)
//    : absPath(dmi.absPath),
//      fileName(dmi.fileName),
//      fileSize(0),
//      lastModified(dmi.lastModified)
//{
//}

////--------------

//FileData& FileData::operator=(const FileData &fd)
//{
//    absPath = fd.absPath;
//    fileName = fd.fileName;
//    fileSize = fd.fileSize;
//    lastModified = fd.lastModified;
//    return *this;
//}
