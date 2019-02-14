#include "viewerdata.h"

ViewerData::ViewerData()
    : QObject(nullptr),
      data(std::unordered_map<int_bd, FiBDViewer>())
{
}

ViewerData::ViewerData(const ViewerData &vd)
    : QObject(vd.parent()),
      data(vd.data)
{
}

ViewerData &ViewerData::operator=(const ViewerData &vd)
{
    this->setParent(vd.parent());
    data = vd.data;
    return *this;
}

ViewerData::~ViewerData()
{
}
