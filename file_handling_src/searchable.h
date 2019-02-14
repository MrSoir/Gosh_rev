#ifndef SEARCHABLE_H
#define SEARCHABLE_H

#include <string>

class Searchable{
public:
    virtual ~Searchable();
    virtual void setSearched(bool searched) = 0;
    virtual void setSearchFocused(bool focused) = 0;
    virtual std::string path() const = 0;
};

#endif // SEARCHABLE_H
