#ifndef PREVIEWICONLOADER_H
#define PREVIEWICONLOADER_H

#include <QObject>
#include <QPixmap>
#include <QFileInfo>
#include <QDebug>

#include <string>
#include <unordered_map>
#include <unordered_set>

#include "queuetask.h"
#include "previewicon.h"
#include "staticfunctions_files.h"

class PreviewIconLoaderWorker : public QueueTask
{
    Q_OBJECT
public:
    explicit PreviewIconLoaderWorker(const std::unordered_set<std::string>& paths,
                               QSize size = QSize(50,50),
                               QObject* parent = nullptr);
    explicit PreviewIconLoaderWorker(const std::vector<std::string>& paths,
                               QSize size = QSize(50,50),
                               QObject* parent = nullptr);
    virtual ~PreviewIconLoaderWorker() override;

    virtual bool executableInParallel() const override;

signals:
    void sendPreviewIcons(std::vector<PreviewIcon>);
public slots:
    virtual void run() override;
private:
    std::vector<PreviewIcon> generatePreviewIcons() const;

    void loadIcons();

    std::unordered_set<std::string> m_paths;
    std::unordered_map<std::string, QPixmap> m_path_icons;
    QSize m_size;
};

#endif // PREVIEWICONLOADER_H
