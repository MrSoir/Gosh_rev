#include "filemanager.h"

FileManager::FileManager(std::string root_path,
                         bool showHiddenFiles)
    : QObject(nullptr),
      m_root_folder(nullptr),
      m_root_path(root_path),
      m_searcher(new FileSearcher(this)),
      m_selector(new FileSelector(this)),
      m_tasks_queue(new FileQueue()),
      m_watcher(nullptr),
      m_viewer(new GraphicsView()),
      m_depthId_elapsed(std::vector<bool>()),
      m_frstDispFile(0),
      m_lastDispFile(0),
      m_showHiddenFiles(showHiddenFiles),
      m_inSearchMode(false)
{
    connectSignals();

    setRoot(root_path);
}

FileManager::~FileManager()
{
    disconnectSiganls();

    deleteRoot();

    delete m_searcher; m_searcher = nullptr;
    delete m_selector; m_selector = nullptr;

    m_tasks_queue->deleteLater(); m_tasks_queue = nullptr;

    deleteFileWatcher();

    m_viewer->deleteLater(); m_viewer = nullptr;

//    m_searcher->deleteLater();
//    m_searcher->deleteLater();
//    m_tasks_queue->deleteLater();
//    m_watcher->deleteLater();
}

void FileManager::test()
{
    selectionChanged();
}

void FileManager::setRoot(const std::string& rootPath)
{
    if(m_root_folder)
        deleteRoot();

    m_root_path = rootPath;

    deleteFileWatcher();
    m_watcher = new FileWatcher(this);
    connect(m_watcher, &FileWatcher::directoryChanged, this, &FileManager::directoryChanged);
    connect(this, &FileManager::addPathToFileWatcher, m_watcher, &FileWatcher::addPath);


    if(m_root_folder && m_root_folder->absPath() != rootPath)
    {
        m_root_folder = new FileInfoBD(rootPath);
        revalidateEntries();
    }
}

void FileManager::closeFileInfoBD(std::string path)
{
    // falls parent-folder bereits geloescht wird, wird sub_folder ebenfalls FileInfoBD::requestClosing senden,
    // d.h. FileInfoBD::requestClosing wird vom parent-folder und von all seinen sub-foldern rekursiv
    // gesendet -> damit nicht x-mal geloescht wird, wird nur das vom parent-folder akzeptiert:
    if(m_folders.find(path) != m_folders.end())
    {
        FileInfoBD* fi = m_folders[path];
         // damit nicht im spaeteren verlauf vor beendigung des loeschens im FileManager der deltete FileInfoBD weiter referenziert wird. und da beim loeschen/deleten der FileInfoBD rekursiv geloescht wird, mussen auch alle sub-folder rekursiv aus den containern geloescht werden:
        fi->iterateOverFolders([=](FileInfoBD* subFold){
            removeFiBDFromContainers(subFold->absPath());
        });

        if(m_root_folder == fi)
        {
            m_root_folder = nullptr;
        }

        QueueTasks::TaskFolderCloser* closer = new QueueTasks::TaskFolderCloser(fi);
        connect(closer, &QueueTasks::TaskFolderCloser::finished, this, &FileManager::revalidateEntries);
        emit addQueueTask(closer);
    }
}

void FileManager::selectionChanged()
{
    revalidateViewer_helper();
}

void FileManager::searchResultsChanged()
{
    revalidateViewer_helper();
}

void FileManager::directoryChanged(std::string path)
{
    if(m_folders.find(path) != m_folders.end())
        return;

    auto* fiBD = m_folders[path];
    if(fiBD && m_tasks_queue)
    {
        QueueTasks::FileInfoBDRevalidator* revalidator = new QueueTasks::FileInfoBDRevalidator(fiBD);
        connect(revalidator, &QueueTasks::FileInfoBDRevalidator::finished, this, &FileManager::revalidateEntries);
        emit addQueueTask(revalidator);
    }
}

