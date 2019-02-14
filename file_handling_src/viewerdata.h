#ifndef VIEWERDATA_H
#define VIEWERDATA_H

#include <QObject>

#include <unordered_map>

#include "fibdviewer.h"

#define int_bd long long

class ViewerData : public QObject
{
    Q_OBJECT
public:
    ViewerData();
    ViewerData(const ViewerData& vd);

    ViewerData& operator=(const ViewerData& vd);

    virtual ~ViewerData() override;

    std::unordered_map<int_bd, FiBDViewer> data;
};


#endif // VIEWERDATA_H
