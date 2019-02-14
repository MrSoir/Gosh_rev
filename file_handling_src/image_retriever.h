#ifndef IMAGE_RETRIEVER_H
#define IMAGE_RETRIEVER_H

#include <QObject>
#include <QPixmap>
#include <QSize>
#include <QString>
#include <QFileInfo>
#include <QThread>

#include <string>
#include <unordered_map>
#include <unordered_set>

#include "staticfunctions_files.h"

#define PIXMAP_GETTER_SIZE QSize(50,50)

class PixMapGetter : public QObject
{
    Q_OBJECT
public:
    explicit PixMapGetter(std::string path,
                          QSize pixmapSize = PIXMAP_GETTER_SIZE);
    explicit PixMapGetter();
    explicit PixMapGetter(const PixMapGetter& g);

    PixMapGetter& operator=(const PixMapGetter& g);

    virtual ~PixMapGetter() override;
signals:
    void finished();
    void sendPixmap(std::string path, QPixmap);
public slots:
    void start();
    void run();
private:
    void generatePixmap();

    std::string m_path;
    QSize m_pixmapSize;
    QPixmap m_pixmap;
};

//----------------------------------------------------------------_

class Image_Retriever : public QObject
{
    Q_OBJECT
public:
    explicit Image_Retriever(QObject *parent = nullptr);
    explicit Image_Retriever(const Image_Retriever& ir);

    Image_Retriever& operator=(const Image_Retriever& ir);

    virtual ~Image_Retriever() override;
signals:

public slots:
    void getImage(const std::string& path);
    void getImages(const std::vector<std::string>& paths);
    void receiveGeneratedPixmap(std::string path, QPixmap pixmap);
private:
    void retrieveImages(std::vector<std::string> paths);
    void retrieveImage(std::string path);

    std::unordered_set<std::string> m_paths;
    std::unordered_map<std::string, QPixmap> m_path_image;
    QSize m_imgSize;
};

#endif // IMAGE_RETRIEVER_H
