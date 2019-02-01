#include "filemanager.h"

FileManager::FileManager(std::string root_path,
                         bool showHiddenFiles)
    : QObject(nullptr),

      m_root_path(root_path),
      m_dir_manager(new DirManager(root_path)),
      m_tree(m_dir_manager->getTree()),

      m_searcher(new FileSearcher(this)),
      m_selector(new FileSelector(this)),

      m_tasks_queue(new FileQueue()),

      m_viewer(new GraphicsView()),

      m_depthId_elapsed(std::vector<bool>()),
      m_frstDispFile(0),
      m_lastDispFile(0),

      m_showHiddenFiles(showHiddenFiles),
      m_inSearchMode(false),

      m_lastKeyPressed(0),
      m_keysPressed(""),

      m_zoomFactor(0)
{
    connectSignals();

    setRoot(root_path);
}

FileManager::~FileManager()
{
    disconnectSiganls();

    delete m_searcher; m_searcher = nullptr;
    delete m_selector; m_selector = nullptr;

    m_tasks_queue->deleteLater(); m_tasks_queue = nullptr;

    m_viewer->deleteLater(); m_viewer = nullptr;

    if(m_tree)
        delete m_tree;
}

void FileManager::test()
{
    selectionChanged();
}

QLayout *FileManager::genPane()
{

}

void FileManager::setRoot(const std::string& rootPath)
{
    m_root_path = rootPath;

    emit rootDirChanged_dm(rootPath);
}

void FileManager::close()
{
    if(m_dir_manager)
        m_dir_manager->close();
    m_dir_manager = nullptr;

    if(m_searcher)
        m_searcher->close();
    m_searcher = nullptr;

    if(m_selector)
        m_selector->close();
    m_selector = nullptr;

    if(m_viewer)
        m_viewer->deleteLater();
    m_viewer = nullptr;

    clearEntryContainers();

    if(m_tree)
        delete m_tree;
    m_tree = nullptr;
}

void FileManager::selectionChanged()
{
    revalidateViewer_helper();
}

void FileManager::searchResultsChanged()
{
    revalidateViewer_helper();
}

void FileManager::elapseFolder(std::string path)
{
    emit elapse_dm(path);
}
void FileManager::elapseFolder_rec(std::string path)
{
    emit elapseRec_dm(path);
}
void FileManager::collapseFolder(std::string path)
{
    emit collapse_dm(path);
}
void FileManager::collapseFolder_rec(std::string path)
{
    emit collapseRec_dm(path);
}

//-------------------------------------------------------------

void FileManager::elapseAllFoldersOfDepthId(int id)
{
    if(m_depth_folders_colpsd.find(id) != m_depth_folders_colpsd.end() &&
            m_tasks_queue)
    {
        m_depthId_elapsed[static_cast<unsigned long>(id)] = !m_depthId_elapsed[static_cast<unsigned long>(id)];
        bool collapse = !m_depthId_elapsed[static_cast<unsigned long>(id)];

        auto& fis = m_depth_folders_colpsd[id];
        if(collapse)
        {
            emit collapse_dm(std::vector<std::string>(fis.begin(), fis.end()));
        }else{
            emit elapse_dm(std::vector<std::string>(fis.begin(), fis.end()));
        }
    }
}
void FileManager::elapseSelectedFoldersRecursively()
{
    if(m_selector && m_tasks_queue)
    {
        const auto& folder_paths = m_selector->getSelectedFolders();
        emit elapseRec_dm(std::vector<std::string>(folder_paths.begin(), folder_paths.end()));
    }
}
void FileManager::elapseSelectedFolders()
{
    if(m_selector && m_tasks_queue)
    {
        const auto& folder_paths = m_selector->getSelectedFolders();
        emit elapse_dm(std::vector<std::string>(folder_paths.begin(), folder_paths.end()));
    }
}
void FileManager::collapseSelectedFoldersRecursively()
{
    if(m_selector && m_tasks_queue)
    {
        const auto& folder_paths = m_selector->getSelectedFolders();
        emit collapseRec_dm(std::vector<std::string>(folder_paths.begin(), folder_paths.end()));

    }
}
void FileManager::collapseSelectedFolders()
{
    if(m_selector && m_tasks_queue)
    {
        const auto& folder_paths = m_selector->getSelectedFolders();
        emit collapse_dm(std::vector<std::string>(folder_paths.begin(), folder_paths.end()));
    }
}
void FileManager::elapseOrCollapseFolderDependingOnCurrentState(QString q_path)
{
    std::string path = q_path.toStdString();
    if(m_folders.find(path) != m_folders.end())
    {
        auto* folder = m_folders[path];
        bool collapse = folder->isElapsed;
        if(collapse)
        {
            emit collapse_dm(path);
        }else{
            emit elapse_dm(path);
        }
    }
}