void FileManager::elapseFolder(std::string path)
{
    if(m_folders.find(path) != m_folders.end())
        return;

    auto* fiBD = m_folders[path];
    if(fiBD && m_tasks_queue)
    {
        QueueTasks::TaskFolderElapser* elapser = new QueueTasks::TaskFolderElapser(fiBD, false, false);
        connect(elapser, &QueueTasks::TaskFolderElapser::finished, this, &FileManager::revalidateEntries);
        emit addQueueTask(elapser);
    }
}
void FileManager::elapseFolder_rec(std::string path)
{
    if(m_folders.find(path) != m_folders.end())
        return;

    auto* fiBD = m_folders[path];
    if(fiBD && m_tasks_queue)
    {
        QueueTasks::TaskFolderElapser* elapser = new QueueTasks::TaskFolderElapser(fiBD, false, true);
        connect(elapser, &QueueTasks::TaskFolderElapser::finished, this, &FileManager::revalidateEntries);
        emit addQueueTask(elapser);
    }
}
void FileManager::collapseFolder(std::string path)
{
    if(m_folders.find(path) != m_folders.end())
        return;

    auto* fiBD = m_folders[path];
    if(fiBD && m_tasks_queue)
    {
        QueueTasks::TaskFolderElapser* collapser = new QueueTasks::TaskFolderElapser(fiBD, true, false);
        connect(collapser, &QueueTasks::TaskFolderElapser::finished, this, &FileManager::revalidateEntries);
        emit addQueueTask(collapser);
    }
}
void FileManager::collapseFolder_rec(std::string path)
{
    if(m_folders.find(path) != m_folders.end())
        return;

    auto* fiBD = m_folders[path];
    if(fiBD && m_tasks_queue)
    {
        QueueTasks::TaskFolderElapser* collapser = new QueueTasks::TaskFolderElapser(fiBD, true, true);
        connect(collapser, &QueueTasks::TaskFolderElapser::finished, this, &FileManager::revalidateEntries);
        emit addQueueTask(collapser);
    }
}
void FileManager::showHiddenFiles(std::string path)
{
    if(m_folders.find(path) != m_folders.end())
        return;

    auto* fiBD = m_folders[path];
    if(fiBD && m_tasks_queue)
    {
        QueueTasks::TaskShowHiddenFiles* shower = new QueueTasks::TaskShowHiddenFiles(fiBD, false, false);
        connect(shower, &QueueTasks::TaskShowHiddenFiles::finished, this, &FileManager::revalidateEntries);
        emit addQueueTask(shower);
    }
}
void FileManager::showHiddenFiles_rec(std::string path)
{
    if(m_folders.find(path) != m_folders.end())
        return;

    auto* fiBD = m_folders[path];
    if(fiBD && m_tasks_queue)
    {
        QueueTasks::TaskShowHiddenFiles* shower = new QueueTasks::TaskShowHiddenFiles(fiBD, false, true);
        connect(shower, &QueueTasks::TaskShowHiddenFiles::finished, this, &FileManager::revalidateEntries);
        emit addQueueTask(shower);
    }
}
void FileManager::hideHiddenFiles(std::string path)
{
    if(m_folders.find(path) != m_folders.end())
        return;

    auto* fiBD = m_folders[path];
    if(fiBD && m_tasks_queue)
    {
        QueueTasks::TaskShowHiddenFiles* hider = new QueueTasks::TaskShowHiddenFiles(fiBD, true, false);
        connect(hider, &QueueTasks::TaskShowHiddenFiles::finished, this, &FileManager::revalidateEntries);
        emit addQueueTask(hider);
    }
}
void FileManager::hideHiddenFiles_rec(std::string path)
{
    if(m_folders.find(path) != m_folders.end())
        return;

    auto* fiBD = m_folders[path];
    if(fiBD && m_tasks_queue)
    {
        QueueTasks::TaskShowHiddenFiles* hider = new QueueTasks::TaskShowHiddenFiles(fiBD, true, true);
        connect(hider, &QueueTasks::TaskShowHiddenFiles::finished, this, &FileManager::revalidateEntries);
        emit addQueueTask(hider);
    }
}

//-------------------------------------------------------------

