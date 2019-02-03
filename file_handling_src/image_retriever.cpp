#include "image_retriever.h"

Image_Retriever::Image_Retriever(QObject *parent) : QObject(parent)
{

}

void Image_Retriever::getImage(const std::string &path)
{
    retrieveImage(path);
}

void Image_Retriever::getImages(const std::vector<std::string> &paths)
{
    retrieveImages(paths);
}

void Image_Retriever::receiveGeneratedPixmap(string path, QPixmap pixmap)
{
    m_paths.insert(path);
    m_path_image.emplace(path, pixmap);
}

void Image_Retriever::retrieveImages(std::vector<std::string> paths)
{
    for(const auto& path: paths)
    {
        retrieveImage(path);
    }
}

void Image_Retriever::retrieveImage(std::string path)
{
    PixMapGetter* pmg = new PixMapGetter(path);
    connect(pmg, &PixMapGetter::sendPixmap, this, &Image_Retriever::receiveGeneratedPixmap, Qt::QueuedConnection);
    pmg->start();
}


//----------------------------------------------------------------------

PixMapGetter::PixMapGetter(string path,
                           QSize pixmapSize)
    : m_path(path),
      m_pixmapSize(pixmapSize)
{
    connect(this, &PixMapGetter::finished, this, &PixMapGetter::deleteLater);
}

void PixMapGetter::start()
{
    QThread* thread = new QThread();

    this->moveToThread(thread);

    connect(thread, &QThread::finished, thread, &QThread::deleteLater);
    connect(thread, &QThread::started, this, &PixMapGetter::run);

    thread->start();
}

void PixMapGetter::run()
{
    generatePixmap();

    emit sendPixmap(m_path, m_pixmap);
    emit finished();
}

void PixMapGetter::generatePixmap()
{
    QString q_path = QString::fromStdString(m_path);
    QFileInfo fi(q_path);
    if( STATIC_FUNCTIONS::isSupportedImage(m_path))
    {
        m_pixmap = QPixmap(q_path).scaled(m_pixmapSize);
    }else if( STATIC_FUNCTIONS::isPDF(m_path) )
    {

    }
}
