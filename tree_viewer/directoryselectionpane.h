#ifndef DIRECTORYSELECTIONPANE_H
#define DIRECTORYSELECTIONPANE_H

#include <QObject>
#include <QGraphicsView>
#include <QFileInfo>
#include <QDebug>
#include <QFile>
#include <QDir>
#include <QColor>

#include <vector>
#include <functional>
#include "graphicsviewupdater.h"

#include "staticfunctions.h"
#include "customgraphicitems.h"

class DirectorySelectionPane : public QGraphicsView,
                               public GraphicsViewUpdater
{
    Q_OBJECT
public:
    explicit DirectorySelectionPane(QDir rootFolder,
                                    QWidget* parent = nullptr);
    virtual ~DirectorySelectionPane() override;

    void resizeEvent(QResizeEvent *event) override;

    void updateGraphicsView() override;

    void undoEnabled(std::function<bool ()> caller);
    void onUndo(std::function<void ()> caller);
    void onButtonClick(std::function<void (QDir)> caller);

signals:
    void undo();
    void buttonClicked(QDir dir);

public slots:
    void revalidate();
    void setFolder(std::string folder_path);
    void setFolder(QDir folder);
    void blockButtons(bool blockBtns);

    void enableUndo();
    void disableUndo();
private:
    void rePaintCanvas();
    void addDirGI(const QDir& dirName, QVector<GraphicItemsBD::TextRect*>& dirGItms);

    GraphicItemsBD::PixmapRect* createUndoPixmapItem(const QColor& undoColor, QPoint pos, int undo_pixmap_width = 15);

    QString showFileSelectorDialog(QString absPath);

    bool isBlocked();

//    ------------------------------------

    QDir m_folder;
    std::vector<QDir> m_cur_displd_folders;

    bool m_undoEnabled;

    QGraphicsScene* m_scene;

    bool m_blockButtons;
};

#endif // DIRECTORYSELECTIONPANE_H
