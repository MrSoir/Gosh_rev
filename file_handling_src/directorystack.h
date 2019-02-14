#ifndef DIRECTORYSTACK_H
#define DIRECTORYSTACK_H

#include <QObject>
#include <QDir>

#include <vector>
#include <string>

class DirectoryStack : public QObject
{
    Q_OBJECT
public:
    explicit DirectoryStack();
    explicit DirectoryStack(const DirectoryStack& ds);

    DirectoryStack& operator=(const DirectoryStack& ds);

    virtual ~DirectoryStack() override;

    void addDir(QDir dir);
    void addPath(std::string path);

    std::string pop_back();

    std::size_t size() const;

signals:
    void stackIsEmpty();
    void stackContainsPaths();
private:

    void addPath_hlpr(std::string s);

    std::vector<std::string> m_dirs;
};

#endif // DIRECTORYSTACK_H