//-------------------------------------------------------------

void FileManager::setIncludeHiddenFiles(bool inclHdnFls)
{
    m_showHiddenFiles = inclHdnFls;
    if(m_showHiddenFiles)
    {
        emit includeHiddenFiles_dm();
    }else {
        emit excludeHiddenFiles_dm();
    }
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
        m_selector->selectEntireContent();
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

void FileManager::sortDir(QString dir, Order order)
{
    emit sortDir_dm(order, dir.toStdString());
}

void FileManager::sortDirs(std::vector<QString> dirs, Order order)
{
    std::vector<std::string> std_dirs;
    for(const auto& dir: dirs)
    {
        std_dirs.push_back(dir.toStdString());
    }
    emit sortDirs_dm(order, std_dirs);
}

void FileManager::sortAllFolders(Order order)
{
    emit sortAllDirs_dm(order);
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

void FileManager::dirChanged_dm(DirManagerInfo* changedDir)
{
    m_tree->replaceContents(changedDir);
}

void FileManager::treeChanged_dm(DirManagerInfo* entireTree)
{
    replaceTree(entireTree);
}

void FileManager::deepSearchFinished_dm(std::vector<string> matchingPaths, string keyword)
{
    if(m_searcher)
    {
        m_searcher->setSearched(keyword, matchingPaths);
    }
}

//-------------------------------------------------------------

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

    if(m_dir_manager)
    {
        connect(m_dir_manager, &DirManager::dirChanged, this, &FileManager::dirChanged_dm, Qt::QueuedConnection);
        connect(m_dir_manager, &DirManager::treeChanged, this, &FileManager::treeChanged_dm, Qt::QueuedConnection);
        connect(m_dir_manager, &DirManager::deepSearchFinished, this, &FileManager::deepSearchFinished_dm, Qt::QueuedConnection);

        connect(this, SIGNAL(elapse_dm(std::string)), m_dir_manager, SLOT(elapse(std::string)), Qt::QueuedConnection);
        connect(this, SIGNAL(elapse_dm(std::vector<std::string>)), m_dir_manager, SLOT(elapse(std::vector<std::string>)), Qt::QueuedConnection);

        connect(this, SIGNAL(elapseRec_dm(std::string)), m_dir_manager, SLOT(elapseRec(std::string)), Qt::QueuedConnection);
        connect(this, SIGNAL(elapseRec_dm(std::vector<std::string>)), m_dir_manager, SLOT(elapseRec(std::vector<std::string>)), Qt::QueuedConnection);

        connect(this, SIGNAL(collapse_dm(std::string)), m_dir_manager, SLOT(collapse(std::string)), Qt::QueuedConnection);
        connect(this, SIGNAL(collapse_dm(std::vector<std::string>)), m_dir_manager, SLOT(collapse(std::vector<std::string>)), Qt::QueuedConnection);

        connect(this, SIGNAL(collapseRec_dm(std::string)), m_dir_manager, SLOT(collapseRec(std::string)), Qt::QueuedConnection);
        connect(this, SIGNAL(collapseRec_dm(std::vector<std::string>)), m_dir_manager, SLOT(collapseRec(std::vector<std::string>)), Qt::QueuedConnection);

        connect(this, SIGNAL(deleteDir_dm(std::string)), m_dir_manager, SLOT(deleteDir(std::string)), Qt::QueuedConnection);
        connect(this, SIGNAL(deleteDirs_dm(std::vector<std::string>)), m_dir_manager, SLOT(deleteDirs(std::vector<std::string>)), Qt::QueuedConnection);

        connect(this, &FileManager::includeHiddenFiles_dm, m_dir_manager, &DirManager::includeHiddenFiles, Qt::QueuedConnection);
        connect(this, &FileManager::excludeHiddenFiles_dm, m_dir_manager, &DirManager::excludeHiddenFiles, Qt::QueuedConnection);

        connect(this, &FileManager::sortDir_dm, m_dir_manager, &DirManager::sortDir, Qt::QueuedConnection);
        connect(this, &FileManager::sortDirs_dm, m_dir_manager, &DirManager::sortDirs, Qt::QueuedConnection);
        connect(this, &FileManager::sortAllDirs_dm, m_dir_manager, &DirManager::sortAllDirs, Qt::QueuedConnection);

        connect(this, &FileManager::cdUP_dm, m_dir_manager, &DirManager::cdUP, Qt::QueuedConnection);

        connect(this, &FileManager::deepSearch_dm, m_dir_manager, &DirManager::deepSearch, Qt::QueuedConnection);

        connect(this, &FileManager::rootDirChanged_dm, m_dir_manager, &DirManager::rootDirChanged, Qt::QueuedConnection);
    }else
        qDebug() << "trying to connect m_dir_manager -> nullptr! [root_path: "  << QString::fromStdString(m_root_path) << "]";
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


    if(m_dir_manager)
    {
        hier die singal/slots DISconnecten!;
    }
}

//void FileManager::revalidateEntries()
//{
//    int* order = new int(0);
//    int* order_clpsd = new int(0);
//    int* maxDepthId = new int(0);

//    clearEntryContainers();


//    if(!m_root_folder || !QFileInfo(QString::fromStdString(m_root_path)).exists())
//    {
//        evalFirstExistingRootFolder();
//    }
//    if(!m_root_folder)
//        return;

//    m_root_folder->iterate(
//             [=](FileInfoBD* fiBD,
//                 std::string path,
//                 std::string fileName,
//                 bool isFolder,
//                 bool isCollapsed,
//                 bool isHidden,
//                 FileInfoBD* firstNonCollapsedFold,
//                 int depthId)
//             {
//                if(*maxDepthId < depthId)
//                    *maxDepthId = depthId;

//                connect(fiBD, &FileInfoBD::requestClosing, this, &FileManager::closeFileInfoBD);

//                if( (isHidden && m_showHiddenFiles) ||
//                        (!isHidden) )
//                {
//                     if(isFolder)
//                     {
//                         m_folders[path] = fiBD;
//                         emit addPathToFileWatcher(path);
//                     }

//                     m_paths.emplace(path);
//                     m_folder_paths.emplace(path);
//                     m_entry_to_firstElapsedFolder[path] = firstNonCollapsedFold->absPath();
//                     m_entries.emplace(std::make_pair(path, FiBD(fiBD, isFolder)));
//                     m_entries_order[path] = *order;
//                     m_order_entries[*order] = path;
//                     m_fileNames[path] = fileName;
//                     ++(*order);

//                     if(!isCollapsed)
//                     {
//                         if(isFolder)
//                         {
//                             m_folders_colpsd[path] = fiBD;
//                             if(m_depth_folders_colpsd.find(depthId) == m_depth_folders_colpsd.end())
//                             {
//                                    m_depth_folders_colpsd[depthId] = std::unordered_set<FileInfoBD*>();
//                             }
//                             m_depth_folders_colpsd[depthId].insert(fiBD);
//                         }
//                         m_paths_colpsd.emplace(path);
//                         m_folder_paths_colpsd.emplace(path);
//                         m_entries_colpsd.emplace(std::make_pair(path, FiBD(fiBD, isFolder)));
//                         m_entries_order_colpsd[path] = *order;
//                         m_order_entries_colpsd[*order_clpsd] = path;
//                         m_fileNames_colpsd[path] = fileName;
//                         ++(*order_clpsd);
//                     }
//                }
//             }
//    );

//    while(m_depthId_elapsed.size() < static_cast<unsigned long>((*maxDepthId) + 1))
//        m_depthId_elapsed.push_back(false);

//    if(m_selector)
//    {
//        m_selector->entriesChanged(&m_paths_colpsd, &m_order_entries_colpsd, &m_entries_order_colpsd, &m_fileNames_colpsd, &m_folder_paths_colpsd);
//    }
//    if(m_searcher)
//    {
//        m_searcher->entriesChanged(&m_fileNames_colpsd, &m_entries_order_colpsd);
//    }

//    delete order;
//    delete order_clpsd;
//    delete maxDepthId;
//}

void FileManager::revalidateViewer_helper()
{
    std::vector<FiBDViewer> entries;
    for(long long i=m_frstDispFile; i <= m_lastDispFile; ++i)
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
//    m_entries.clear();
//    m_entries_colpsd.clear();
    m_fileNames.clear();
    m_fileNames_colpsd.clear();
    m_depth_folders_colpsd.clear();
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
        unsigned long long order = m_entries_order_colpsd[absPath];
        emit focusGraphicsView(order);
    }
}