void FileManager::elapseAllFoldersOfDepthId(int id)
{
    if(m_depth_folders_colpsd.find(id) != m_depth_folders_colpsd.end() &&
            m_tasks_queue)
    {
        m_depthId_elapsed[static_cast<unsigned long>(id)] = !m_depthId_elapsed[static_cast<unsigned long>(id)];
        bool collapse = !m_depthId_elapsed[static_cast<unsigned long>(id)];

        std::unordered_set<FileInfoBD*>& fis = m_depth_folders_colpsd[id];
        QueueTasks::TaskFolderElapser* elapser = new QueueTasks::TaskFolderElapser(fis, collapse, false);
        connect(elapser, &QueueTasks::TaskFolderElapser::finished, this, &FileManager::revalidateEntries);
        emit addQueueTask(elapser);
    }
}
void FileManager::elapseSelectedFoldersRecursively()
{
    if(m_selector && m_tasks_queue)
    {
        const auto& folder_paths = m_selector->getSelectedFolders();
        std::vector<FileInfoBD*> folders;
        for(const auto& fp: folder_paths)
        {
            if(m_folders.find(fp) != m_folders.end())
                folders.push_back(m_folders[fp]);
        }
        QueueTasks::TaskFolderElapser* elapser = new QueueTasks::TaskFolderElapser(folders, false, true);
        connect(elapser, &QueueTasks::TaskFolderElapser::finished, this, &FileManager::revalidateEntries);
        emit addQueueTask(elapser);
    }
}
void FileManager::elapseSelectedFolders()
{
    if(m_selector && m_tasks_queue)
    {
        const auto& folder_paths = m_selector->getSelectedFolders();
        std::vector<FileInfoBD*> folders;
        for(const auto& fp: folder_paths)
        {
            if(m_folders.find(fp) != m_folders.end())
                folders.push_back(m_folders[fp]);
        }
        QueueTasks::TaskFolderElapser* elapser = new QueueTasks::TaskFolderElapser(folders, false, false);
        connect(elapser, &QueueTasks::TaskFolderElapser::finished, this, &FileManager::revalidateEntries);
        emit addQueueTask(elapser);
    }
}
void FileManager::collapseSelectedFoldersRecursively()
{
    if(m_selector && m_tasks_queue)
    {
        const auto& folder_paths = m_selector->getSelectedFolders();
        std::vector<FileInfoBD*> folders;
        for(const auto& fp: folder_paths)
        {
            if(m_folders.find(fp) != m_folders.end())
                folders.push_back(m_folders[fp]);
        }
        QueueTasks::TaskFolderElapser* elapser = new QueueTasks::TaskFolderElapser(folders, true, true);
        connect(elapser, &QueueTasks::TaskFolderElapser::finished, this, &FileManager::revalidateEntries);
        emit addQueueTask(elapser);
    }
}
void FileManager::collapseSelectedFolders()
{
    if(m_selector && m_tasks_queue)
    {
        const auto& folder_paths = m_selector->getSelectedFolders();
        std::vector<FileInfoBD*> folders;
        for(const auto& fp: folder_paths)
        {
            if(m_folders.find(fp) != m_folders.end())
                folders.push_back(m_folders[fp]);
        }
        QueueTasks::TaskFolderElapser* elapser = new QueueTasks::TaskFolderElapser(folders, true, false);
        connect(elapser, &QueueTasks::TaskFolderElapser::finished, this, &FileManager::revalidateEntries);
        emit addQueueTask(elapser);
    }
}
void FileManager::elapseOrCollapseFolderDependingOnCurrentState(QString q_path)
{
    std::string path = q_path.toStdString();
    if(m_folders.find(path) != m_folders.end())
    {
        FileInfoBD* folder = m_folders[path];
        bool collapse = folder->isElapsed();
        QueueTasks::TaskFolderElapser* elapser = new QueueTasks::TaskFolderElapser(folder, collapse, false);
        connect(elapser, &QueueTasks::TaskFolderElapser::finished, this, &FileManager::revalidateEntries);
        emit addQueueTask(elapser);
    }
}

//-------------------------------------------------------------

void FileManager::setIncludeHiddenFiles(bool inclHdnFls)
{
    m_showHiddenFiles = inclHdnFls;
    revalidateEntries();
    revalidateViewer_helper();
}

void FileManager::openSelectedFoldersInTab()
{
    if(m_selector){
        const auto& sfs = m_selector->getSelectedFolders();
        std::vector<std::string> folders;
        for(const auto& sf: sfs)
        {
            folders.push_back(sf);
        }
        emit openFoldersInTab(folders);
    }
}

