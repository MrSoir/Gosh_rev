#ifndef IMAGELOADER_H
#define IMAGELOADER_H

#include <QObject>
#include <QString>
#include <QImage>
#include <QImageReader>
#include <QPixmap>
#include <QPicture>
#include <QPainter>
#include <QThread>
#include <QFileInfo>
#include <QFile>
#include <QDir>
#include <QMap>
#include <vector>
#include <unordered_set>
#include <unordered_map>
#include <queue>
#include <string>
#include <random>
#include <QDebug>

#include "staticfunctions_files.h"

class ImageLoader : public QObject
{
    Q_OBJECT
public:
    explicit ImageLoader(QSize img_size,
                         QObject *parent = nullptr);
    virtual ~ImageLoader() override;

    // const QImage als return-type, damit nicht QImage.detach() von ausserhalb aufgerufen werden kann - wuerde programm crashen!:
    QImage getImage(const QString& path) const;
    QImage getImage(const std::string& path) const;
signals:
    void newImagesLoaded();
    void cancelHelpers();
public slots:
    void loadImage(const std::string& path);
    void loadImages(const std::unordered_set<std::string> paths);
    void receiveImage(QString path, QImage img);

    void close();
private:
    void createImageLoaderHelper(const std::string& path);
    void loadImage_hlpr(const std::string& path);
    void loadFromWaitingQueue();
    void cleanPathsToLoad();

    QSize IMG_SIZE;
    QMap<QString, QImage> m_path_img;
    std::queue<std::string> m_paths;
    std::queue<std::string> m_paths_toLoad;
    std::unordered_set<std::string> m_paths_toLoad_set;
    std::unordered_set<std::string> m_paths_loading;
    std::size_t MAX_PATHS = 50;
    std::size_t MAX_PATHS_LOADING = 10;

    bool m_closed = false;
};

//------------------------------------------------------------

class ImageLoaderHelper : public QObject
{
    Q_OBJECT
public:
    explicit ImageLoaderHelper(const std::string& path,
                               QSize size = QSize(50,50),
                               QObject* parent = nullptr);
    virtual ~ImageLoaderHelper() override;

    void startThread();
signals:
    void finished();
    void sendImage(QString path, QImage m_img);
public slots:
    void run();
    void cancel();
private:
    void loadImage();

    QString m_path;
    QSize m_size;
    QImage m_img;

    bool m_cancelled = false;

    QString tarImgDirPath = QString("/home/hippo/Pictures/test_images");
};

#endif // IMAGELOADER_H
