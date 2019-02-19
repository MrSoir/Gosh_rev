#include "previewicon.h"

PreviewIcon::PreviewIcon(const std::string &path,
                         const QPixmap &icon)
    : QObject(nullptr),
      path(path),
      icon(icon)
{}

PreviewIcon::PreviewIcon(const PreviewIcon &pi)
    : QObject(pi.parent()),
      path(pi.path),
      icon(pi.icon)
{}

PreviewIcon::PreviewIcon()
    : QObject(nullptr),
      path(""),
      icon(QPixmap())
{}

PreviewIcon::~PreviewIcon()
{
    icon.detach();
}