void FileManager::searchForKeyWord(QString keyword, bool deepsearch)
{
    Q_UNUSED(deepsearch);
    if(m_searcher)
    {
        m_searcher->search(keyword.toStdString());
        m_inSearchMode = true;
    }
}
void FileManager::selectEntireContent()
{
    if(m_selector)
    {
        m_selector->selectEntiresContent();
        revalidateViewer_helper();
    }
}
void FileManager::clearSelection()
{
    if(m_selector)
    {
        m_selector->clearSelection();
        revalidateViewer_helper();
    }
}
void FileManager::selectButtonUp(bool cntrlPrsd, bool shiftPrsd)
{
    if(m_selector)
    {
        m_selector->selectPrevious(cntrlPrsd, shiftPrsd);
        revalidateViewer_helper();
    }
}
void FileManager::selectButtonDown(bool cntrlPrsd, bool shiftPrsd)
{
    if(m_selector)
    {
        m_selector->selectNext(cntrlPrsd, shiftPrsd);
        revalidateViewer_helper();
    }
}
void FileManager::selectEntry(QString entry, bool contrlPrsd, bool shiftPrsd)
{
    if(m_selector)
    {
        m_selector->select(entry.toStdString(), contrlPrsd, shiftPrsd);
        revalidateViewer_helper();
    }
}
void FileManager::sortDisplFolder(QString dir, Order order)
{
    std::string path = dir.toStdString();
    if(m_folders.find(path) != m_folders.end())
    {
        FileInfoBD* folder = m_folders[path];
        QueueTasks::TaskFolderSorter* elapser = new QueueTasks::TaskFolderSorter(folder, order, false);
        connect(elapser, &QueueTasks::TaskFolderSorter::finished, this, &FileManager::revalidateEntries);
        emit addQueueTask(elapser);
    }
}
void FileManager::sortAllFolders(Order order)
{
    // es werden nur die displayed_folders sortiert. erst wenn die collapsed folders elapsed werden, werden die folders
    // sortiert. sonst wuerde unnoetig viel sortiert, was gar nicht zur darstellung gebracht wird
    std::vector<FileInfoBD*> folders(m_folders_colpsd.begin(), m_folders_colpsd.end());
    QueueTasks::TaskFolderSorter* elapser = new QueueTasks:: TaskFolderSorter(folders, order, false);
    connect(elapser, &QueueTasks::TaskFolderSorter::finished, this, &FileManager::revalidateEntries);
    emit addQueueTask(elapser);
}
void FileManager::copySelectedFilePathToClipboard()
{
    if(m_selector)
    {
        QClipboard* clipboard = QGuiApplication::clipboard();
        clipboard->setText( QString::fromStdString(m_selector->getLastSelectedEntry()) );
    }
}
void FileManager::copySelectedContent()
{
    copyCutToClipboard_hlpr(false);
}
void FileManager::cutSelectedContent()
{
    copyCutToClipboard_hlpr(true);
}
void FileManager::duplicateSelectedContent()
{
    if(m_selector)
    {
        const auto& sel_entries = m_selector->getSelectedEntries();
        std::vector<std::string> entries_vec(sel_entries.begin(), sel_entries.end());
        COPY_MOVE::DuplicateFiles* duplicateWorker = new COPY_MOVE::DuplicateFiles(entries_vec);
        emit addQueueTask(duplicateWorker);
    }
}
void FileManager::openSelectedContent()
{
    if(m_selector)
    {
        const auto& sel_entries = m_selector->getSelectedEntries();
        std::vector<std::string> entries_vec(sel_entries.begin(), sel_entries.end());
        OpenFiles* openFilesWorker = new OpenFiles(entries_vec);
        emit addQueueTask(openFilesWorker);
    }
}
void FileManager::openSelectedContentWith()
{
    if(m_selector)
    {
        const auto& sel_entries = m_selector->getSelectedEntries();
        std::vector<std::string> entries_vec(sel_entries.begin(), sel_entries.end());
        OpenFiles* openFilesWorker = new OpenFiles(entries_vec, true);
        emit addQueueTask(openFilesWorker);
    }
}

void FileManager::renameSelectedContent()
{
    if(m_selector)
    {
        const auto& sel_entries = m_selector->getSelectedEntries();
        if(sel_entries.size() > 0)
        {
            const auto& lastSelctdEntry = m_selector->getLastSelectedEntry();
            if( !lastSelctdEntry.empty() )
            {
                RenameFile* openFilesWorker = new RenameFile( lastSelctdEntry );
                emit addQueueTask(openFilesWorker);
            }
        }
    }
}

void FileManager::pasteFromClipboard()
{
    const QClipboard* clipboard = QApplication::clipboard();
    const QMimeData* mimeData = clipboard->mimeData();

    std::vector<std::string> paths;

    bool cut = false;

    if(mimeData->hasUrls()) {
        const auto& urls = mimeData->urls();

        auto pair = extractPathsFromUrls(urls);
        cut = pair.second;
        paths = pair.first;

    }else if(mimeData->hasText()) {
        QString paths_str = mimeData->text();

        auto pair = extractPathsFromPathsString(paths_str);
        cut = pair.second;
        paths = pair.first;
    }

    std::string tarPath = m_root_path;
    if(m_selector)
    {
        std::string lastSelectedPath = m_selector->getLastSelectedEntry();
        if(!lastSelectedPath.empty())
            tarPath = lastSelectedPath;
    }

    copyFiles(paths, tarPath, cut);
}

void FileManager::paste(QString dropStr, QString targetPath)
{
    auto pair = extractPathsFromPathsString(dropStr);
    auto cut = pair.second;
    auto paths = pair.first;

    copyFiles(paths, targetPath.toStdString(), cut);
}

void FileManager::deleteSelectedContent()
{
    if(m_selector)
    {
        const auto& sel_entries = m_selector->getSelectedEntries();
        std::vector<std::string> entries_vec(sel_entries.begin(), sel_entries.end());
        COPY_MOVE::DeleteFiles* deleteWorker = new COPY_MOVE::DeleteFiles(entries_vec);
        emit addQueueTask(deleteWorker);
    }
}

