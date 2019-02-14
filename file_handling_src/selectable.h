#ifndef SELECTABLE_H
#define SELECTABLE_H

#include <string>

class Selectable{
public:
    virtual ~Selectable();
    virtual std::string path() const = 0;
    virtual void setSelected(bool selected) = 0;
};

#endif // SELECTABLE_H
