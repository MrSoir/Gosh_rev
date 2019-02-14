#ifndef DIRMANAGERINFO_H
#define DIRMANAGERINFO_H

#include <QDateTime>

#include <string>
#include <vector>

#include "orderby.h"
#include "filedata.h"
#include "fileinfobd.h"
#include "staticfunctions_files.h"

class DirManagerInfo
{
public:
    explicit DirManagerInfo(std::string absPath,
                            std::string fileName,
                            bool isHidden,
                            bool isElapsed,
                            bool isLoaded,
                            QDateTime lastModified,
                            Order order);
    explicit DirManagerInfo(FileInfoBD* fi);

    explicit DirManagerInfo(const DirManagerInfo& dmi);

    explicit DirManagerInfo();

    DirManagerInfo& operator=(const DirManagerInfo& dmf);

    virtual ~DirManagerInfo();

    //------

    void replaceContents(DirManagerInfo* new_dmi);
    bool replaceContents_hlpr(DirManagerInfo* new_mdi);

    //------

    std::string absPath;
    std::string fileName;

    bool isHidden;
    bool isElapsed;
    bool isLoaded;

    QDateTime lastModified;

    Order order;

    std::vector<DirManagerInfo*> subDirs_sorted;
    std::vector<std::pair<std::string,std::string>> files_sorted; // path -> fileName
};

#endif // DIRMANAGERINFO_H