void FileManager::zipSelectedContent()
{
    if(m_selector)
    {
        const auto& sel_entries = m_selector->getSelectedEntries();
        std::vector<std::string> entries_vec(sel_entries.begin(), sel_entries.end());
        if(entries_vec.size() > 0)
        {
            // tarPath ermitteln: logik: sofern alle eintraege in demselben ordner sind, werden die dateien in eine datei im selben ordner gezippt. sind die dateien auf versch. ordner verteilt, werden sie im m_root_path gezippt
            std::string tarPath = m_root_path;
            QString basePath = PATH::getBasePath(entries_vec[0]);
            bool alleFilesInSameFolder = true;
            for(unsigned long i=1; i < entries_vec.size(); ++i)
            {
                QString curBasePath = PATH::getBasePath(entries_vec[i]);
                if(curBasePath != basePath)
                {
                    alleFilesInSameFolder = false;
                    break;
                }
            }
            if(alleFilesInSameFolder)
                tarPath = basePath.toStdString();

            tarPath = PATH::join(tarPath, "zipped.zip");
            tarPath = STATIC_FUNCTIONS::getUniqueFilePath("tarPath").toStdString();

            ZIP::ZipFiles* zipWorker = new ZIP::ZipFiles(entries_vec, tarPath);
            emit addQueueTask(zipWorker);
        }
    }
}

void FileManager::unzipSelectedContent()
{
    if(m_selector)
    {
        auto filePathToZip = m_selector->getLastSelectedEntry();
        ZIP::UnZipFile* unzipWorker = new ZIP::UnZipFile(filePathToZip);
        emit addQueueTask(unzipWorker);
    }
}

void FileManager::createNewFolder()
{
    std::string baseDir = getBaseDirOfSelection();
    CreateFolder* createFolderWorker = new CreateFolder(baseDir);
    emit addQueueTask(createFolderWorker);
}

void FileManager::createNewFile()
{
    std::string baseDir = getBaseDirOfSelection();
    CreateFile* createFileWorker = new CreateFile(baseDir);
    emit addQueueTask(createFileWorker);
}

void FileManager::killCurrentBlockingAction()
{

}

void FileManager::requestFocus()
{
    emit requestFocusSGNL(QDir(QString::fromStdString(m_root_path)));
}

void FileManager::initDragging(QString draggingSource)
{
    if(m_selector)
    {
        const auto& sel_paths = m_selector->getSelectedEntries();
        if(sel_paths.size() > 0)
        {
            QList<QUrl> urls;
            urls.append(QUrl("cut"));

            for(const auto& path: sel_paths)
            {
                urls.append( QUrl::fromLocalFile(QString::fromStdString(path)) );
            }

            QUrl initatorUrl = QUrl::fromLocalFile(draggingSource);
            if( !urls.contains(initatorUrl) ){
                urls.append( initatorUrl );
            }

            QDrag *drag = new QDrag(this);
            QMimeData *mimeData = new QMimeData;

            mimeData->setUrls( urls );
            drag->setMimeData(mimeData);
    //        drag->setPixmap(iconPixmap);

            Qt::DropAction dropAction = drag->exec();
            Q_UNUSED( dropAction )
        }
    }
}

void FileManager::keyPressed(char c)
{
    auto currentTime = getCurrentTime();
    auto timePassed = std::chrono::duration_cast<std::chrono::milliseconds>(currentTime - m_lastKeyPressed).count();
    m_lastKeyPressed = currentTime;

    if(timePassed > 300)
    {
        m_keysPressed.clear();
    }

    m_keysPressed.push_back(c);
    m_keysPressed = StringOps::toLower(m_keysPressed);


    if(m_selector)
    {
        std::string old_last_sel = m_selector->getLastSelectedEntry();

        m_selector->selectKeyWord(m_keysPressed);

        std::string new_last_sel = m_selector->getLastSelectedEntry();
        if( (old_last_sel.empty() && !new_last_sel.empty()) ||
            (old_last_sel != new_last_sel))
        {
            focusPath(new_last_sel);
        }
    }
}

void FileManager::setSelectionToRoot()
{
    if(m_selector)
    {
        std::string last_sel = m_selector->getLastSelectedEntry();
        if(!last_sel.empty())
        {
            setRoot(last_sel);
        }
    }
}

void FileManager::setParentToRoot()
{
    QString q_root_path = QString::fromStdString(m_root_path);
    QFileInfo root_fi(q_root_path);
    QDir rootPath(root_fi.absoluteFilePath());
    if(rootPath.cdUp()){ // QDor.cdUp() holt sich den parent-root-path, aber nur, wenn dieser auch existiert!
        this->setRoot(rootPath.absolutePath().toStdString());
    }
}

