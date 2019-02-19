#include "image_retriever.h"

Image_Retriever::Image_Retriever(QObject *parent)
    : QObject(parent),
      m_paths(std::unordered_set<std::string>()),
      m_path_image(std::unordered_map<std::string, QPixmap>()),
      m_imgSize(PIXMAP_GETTER_SIZE)
{
}

Image_Retriever::Image_Retriever(const Image_Retriever &ir)
    : QObject(ir.parent()),
      m_paths(ir.m_paths),
      m_path_image(ir.m_path_image),
      m_imgSize(ir.m_imgSize)
{
}

Image_Retriever &Image_Retriever::operator=(const Image_Retriever &ir)
{
    this->setParent(ir.parent());
    m_paths = ir.m_paths;
    m_path_image = ir.m_path_image;
    m_imgSize = ir.m_imgSize;

    return *this;
}

Image_Retriever::~Image_Retriever()
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
    : QObject(nullptr),
      m_path(path),
      m_pixmapSize(pixmapSize),
      m_pixmap(QPixmap())
{
    connect(this, &PixMapGetter::finished, this, &PixMapGetter::deleteLater);
}

PixMapGetter::PixMapGetter()
    : QObject(nullptr),
      m_path(""),
      m_pixmapSize(PIXMAP_GETTER_SIZE),
      m_pixmap(QPixmap())
{
    connect(this, &PixMapGetter::finished, this, &PixMapGetter::deleteLater);
}

PixMapGetter::PixMapGetter(const PixMapGetter &g)
    : QObject(g.parent()),
      m_path(g.m_path),
      m_pixmapSize(g.m_pixmapSize),
      m_pixmap(g.m_pixmap)
{
    connect(this, &PixMapGetter::finished, this, &PixMapGetter::deleteLater);
}

PixMapGetter &PixMapGetter::operator=(const PixMapGetter &g)
{
    this->setParent(g.parent());
    m_path = g.m_path;
    m_pixmapSize = g.m_pixmapSize;
    m_pixmap = g.m_pixmap;

    connect(this, &PixMapGetter::finished, this, &PixMapGetter::deleteLater);

    return *this;
}

PixMapGetter::~PixMapGetter()
{
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
        m_pixmap = QPixmap(q_path).scaled(m_pixmapSize,
                                          Qt::IgnoreAspectRatio, Qt::FastTransformation);
    }else if( STATIC_FUNCTIONS::isPDF(m_path) )
    {

    }
}
