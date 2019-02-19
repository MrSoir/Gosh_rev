#ifndef PREVIEWICONQUEUE_H
#define PREVIEWICONQUEUE_H

#include <QPixmap>
#include <QObject>

#include <string>
#include <unordered_map>
#include <unordered_set>

#include "previewicon.h"
#include "previewiconloaderworker.h"

#define ICON_LIMIT 20

class PreviewIconLoader : public QObject
{
    Q_OBJECT
public:
    explicit PreviewIconLoader(QSize size,
                              QObject *parent = nullptr);
    virtual ~PreviewIconLoader() override;

    std::unordered_map<std::string, QPixmap> getPreviewIconsMap(const std::unordered_set<std::string>& paths) const;
signals:
    void newIconsLoaded();
    void cancelWorkers();
public slots:
    void loadIcon(const std::string& path);
    void loadIcons(const std::vector<std::string>& paths);
    void loadIcons(const std::unordered_set<std::string>& paths);

    void receiveIcons(std::vector<PreviewIcon>);

    void close();
private:
    void loadIcons_Hlpr(const std::vector<string> &paths);
    void addIcon(const PreviewIcon& pi);

    std::queue<std::string> m_paths_queue;
    std::unordered_map<std::string, QPixmap> m_path_icon;

    QSize m_size;

    bool m_closed = false;
};

#endif // PREVIEWICONQUEUE_H