void FileManager::openTerminal()
{
    if(m_selector)
    {
        std::string last_sel = m_selector->getLastSelectedEntry();
        QString sel_dir = PATH::getDirFromPath(last_sel);
        STATIC_FUNCTIONS::openTerminal(sel_dir);
    }
}

void FileManager::zoomFactorChanged(int newZoomFactor)
{
    m_zoomFactor = newZoomFactor;
}

void FileManager::showHiddenFiles(bool showHiddenFiles)
{
    m_showHiddenFiles = showHiddenFiles;
    emit includeHiddenFiles(m_showHiddenFiles);
}

//-------------------------------------------------------------

void FileManager::deleteRoot()
{
    if(m_root_folder)
    {
        m_root_folder->close();
        delete m_root_folder;
        clearEntryContainers();
    }
}

void FileManager::deleteFileWatcher()
{
    if(m_watcher)
    {
        disconnect(m_watcher, &FileWatcher::directoryChanged, this, &FileManager::directoryChanged);
        delete m_watcher;
    }
}

void FileManager::connectSignals()
{
    if(m_searcher)
    {
        QObject::connect(this, &FileManager::search, m_searcher, &FileSearcher::search);
    }else
        qDebug() << "trying to connect m_searcher -> nullptr! [root_path: "  << QString::fromStdString(m_root_path) << "]";

    if(m_selector)
    {

    }else
        qDebug() << "trying to connect m_selector -> nullptr! [root_path: "  << QString::fromStdString(m_root_path) << "]";

    if(m_tasks_queue)
    {
        QObject::connect(this, &FileManager::addQueueTask,          m_tasks_queue, &FileQueue::addTask);
        QObject::connect(this, &FileManager::cancelFileQueueTask,   m_tasks_queue, &FileQueue::cancel);
        QObject::connect(this, &FileManager::pauseFileQueueTask,    m_tasks_queue, &FileQueue::pause);
        QObject::connect(this, &FileManager::continueFileQueueTask, m_tasks_queue, &FileQueue::continue_);
        QObject::connect(m_tasks_queue, &FileQueue::blockUntilTaskHasFinish, this, &FileManager::startWaitingLoop);
        QObject::connect(m_tasks_queue, &FileQueue::blockingTaskHasFinished, this, &FileManager::exitWaitingLoop);
    }else
        qDebug() << "trying to connect m_tasks_queue -> nullptr! [root_path: "  << QString::fromStdString(m_root_path) << "]";

    if(m_viewer) // if, da m_viewer zu test-zwecken noch auf nullptr gesetzt ist. im eigentlichen programm dann eig nicht mehr notwendig darauf zu testen im konstruktor!
    {
        QObject::connect(m_viewer, &GraphicsView::killCurrentBlockingAction, this, &FileManager::cancelFileQueueTask);
//        QObject::connect(m_viewer, &GraphicsView::pauseCurrentBlockingAction,  this, &FileManager::pauseFileQueueTask);
        QObject::connect(this, &FileManager::startWaitingLoop, m_viewer, &GraphicsView::startWaitingAnimation);
        QObject::connect(this, &FileManager::exitWaitingLoop,  m_viewer, &GraphicsView::killWaitingAnimation);
    }else
        qDebug() << "trying to connect m_viewer -> nullptr! [root_path: "  << QString::fromStdString(m_root_path) << "]";
}

void FileManager::disconnectSiganls()
{
    if(m_searcher)
    {
        QObject::disconnect(this, &FileManager::search, m_searcher, &FileSearcher::search);
    }

    if(m_tasks_queue)
    {
        QObject::disconnect(this, &FileManager::addQueueTask,          m_tasks_queue, &FileQueue::addTask);
        QObject::disconnect(this, &FileManager::cancelFileQueueTask,   m_tasks_queue, &FileQueue::cancel);
        QObject::disconnect(this, &FileManager::pauseFileQueueTask,    m_tasks_queue, &FileQueue::pause);
        QObject::disconnect(this, &FileManager::continueFileQueueTask, m_tasks_queue, &FileQueue::continue_);
        QObject::disconnect(m_tasks_queue, &FileQueue::blockUntilTaskHasFinish, this, &FileManager::startWaitingLoop);
        QObject::disconnect(m_tasks_queue, &FileQueue::blockingTaskHasFinished, this, &FileManager::exitWaitingLoop);
    }

    if(m_viewer) // if, da m_viewer zu test-zwecken noch auf nullptr gesetzt ist. im eigentlichen programm dann eig nicht mehr notwendig darauf zu testen im konstruktor!
    {
        QObject::disconnect(m_viewer, &GraphicsView::killCurrentBlockingAction, this, &FileManager::cancelFileQueueTask);
//        QObject::disconnect(m_viewer, &GraphicsView::pauseCurrentBlockingAction,  this, &FileManager::pauseFileQueueTask);
        QObject::disconnect(this, &FileManager::startWaitingLoop, m_viewer, &GraphicsView::startWaitingAnimation);
        QObject::disconnect(this, &FileManager::exitWaitingLoop,  m_viewer, &GraphicsView::killWaitingAnimation);
    }
}

