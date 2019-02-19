#ifndef PREVIEWICON_H
#define PREVIEWICON_H

#include <QPixmap>
#include <QObject>

#include <string>
#include <unordered_map>
#include <unordered_set>

class PreviewIcon : public QObject{
    Q_OBJECT
public:
    explicit PreviewIcon(const std::string& path,
                         const QPixmap& icon);
    explicit PreviewIcon(const PreviewIcon& pi);
    explicit PreviewIcon();

    virtual ~PreviewIcon() override;

    std::string path;
    QPixmap icon;
};

#endif // PREVIEWICON_H
