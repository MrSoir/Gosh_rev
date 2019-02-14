#include "dirmanagerinfo.h"

DirManagerInfo::DirManagerInfo(std::string absPath,
                               std::string fileName,
                               bool isHidden,
                               bool isElapsed,
                               bool isLoaded,
                               QDateTime lastModified,
                               Order order)
    : absPath(absPath),
      fileName(fileName),
      isHidden(isHidden),
      isElapsed(isElapsed),
      isLoaded(isLoaded),
      lastModified(lastModified),
      order(order),
      subDirs_sorted(std::vector<DirManagerInfo*>()),
      files_sorted(std::vector<std::pair<std::string,std::string>>())
{
}

DirManagerInfo::DirManagerInfo(FileInfoBD* fi)
    : absPath(fi->absPath()),
      fileName(fi->fileName()),
      isHidden(fi->isHidden()),
      isElapsed(fi->isElapsed()),
      isLoaded(fi->isLoaded()),
      lastModified(fi->getFileInfo().lastModified()),
      order(fi->getOrder()),
      subDirs_sorted(std::vector<DirManagerInfo*>()),
      files_sorted(std::vector<std::pair<std::string,std::string>>())
{
    for(auto* sub_dir: fi->getSortedSubFolders())
    {
        subDirs_sorted.push_back( new DirManagerInfo(sub_dir) );
    }


    for(const auto& file_path: fi->getSortedFiles())
    {
        files_sorted.push_back( std::make_pair(file_path, fi->getFileName(file_path)) );
    }
}

//-------------------------

DirManagerInfo::DirManagerInfo(const DirManagerInfo& dmi)
    : absPath(dmi.absPath),
      fileName(dmi.fileName),
      isHidden(dmi.isHidden),
      isElapsed(dmi.isElapsed),
      isLoaded(dmi.isLoaded),
      lastModified(dmi.lastModified),
      order(dmi.order),
      subDirs_sorted(dmi.subDirs_sorted),
      files_sorted(dmi.files_sorted)
{
}

DirManagerInfo::DirManagerInfo()
    : absPath(""),
      fileName(""),
      isHidden(false),
      isElapsed(false),
      isLoaded(false),
      lastModified(QDateTime()),
      order(Order()),
      subDirs_sorted(std::vector<DirManagerInfo*>()),
      files_sorted(std::vector<std::pair<std::string,std::string>>())
{
}

//-------------------------

DirManagerInfo &DirManagerInfo::operator=(const DirManagerInfo &dmf)
{
    absPath        = dmf.absPath;
    fileName       = dmf.fileName;
    isHidden       = dmf.isHidden;
    isElapsed      = dmf.isElapsed;
    isLoaded       = dmf.isLoaded;
    lastModified   = dmf.lastModified;
    order          = dmf.order;
    subDirs_sorted = dmf.subDirs_sorted;
    files_sorted   = dmf.files_sorted;
    return *this;
}

//-------------------------

DirManagerInfo::~DirManagerInfo()
{
    while(subDirs_sorted.size() > 0)
    {
        DirManagerInfo* dmi = subDirs_sorted[0];
        subDirs_sorted.erase(subDirs_sorted.begin());
        delete dmi;
    }
}

//-------------------------

void DirManagerInfo::replaceContents(DirManagerInfo* new_dmi)
{
    replaceContents_hlpr(new_dmi);
}

bool DirManagerInfo::replaceContents_hlpr(DirManagerInfo* new_dmi)
{
    if(absPath == new_dmi->absPath)
    {
        absPath        = new_dmi->absPath;
        fileName       = new_dmi->fileName;
        isHidden       = new_dmi->isHidden;
        isElapsed      = new_dmi->isElapsed;
        isLoaded       = new_dmi->isLoaded;
        lastModified   = new_dmi->lastModified;
        order          = new_dmi->order;
        subDirs_sorted = new_dmi->subDirs_sorted;
        files_sorted   = new_dmi->files_sorted;
        return true;
    }
    for(auto* sub_dir: subDirs_sorted)
    {
        bool isSubDir = STATIC_FUNCTIONS::isSubDirectory(new_dmi->absPath, this->absPath);
        if(isSubDir)
        {
            bool successfully_replaced = replaceContents_hlpr(sub_dir);
            if(successfully_replaced)
                return true;
        }
    }
    return false;
}