void FileManager::revalidateEntries()
{
    int* order = new int(0);
    int* order_clpsd = new int(0);
    int* maxDepthId = new int(0);

    clearEntryContainers();


    if(!m_root_folder || !QFileInfo(QString::fromStdString(m_root_path)).exists())
    {
        evalFirstExistingRootFolder();
    }
    if(!m_root_folder)
        return;

    m_root_folder->iterate(
             [=](FileInfoBD* fiBD,
                 std::string path,
                 std::string fileName,
                 bool isFolder,
                 bool isCollapsed,
                 bool isHidden,
                 FileInfoBD* firstNonCollapsedFold,
                 int depthId)
             {
                if(*maxDepthId < depthId)
                    *maxDepthId = depthId;

                connect(fiBD, &FileInfoBD::requestClosing, this, &FileManager::closeFileInfoBD);

                if( (isHidden && m_showHiddenFiles) ||
                        (!isHidden) )
                {
                     if(isFolder)
                     {
                         m_folders[path] = fiBD;
                         emit addPathToFileWatcher(path);
                     }

                     m_paths.emplace(path);
                     m_folder_paths.emplace(path);
                     m_entry_to_firstElapsedFolder[path] = firstNonCollapsedFold->absPath();
                     m_entries.emplace(std::make_pair(path, FiBD(fiBD, isFolder)));
                     m_entries_order[path] = *order;
                     m_order_entries[*order] = path;
                     m_fileNames[path] = fileName;
                     ++(*order);

                     if(!isCollapsed)
                     {
                         if(isFolder)
                         {
                             m_folders_colpsd[path] = fiBD;
                             if(m_depth_folders_colpsd.find(depthId) == m_depth_folders_colpsd.end())
                             {
                                    m_depth_folders_colpsd[depthId] = std::unordered_set<FileInfoBD*>();
                             }
                             m_depth_folders_colpsd[depthId].insert(fiBD);
                         }
                         m_paths_colpsd.emplace(path);
                         m_folder_paths_colpsd.emplace(path);
                         m_entries_colpsd.emplace(std::make_pair(path, FiBD(fiBD, isFolder)));
                         m_entries_order_colpsd[path] = *order;
                         m_order_entries_colpsd[*order_clpsd] = path;
                         m_fileNames_colpsd[path] = fileName;
                         ++(*order_clpsd);
                     }
                }
             }
    );

    while(m_depthId_elapsed.size() < static_cast<unsigned long>((*maxDepthId) + 1))
        m_depthId_elapsed.push_back(false);

    if(m_selector)
    {
        m_selector->entriesChanged(&m_paths_colpsd, &m_order_entries_colpsd, &m_entries_order_colpsd, &m_fileNames_colpsd, &m_folder_paths_colpsd);
    }
    if(m_searcher)
    {
        m_searcher->entriesChanged(&m_fileNames_colpsd);
    }

    delete order;
    delete order_clpsd;
    delete maxDepthId;
}

void FileManager::revalidateViewer_helper()
{
    std::vector<FiBDViewer> entries;
    for(int i=m_frstDispFile; i <= m_lastDispFile; ++i)
    {
        FiBDViewer fiView;
        entries.push_back(fiView);
    }
    emit revalidateViewer(entries);
}

void FileManager::clearEntryContainers()
{
    m_folders.clear();
    m_folders_colpsd.clear();
    m_entry_to_firstElapsedFolder.clear();
    m_paths.clear();
    m_paths_colpsd.clear();
    m_folder_paths.clear();
    m_folder_paths_colpsd.clear();
    m_entries_order.clear();
    m_order_entries.clear();
    m_entries_order_colpsd.clear();
    m_order_entries_colpsd.clear();
    m_entries.clear();
    m_entries_colpsd.clear();
    m_fileNames.clear();
    m_fileNames_colpsd.clear();
    m_depth_folders_colpsd.clear();
}

