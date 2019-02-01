#ifndef FILEDATA_H
#define FILEDATA_H

#include <QDateTime>
#include <QFileInfo>

#include <string>

class DirManagerInfo;

class FileData
{
public:
    explicit FileData(std::string absPath,
             std::string fileName,
             unsigned long long fileSize,
             QDateTime lastModified);

    explicit FileData(const QFileInfo& fi);

    virtual ~FileData();

    explicit FileData(const FileData& dmi);
    explicit FileData(FileData* dmi);

    explicit FileData(const DirManagerInfo& dmi);
    explicit FileData(DirManagerInfo* dmi);

    FileData& operator=(const FileData& fd);
    FileData* operator=(FileData* fd);

    std::string absPath;
    std::string fileName;

    unsigned long long fileSize;
    QDateTime lastModified;
};

#endif // FILEDATA_H