void FileManager::replaceTree(DirManagerInfo* tree)
{
    if(m_tree)
        delete m_tree;

    m_tree = tree;

    clearEntryContainers();

    unsigned long long* cntr = new unsigned long long(0);
    unsigned long long* cntr_colpsd= new unsigned long long(0);
    int* maxDepthId = new int(0);

    replaceTree_hlpr(tree, tree, false, cntr, cntr_colpsd, 0, maxDepthId);

    while(m_depthId_elapsed.size() < static_cast<unsigned long>((*maxDepthId) + 1))
        m_depthId_elapsed.push_back(false);

    if(m_selector)
    {
        m_selector->entriesChanged(&m_paths_colpsd, &m_order_entries_colpsd, &m_entries_order_colpsd, &m_fileNames_colpsd, &m_folder_paths_colpsd);
    }
    if(m_searcher)
    {
        m_searcher->entriesChanged(&m_fileNames_colpsd, &m_entries_order_colpsd);
    }

    delete cntr;
    delete cntr_colpsd;
    delete maxDepthId;
}

void FileManager::replaceTree_hlpr(DirManagerInfo* entry,
                                   DirManagerInfo* firstNonCollapsedFold,
                                   bool isCollapsed,
                                   unsigned long long *cntr,
                                   unsigned long long *cntr_clpsd,
                                   int depthId,
                                   int* maxDepthId)
{
    if(*maxDepthId < depthId)
        *maxDepthId = depthId;

    if( (entry->isHidden && m_showHiddenFiles) || (!entry->isHidden) )
    {
        std::string path = entry->absPath;

//        FileData* fd = new FileData(entry);

        m_folders[path] = entry;

        m_paths.emplace(path);
        m_folder_paths.emplace(path);
        m_entry_to_firstElapsedFolder[path] = firstNonCollapsedFold->absPath;

//        m_entries.emplace(std::make_pair(path, fd));
        m_entries_order[path] = *cntr;
        m_order_entries[*cntr] = path;

        m_fileNames[path] = entry->fileName;

        if(!isCollapsed)
        {
            m_folders_colpsd[path] = entry;
            if(m_depth_folders_colpsd.find(depthId) == m_depth_folders_colpsd.end())
            {
                m_depth_folders_colpsd[depthId] = std::unordered_set<std::string>();
            }
            m_depth_folders_colpsd[depthId].insert(path);

            m_paths_colpsd.emplace(path);
            m_folder_paths_colpsd.emplace(path);
//            m_entries_colpsd.emplace(std::make_pair(path, fd));
            m_entries_order_colpsd[path] = *cntr;
            m_order_entries_colpsd[*cntr_clpsd] = path;
            m_fileNames_colpsd[path] = entry->fileName;

            ++(*cntr_clpsd);
        }

        ++(*cntr);


        bool subs_collapsed = isCollapsed || !entry->isElapsed;

        for(auto* sub_dir: entry->subDirs_sorted)
        {
            DirManagerInfo* fncf = isCollapsed ? firstNonCollapsedFold : sub_dir;

            replaceTree_hlpr(sub_dir, fncf, subs_collapsed, cntr, cntr_clpsd, depthId, maxDepthId);
        }

        for(const auto& file_path: entry->files_sorted)
        {
            std::string filePath = file_path;

//            m_entries[filePath] = file;
            m_entries_order[filePath] = *cntr;
            m_order_entries[*cntr] = filePath;

            if(subs_collapsed)
            {
//                m_entries_colpsd[filePath] = file;
                m_entries_order_colpsd[filePath] = *cntr_clpsd;
                m_order_entries_colpsd[*cntr_clpsd] = filePath;

                ++(*cntr_clpsd);
            }

            ++(*cntr);
        }
    }
}