void FileManager::removeFiBDFromContainers(string path)
{
    if(m_folders.find(path) != m_folders.end())
        return;

    FileInfoBD* fi = m_folders[path];

    auto it1 = m_folders.find(path);
    if(it1 != m_folders.end())
        m_folders.erase(it1);

    auto it2 = m_folders_colpsd.find(path);
    if(it2 != m_folders_colpsd.end())
        m_folders_colpsd.erase(it2);

    for(auto it: m_depth_folders_colpsd)
    {
        auto& set = it.second;
        auto it3 = set.find(fi);
        if(it3 != set.end())
            set.erase(it3);
    }

    auto it4 = m_entries.find(path);
    if(it4 != m_entries.end())
        m_entries.erase(it4);

    auto it5 = m_entries_colpsd.find(path);
    if(it5 != m_entries_colpsd.end())
        m_entries_colpsd.erase(it5);
}

void FileManager::evalFirstExistingRootFolder()
{
    QDir dir(QString::fromStdString(m_root_path));
    while(!dir.exists())
    {
        dir.cdUp();
    }
    if(dir.exists()){
        m_root_path = dir.absolutePath().toStdString();
        m_root_folder = new FileInfoBD(m_root_path, nullptr, m_showHiddenFiles);
    }else{
        qCritical() << "[FileManger::evalFirstExistingRootFolder(): '" << dir.absolutePath() << "' does not have an existing parent folder -> iterating dir-tree up failed!";
    }
}

void FileManager::copyCutToClipboard_hlpr(bool deleteSourceAfterCopying)
{
    if(m_selector)
    {
        QClipboard* clipboard = QApplication::clipboard();
        QList<QUrl> urls;

        if(deleteSourceAfterCopying){
            urls.append(QUrl("cut"));
        }

        const std::unordered_set<std::string>& slctd_entries = m_selector->getSelectedEntries();

        for(const auto& path: slctd_entries) {
            if( !path.empty() ){
                urls.append( QUrl::fromLocalFile(QString::fromStdString(path)) );
            }
        }
        if( urls.size() > 0 ){
            QMimeData *mimeData = new QMimeData;
            mimeData->setUrls(urls);
            clipboard->setMimeData(mimeData);
        }
        clipboard->deleteLater();
    }
}

std::pair<std::vector<std::string>, bool> FileManager::extractPathsFromPathsString(QString paths_str)
{
    bool cut = false;
    std::vector<std::string> paths;
    QTextStream stream(&paths_str);
    QString line;
    QVector<QString> pathsFromClpbrd;
    while (stream.readLineInto(&line)) {
        if(line == QString("cut"))
        {
            cut = true;
        }else{
            if( !line.isEmpty() )
                paths.push_back( line.toStdString() );
        }
    }
    return std::pair<std::vector<std::string>, bool>(paths, cut);
}

std::pair<std::vector<std::string>, bool> FileManager::extractPathsFromUrls(const QList<QUrl> &urls)
{
    bool cut = false;
    std::vector<std::string> paths;
    for(const auto& url: urls)
    {
        if(url.toString() == QString("cut")){
            cut = true;
        }else{
            QString pth = url.toLocalFile();
            if( !pth.isEmpty() ){
                paths.push_back( url.toLocalFile().toStdString() );
            }
        }
    }
    return std::pair<std::vector<std::string>, bool>(paths, cut);
}

void FileManager::copyFiles(std::vector<string> pathsToCopy, string targetPath, bool cut)
{
    if(pathsToCopy.size() > 0 && !targetPath.empty()){
        if(cut)
        {
            COPY_MOVE::MoveFiles* copyWorker = new COPY_MOVE::MoveFiles(pathsToCopy, targetPath);
            emit addQueueTask(copyWorker);
        }else{
            COPY_MOVE::CopyFiles* moveWorker = new COPY_MOVE::CopyFiles(pathsToCopy, targetPath);
            emit addQueueTask(moveWorker);
        }
    }
}

string FileManager::getBaseDirOfSelection() const
{
    std::string baseDir = m_root_path;
    if(m_selector){
        std::string lastSel = m_selector->getLastSelectedEntry();
        if(QFileInfo(QString::fromStdString(lastSel)).isDir())
            baseDir = lastSel;
        else
            baseDir = PATH::getBasePath(lastSel).toStdString();
    }
    return baseDir;
}

std::chrono::milliseconds FileManager::getCurrentTime() const
{
    return std::chrono::duration_cast< std::chrono::milliseconds >(
            std::chrono::system_clock::now().time_since_epoch()
                );
}

void FileManager::focusPath(const string &absPath)
{
    if(m_entries_order_colpsd.find(absPath) != m_entries_order_colpsd.end())
    {
        int order = m_entries_order_colpsd[absPath];
        emit focusGraphicsView(order);
    }
}

