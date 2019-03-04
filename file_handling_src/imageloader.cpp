#include "imageloader.h"

ImageLoader::ImageLoader(QSize img_size, QObject *parent)
    : QObject(parent),
      IMG_SIZE(img_size),
      m_path_img(QMap<QString, QImage>())
{
}

ImageLoader::~ImageLoader()
{
    for(auto it: m_path_img)
    {
        if( !it.isNull() && !it.isDetached() )
        {
            it.detach();
        }
    }
    QMap<QString, QImage>().swap(m_path_img);
    std::queue<std::string>().swap(m_paths);
    std::queue<std::string>().swap(m_paths_toLoad);
    std::unordered_set<std::string>().swap(m_paths_toLoad_set);
    std::unordered_set<std::string>().swap(m_paths_loading);
}

// getImage gibt nur deep-copies vom bild heraus, niemals nur eine flache kopie
// -> deep copy bezieht sich aufs data-sharing von QImage, siehe Qt-Docs
QImage ImageLoader::getImage(const QString &path) const
{
    if(m_path_img.find(path) != m_path_img.end())
    {
        const auto& img = m_path_img[path];
        if(!img.isNull() && !img.isDetached())
        {
            return QImage(img.bits(), img.width(), img.height(), img.format());
        }else{
            return QImage();
        }
    }
    return QImage();
}
QImage ImageLoader::getImage(const std::string &path) const
{
    return getImage(QString::fromStdString(path));
}

//-------------------------

void ImageLoader::loadImage(const std::string &path)
{
    if(m_closed)
        return;

    loadImage_hlpr(path);
}

void ImageLoader::loadImages(const std::unordered_set<std::string> paths)
{
    if(m_closed)
        return;

    for(const auto& path: paths)
    {
        loadImage_hlpr(path);
    }
}
void ImageLoader::loadImage_hlpr(const std::string &path)
{
    auto qpath = QString::fromStdString(path);
    if( !STATIC_FUNCTIONS::isSupportedImage(qpath) )
    {
        return;
    }

    if( m_path_img.find(qpath) == m_path_img.end() &&
            !(m_paths_toLoad_set.find(path) != m_paths_toLoad_set.end()
            || m_paths_loading.find(path)  != m_paths_loading.end()))
    {
        if(m_paths_loading.size() >= MAX_PATHS_LOADING)
        {
            m_paths_toLoad.push(path);
            m_paths_toLoad_set.insert(path);
            cleanPathsToLoad();
        }else{
            m_paths_loading.insert(path);
            createImageLoaderHelper(path);
        }
    }
}

//-------------------------

void ImageLoader::cleanPathsToLoad()
{
    while(m_paths_toLoad.size() > MAX_PATHS)
    {
        auto tooOldPath = m_paths_toLoad.front();
        auto it = m_paths_toLoad_set.find(tooOldPath);
        if(it != m_paths_toLoad_set.end())
        {
            m_paths_toLoad_set.erase(it);
        }
        m_paths_toLoad.pop();
    }
}

void ImageLoader::receiveImage(QString path, QImage img)
{
    if(m_closed)
    {
        if(!img.isNull() && !img.isDetached())
        {
            img.detach();
        }
        return;
    }

    auto spath = path.toStdString();

    auto it = m_paths_loading.find(spath);
    if(it != m_paths_loading.end())
    {
        m_paths_loading.erase(it);
    }

    while(m_paths.size() > MAX_PATHS)
    {
        auto pathToDel = m_paths.front();
        auto img_it = m_path_img.find(QString::fromStdString(pathToDel));
        img_it->detach();
        m_path_img.erase(img_it);
        m_paths.pop();
    }
    this->m_paths.push(spath);
    this->m_path_img[path] = img;

    loadFromWaitingQueue();

    emit newImagesLoaded();
}

void ImageLoader::close()
{
    m_closed = true;
    emit cancelHelpers();
}

void ImageLoader::createImageLoaderHelper(const std::string& path)
{
    ImageLoaderHelper* hlpr = new ImageLoaderHelper(path, IMG_SIZE);
    connect(hlpr, &ImageLoaderHelper::sendImage, this, &ImageLoader::receiveImage, Qt::QueuedConnection);
    connect(this, &ImageLoader::cancelHelpers, hlpr, &ImageLoaderHelper::cancel, Qt::DirectConnection);
    hlpr->startThread();
}


void ImageLoader::loadFromWaitingQueue()
{
    while(m_paths_loading.size() < MAX_PATHS_LOADING &&
          m_paths_toLoad.size() > 0)
    {
        auto pathToLoad = m_paths_toLoad.front();
        auto it = m_paths_toLoad_set.find(pathToLoad);
        if(it != m_paths_toLoad_set.end())
        {
            m_paths_toLoad_set.erase(it);
        }
        createImageLoaderHelper(pathToLoad);
        m_paths_toLoad.pop();
    }
}



//------------------------------------------------------------------------------------------



ImageLoaderHelper::ImageLoaderHelper(const std::string &path,
                                     QSize size,
                                     QObject *parent)
    : QObject(parent),
      m_path(QString::fromStdString(path)),
      m_size(size),
      m_img(QImage())
{
}

ImageLoaderHelper::~ImageLoaderHelper()
{
}

void ImageLoaderHelper::startThread()
{
    QThread* thread = new QThread();
    this->moveToThread(thread);
    connect(thread, &QThread::started, this, &ImageLoaderHelper::run);
    connect(this, &ImageLoaderHelper::finished, this, &ImageLoaderHelper::deleteLater);
    connect(this, &ImageLoaderHelper::finished, thread, &QThread::quit);
    connect(thread, &QThread::finished, thread, &QThread::deleteLater, Qt::QueuedConnection);
    thread->start();
}

void ImageLoaderHelper::run()
{
    loadImage();

    if( !m_img.isNull() )
    {
        if(m_cancelled)
        {
            m_img.detach();
        }else{
            emit sendImage(m_path, m_img);
        }
    }
    emit finished();
}

void ImageLoaderHelper::cancel()
{
    m_cancelled = true;
}

void ImageLoaderHelper::loadImage()
{
    if(QFileInfo(m_path).exists() && !m_cancelled)
    {
        QImageReader imgRdr(m_path);
        imgRdr.setScaledSize(m_size);
        this->m_img = imgRdr.read();
    }
}
