#include "filemanager.h"
#include "threadtestworker.h"

FileManager::FileManager(std::string root_path,
                         bool showHiddenFiles)
    : QObject(nullptr),
      WidgetCreator(),

      m_root_path(root_path),
      m_dir_manager(new DirManager(root_path)),
      m_tree(m_dir_manager->getTree()),

      m_tasks_queue(new FileQueue()),

      m_depthId_elapsed(std::vector<bool>()),

      m_frstDispFile(0),
      m_lastDispFile(0),

      m_showHiddenFiles(showHiddenFiles),

      m_lastKeyPressed(0),
      m_keysPressed(""),

      m_zoomFactor(INIT_ZOOM_FACTOR),

      m_graphicsViewVBarValueBackup(0),
      m_graphicsViewHBarValueBackup(0),

      m_dirStack(new DirectoryStack()),

      m_searcher(new FileSearcher(&m_path_fileNames_colpsd,
                                  &m_entries_order_colpsd,
                                  this)),
      m_selector(new FileSelector(&m_paths_colpsd,
                                  &m_order_entries_colpsd,
                                  &m_entries_order_colpsd,
                                  &m_path_fileNames_colpsd,
                                  &m_folder_paths_colpsd,
                                  this))
{
    connectSignals();

    setRoot(root_path);
}

FileManager::~FileManager()
{
    qDebug() << "~FileManager";
    disconnectSiganls();

    delete m_searcher; m_searcher = nullptr;
    delete m_selector; m_selector = nullptr;

    m_tasks_queue->deleteLater(); m_tasks_queue = nullptr;

    if(m_tree)
        delete m_tree;

    delete m_dirStack; m_dirStack = nullptr;
}

void FileManager::test()
{
    selectionChanged();
}

QLayout* FileManager::genPane()
{
    QDir rootDir(QString::fromStdString(m_root_path));

    GraphicsView* folderViewer = new GraphicsView(new FileManagerInfo(*this),
                                                  generateViewerData(),
                                                  m_graphicsViewVBarValueBackup,
                                                  m_graphicsViewHBarValueBackup,
                                                  m_zoomFactor);

    QGridLayout* mainGrid = new QGridLayout();
    DirectorySelectionPane* toolBar = new DirectorySelectionPane(rootDir);

    connectViewer(folderViewer);
    connectDirectorySelectionPane(toolBar);

    mainGrid->addWidget(toolBar, 0,0);

    mainGrid->addWidget(folderViewer, 1, 0);
    mainGrid->setContentsMargins(0, 4, 0, 4);
    mainGrid->setSpacing(0);

    return mainGrid;
}

QWidget* FileManager::createWidget()
{
    QWidget* widget = new QWidget();
    widget->setLayout(genPane());
    return widget;
}

void FileManager::setRoot_QDir(QDir dir)
{
    setRoot(dir.absolutePath().toStdString());
}

void FileManager::saveGraphicsViewVBarValue(int value)
{
    m_graphicsViewHBarValueBackup = value;
}

void FileManager::saveGraphicsViewHBarValue(int value)
{
    m_graphicsViewVBarValueBackup = value;
}

void FileManager::setZoomFactor(int zoomFactor)
{
    m_zoomFactor = zoomFactor;
}

void FileManager::setRoot(const std::string& rootPath)
{
    setRoot_hlpr(rootPath);
}

void FileManager::close()
{
    clearEntryContainers();

    if(m_dir_manager)
        m_dir_manager->close();
    m_dir_manager = nullptr;

    if(m_searcher)
        m_searcher->close();
    m_searcher = nullptr;

    if(m_selector)
        m_selector->close();
    m_selector = nullptr;

    if(m_tree)
        delete m_tree;
    m_tree = nullptr;

    this->deleteLater();
}

void FileManager::selectionChanged()
{
    revalidateViewer_EntireData_hlpr();
//    revalidateViewer_metaData_hlpr();
}

void FileManager::searchResultsChanged()
{
    revalidateViewer_EntireData_hlpr();
//    revalidateViewer_metaData_hlpr();
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

void FileManager::setLastPathToRoot()
{
    if(m_dirStack->size() > 0)
    {
        std::string path = m_dirStack->pop_back();
        setRoot_hlpr(path, false);
    }
}

//-------------------------------------------------------------

void FileManager::elapseAllFoldersOfDepthId(int id)
{
    if(m_depth_folders_colpsd.find(id) != m_depth_folders_colpsd.end() &&
            m_tasks_queue)
    {
        bool collapse = m_depthId_elapsed[static_cast<std::size_t>(id)];
        m_depthId_elapsed[static_cast<std::size_t>(id)] = !collapse;

        auto& fis = m_depth_folders_colpsd[id];
        if(collapse)
        {
            emit collapse_dm(std::vector<std::string>(fis.begin(), fis.end()));
        }else{
            emit elapse_dm(std::vector<std::string>(fis.begin(), fis.end()));
        }
    }else{
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
        emit includeHiddenFiles_SGNL();
    }else {
        emit excludeHiddenFiles_SGNL();
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
        emit openFoldersInTab_SGNL(folders);
    }
}

//void FileManager::searchForKeyWord(QString keyword, bool deepsearch)
//{
//    Q_UNUSED(deepsearch);
//    if(m_searcher)
//    {
//        m_searcher->search(keyword.toStdString());
//        m_inSearchMode = true;
//    }
//}
//void FileManager::selectEntireContent()
//{
//    if(m_selector)
//    {
//        m_selector->selectEntireContent();
//        revalidateViewer_helper();
//    }
//}
//void FileManager::clearSelection()
//{
//    if(m_selector)
//    {
//        m_selector->clearSelection();
//        revalidateViewer_helper();
//    }
//}
//void FileManager::selectButtonUp(bool cntrlPrsd, bool shiftPrsd)
//{
//    if(m_selector)
//    {
//        m_selector->selectPrevious(cntrlPrsd, shiftPrsd);
//        revalidateViewer_helper();
//    }
//}
//void FileManager::selectButtonDown(bool cntrlPrsd, bool shiftPrsd)
//{
//    if(m_selector)
//    {
//        m_selector->selectNext(cntrlPrsd, shiftPrsd);
//        revalidateViewer_helper();
//    }
//}
//void FileManager::selectEntry(QString entry, bool contrlPrsd, bool shiftPrsd)
//{
//    if(m_selector)
//    {
//        m_selector->select(entry.toStdString(), contrlPrsd, shiftPrsd);
//        revalidateViewer_helper();
//    }
//}

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
        if(entries_vec.size() == 1 && QFileInfo(QString::fromStdString(entries_vec[0])).isDir())
        {
            setRoot(entries_vec[0]);
        }else{
            OpenFiles* openFilesWorker = new OpenFiles(entries_vec);
            emit addQueueTask(openFilesWorker);
        }
    }
}
void FileManager::openSelectedContentWith()
{
    qDebug() << "FileManager::openSelectedContentWith";
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

void FileManager::showDetailsOfSelectedContent()
{
    qDebug() << "FileManager::showDetailsOfSelectedContent not implemented so far";
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
            for(std::size_t i=1; i < entries_vec.size(); ++i)
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
    emit cancelFileQueueTask();
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

void FileManager::keyPressed(std::string s)
{
    auto currentTime = getCurrentTime();
    auto timePassed = std::chrono::duration_cast<std::chrono::milliseconds>(currentTime - m_lastKeyPressed).count();
    m_lastKeyPressed = currentTime;

    if(timePassed > 300)
    {
        m_keysPressed.clear();
    }

    m_keysPressed.append(s);
    m_keysPressed = StringOps::toLower(m_keysPressed);

    if(m_selector)
    {
        std::string old_last_sel = m_selector->getLastSelectedEntry();

        m_selector->selectKeyWord(m_keysPressed);
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
        qDebug() << "openTerminal: sel_dir: " << sel_dir;
        STATIC_FUNCTIONS::openTerminal(sel_dir);
    }
}

void FileManager::zoomFactorChanged(int newZoomFactor)
{
    m_zoomFactor = newZoomFactor;
}

void FileManager::requestFileViewerRevalidation(int_bd firstEntryToDispl, int_bd lastEntryToDisp)
{
    m_frstDispFile = firstEntryToDispl;
    m_lastDispFile = lastEntryToDisp;

    revalidateViewer_entries_hlpr();
}

void FileManager::deepSearch(QString key_word)
{
    emit deepSearch_dm(key_word.toStdString(), true);
}

void FileManager::dirChanged_dm(DirManagerInfo* changedDir)
{
    m_tree->replaceContents(changedDir);
    revalidateTree();
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

void FileManager::setRoot_hlpr(string rootPath, bool addToDirStack)
{
    m_root_path = rootPath;

    if(addToDirStack)
        m_dirStack->addPath(rootPath);

    m_selector->clearSelectionWithoutSignaling();
    m_searcher->clearSearchWithoutSignaling();

    emit rootDirChanged(rootPath);
}

//-------------------------------------------------------------

void FileManager::connectSignals()
{
    if(m_tasks_queue)
    {
        connect(this, &FileManager::addQueueTask,          m_tasks_queue, &FileQueue::addTask);
        connect(this, &FileManager::cancelFileQueueTask,   m_tasks_queue, &FileQueue::cancel);
        connect(this, &FileManager::pauseFileQueueTask,    m_tasks_queue, &FileQueue::pause);
        connect(this, &FileManager::continueFileQueueTask, m_tasks_queue, &FileQueue::continue_);
        connect(m_tasks_queue, &FileQueue::blockUntilTaskHasFinish, this, &FileManager::startWaitingLoop);
        connect(m_tasks_queue, &FileQueue::blockingTaskHasFinished, this, &FileManager::exitWaitingLoop);
    }else
        qDebug() << "trying to connect m_tasks_queue -> nullptr! [root_path: "  << QString::fromStdString(m_root_path) << "]";

    if(m_selector)
    {
        connect(m_selector, &FileSelector::selectionChanged, this, &FileManager::selectionChanged);
        connect(m_selector, &FileSelector::focusPath, this, &FileManager::focusPath); // wenn keyPressed -> select_KeyWord -> beim match -> focusMatch
    }

    if(m_searcher)
    {
        connect(m_searcher, &FileSearcher::searchResultsChanged, this, &FileManager::searchResultsChanged);
    }

    if(m_dir_manager)
    {
        connect(m_dir_manager, &DirManager::dirChanged,                 this, &FileManager::dirChanged_dm,                          Qt::QueuedConnection);
        connect(m_dir_manager, &DirManager::treeChanged,                this, &FileManager::treeChanged_dm,                         Qt::QueuedConnection);
        connect(m_dir_manager, &DirManager::deepSearchFinished,         this, &FileManager::deepSearchFinished_dm,                  Qt::QueuedConnection);

        connect(this, SIGNAL(elapse_dm(std::string)),                   m_dir_manager, SLOT(elapse(std::string)),                   Qt::QueuedConnection);
        connect(this, SIGNAL(elapse_dm(std::vector<std::string>)),      m_dir_manager, SLOT(elapse(std::vector<std::string>)),      Qt::QueuedConnection);

        connect(this, SIGNAL(elapseRec_dm(std::string)),                m_dir_manager, SLOT(elapseRec(std::string)),                Qt::QueuedConnection);
        connect(this, SIGNAL(elapseRec_dm(std::vector<std::string>)),   m_dir_manager, SLOT(elapseRec(std::vector<std::string>)),   Qt::QueuedConnection);

        connect(this, SIGNAL(collapse_dm(std::string)),                 m_dir_manager, SLOT(collapse(std::string)),                 Qt::QueuedConnection);
        connect(this, SIGNAL(collapse_dm(std::vector<std::string>)),    m_dir_manager, SLOT(collapse(std::vector<std::string>)),    Qt::QueuedConnection);

        connect(this, SIGNAL(collapseRec_dm(std::string)),              m_dir_manager, SLOT(collapseRec(std::string)),              Qt::QueuedConnection);
        connect(this, SIGNAL(collapseRec_dm(std::vector<std::string>)), m_dir_manager, SLOT(collapseRec(std::vector<std::string>)), Qt::QueuedConnection);

        connect(this, SIGNAL(deleteDir_dm(std::string)),                m_dir_manager, SLOT(deleteDir(std::string)),                Qt::QueuedConnection);
        connect(this, SIGNAL(deleteDirs_dm(std::vector<std::string>)),  m_dir_manager, SLOT(deleteDirs(std::vector<std::string>)),  Qt::QueuedConnection);

        connect(this, &FileManager::includeHiddenFiles_SGNL,            m_dir_manager, &DirManager::includeHiddenFiles,             Qt::QueuedConnection);
        connect(this, &FileManager::excludeHiddenFiles_SGNL,            m_dir_manager, &DirManager::excludeHiddenFiles,             Qt::QueuedConnection);

        connect(this, &FileManager::sortDir_dm,                         m_dir_manager, &DirManager::sortDir,                        Qt::QueuedConnection);
        connect(this, &FileManager::sortDirs_dm,                        m_dir_manager, &DirManager::sortDirs,                       Qt::QueuedConnection);
        connect(this, &FileManager::sortAllDirs_dm,                     m_dir_manager, &DirManager::sortAllDirs,                    Qt::QueuedConnection);

        connect(this, &FileManager::cdUP_dm,                            m_dir_manager, &DirManager::cdUP,                           Qt::QueuedConnection);

        connect(this, &FileManager::deepSearch_dm,                      m_dir_manager, &DirManager::deepSearch,                     Qt::QueuedConnection);

        connect(this, &FileManager::rootDirChanged,                     m_dir_manager, &DirManager::rootDirChanged,                 Qt::QueuedConnection);
    }else
        qDebug() << "trying to connect m_dir_manager -> nullptr! [root_path: "  << QString::fromStdString(m_root_path) << "]";
}

void FileManager::disconnectSiganls()
{
    if(m_tasks_queue)
    {
        disconnect(this, &FileManager::addQueueTask,          m_tasks_queue, &FileQueue::addTask);
        disconnect(this, &FileManager::cancelFileQueueTask,   m_tasks_queue, &FileQueue::cancel);
        disconnect(this, &FileManager::pauseFileQueueTask,    m_tasks_queue, &FileQueue::pause);
        disconnect(this, &FileManager::continueFileQueueTask, m_tasks_queue, &FileQueue::continue_);
        disconnect(m_tasks_queue, &FileQueue::blockUntilTaskHasFinish, this, &FileManager::startWaitingLoop);
        disconnect(m_tasks_queue, &FileQueue::blockingTaskHasFinished, this, &FileManager::exitWaitingLoop);
    }

    if(m_selector)
    {
        disconnect(m_selector, &FileSelector::selectionChanged, this, &FileManager::selectionChanged);
        disconnect(m_selector, &FileSelector::focusPath, this, &FileManager::focusPath); // wenn keyPressed -> select_KeyWord -> beim match -> focusMatch
    }

    if(m_searcher)
    {
        disconnect(m_searcher, &FileSearcher::searchResultsChanged, this, &FileManager::searchResultsChanged);
    }

    if(m_dir_manager)
    {
        disconnect(m_dir_manager, &DirManager::dirChanged, this, &FileManager::dirChanged_dm);
        disconnect(m_dir_manager, &DirManager::treeChanged, this, &FileManager::treeChanged_dm);
        disconnect(m_dir_manager, &DirManager::deepSearchFinished, this, &FileManager::deepSearchFinished_dm);

        disconnect(this, SIGNAL(elapse_dm(std::string)), m_dir_manager, SLOT(elapse(std::string)));
        disconnect(this, SIGNAL(elapse_dm(std::vector<std::string>)), m_dir_manager, SLOT(elapse(std::vector<std::string>)));

        disconnect(this, SIGNAL(elapseRec_dm(std::string)), m_dir_manager, SLOT(elapseRec(std::string)));
        disconnect(this, SIGNAL(elapseRec_dm(std::vector<std::string>)), m_dir_manager, SLOT(elapseRec(std::vector<std::string>)));

        disconnect(this, SIGNAL(collapse_dm(std::string)), m_dir_manager, SLOT(collapse(std::string)));
        disconnect(this, SIGNAL(collapse_dm(std::vector<std::string>)), m_dir_manager, SLOT(collapse(std::vector<std::string>)));

        disconnect(this, SIGNAL(collapseRec_dm(std::string)), m_dir_manager, SLOT(collapseRec(std::string)));
        disconnect(this, SIGNAL(collapseRec_dm(std::vector<std::string>)), m_dir_manager, SLOT(collapseRec(std::vector<std::string>)));

        disconnect(this, SIGNAL(deleteDir_dm(std::string)), m_dir_manager, SLOT(deleteDir(std::string)));
        disconnect(this, SIGNAL(deleteDirs_dm(std::vector<std::string>)), m_dir_manager, SLOT(deleteDirs(std::vector<std::string>)));

        disconnect(this, &FileManager::includeHiddenFiles_SGNL, m_dir_manager, &DirManager::includeHiddenFiles);
        disconnect(this, &FileManager::excludeHiddenFiles_SGNL, m_dir_manager, &DirManager::excludeHiddenFiles);

        disconnect(this, &FileManager::sortDir_dm, m_dir_manager, &DirManager::sortDir);
        disconnect(this, &FileManager::sortDirs_dm, m_dir_manager, &DirManager::sortDirs);
        disconnect(this, &FileManager::sortAllDirs_dm, m_dir_manager, &DirManager::sortAllDirs);

        disconnect(this, &FileManager::cdUP_dm, m_dir_manager, &DirManager::cdUP);

        disconnect(this, &FileManager::deepSearch_dm, m_dir_manager, &DirManager::deepSearch);

        disconnect(this, &FileManager::rootDirChanged, m_dir_manager, &DirManager::rootDirChanged);
    }else
        qDebug() << "trying to DISconnect m_dir_manager -> nullptr! [root_path: "  << QString::fromStdString(m_root_path) << "]";
}

void FileManager::connectViewer(GraphicsView* viewer)
{
    if(viewer) // if, da m_viewer zu test-zwecken noch auf nullptr gesetzt ist. im eigentlichen programm dann eig nicht mehr notwendig darauf zu testen im konstruktor!
    {
        // FileManager -> GraphicsView:
        connect(this, &FileManager::startWaitingLoop,            viewer, &GraphicsView::startWaitingAnimation);
        connect(this, &FileManager::exitWaitingLoop,             viewer, &GraphicsView::killWaitingAnimation);
        connect(this, &FileManager::focusGraphicsView,           viewer, &GraphicsView::focusId);
        connect(this, &FileManager::requestFocusSGNL,            viewer, &GraphicsView::requestFocus);
        connect(this, &FileManager::revalidateViewer_Entries,    viewer, &GraphicsView::receiveFileViewers);
        connect(this, &FileManager::revalidateViewer_MetaData,   viewer, &GraphicsView::receiveFileManagerMetaData);
        connect(this, &FileManager::revalidateViewer_EntireData, viewer, &GraphicsView::receiveFileManagerData);

        // GraphicsView -> FileSearcher:
        connect(viewer, &GraphicsView::nextSearchResultSGNL, m_searcher, &FileSearcher::focusNextMatch);
        connect(viewer, &GraphicsView::prevSearchResultSGNL, m_searcher, &FileSearcher::focusPreviousMatch);
        connect(viewer, &GraphicsView::closeSearchMenuSGNL,  m_searcher, &FileSearcher::exitSearchMode);
        connect(viewer, &GraphicsView::searchForKeyWord,     m_searcher, &FileSearcher::search_QString);

        // GraphicsView -> FilesSelector:
        connect(viewer, &GraphicsView::selectEntireContent, m_selector, &FileSelector::selectEntireContent);
        connect(viewer, &GraphicsView::clearSelection,      m_selector, &FileSelector::clearSelection);
        connect(viewer, &GraphicsView::selectButtonUp,      m_selector, &FileSelector::selectPrevious);
        connect(viewer, &GraphicsView::selectButtonDown,    m_selector, &FileSelector::selectNext);
        connect(viewer, &GraphicsView::selectContent,       m_selector, &FileSelector::select_QString);

        // GraphicsView -> FileManager:
        connect(viewer, &GraphicsView::setRootFolder, this, &FileManager::setRoot_QDir);
        connect(viewer, &GraphicsView::deepSearch, this, &FileManager::deepSearch);
        connect(viewer, &GraphicsView::elapseAllFoldersOfDepthId, this, &FileManager::elapseAllFoldersOfDepthId);
        connect(viewer, &GraphicsView::elapseSelectedFoldersRecursively, this, &FileManager::elapseSelectedFoldersRecursively);
        connect(viewer, &GraphicsView::elapseSelectedFolders, this, &FileManager::elapseSelectedFolders);
        connect(viewer, &GraphicsView::collapseSelectedFoldersRecursively, this, &FileManager::collapseSelectedFoldersRecursively);
        connect(viewer, &GraphicsView::collapseSelectedFolders, this, &FileManager::collapseSelectedFolders);
        connect(viewer, &GraphicsView::elapseOrCollapseFolderDependingOnCurrentState, this, &FileManager::elapseOrCollapseFolderDependingOnCurrentState);
        connect(viewer, &GraphicsView::showHiddenFilesSGNL, this, &FileManager::setIncludeHiddenFiles);
        connect(viewer, &GraphicsView::requestOpenFoldersInTab, this, &FileManager::openSelectedFoldersInTab);
        connect(viewer, &GraphicsView::sortDir, this, &FileManager::sortDir);
        connect(viewer, &GraphicsView::sortAllFolders, this, &FileManager::sortAllFolders);
        connect(viewer, &GraphicsView::copySelectedFilePathToClipboard, this, &FileManager::copySelectedFilePathToClipboard);
        connect(viewer, &GraphicsView::copySelectedContent, this, &FileManager::copySelectedContent);
        connect(viewer, &GraphicsView::cutSelectedContent, this, &FileManager::cutSelectedContent);
        connect(viewer, &GraphicsView::duplicateSelectedContent, this, &FileManager::duplicateSelectedContent);
        connect(viewer, &GraphicsView::openSelectedContent, this, &FileManager::openSelectedContent);
        connect(viewer, &GraphicsView::openSelectedContentWith, this, &FileManager::openSelectedContentWith);
        connect(viewer, &GraphicsView::renameSelectedContent, this, &FileManager::renameSelectedContent);
        connect(viewer, &GraphicsView::pasteFromClipboard, this, &FileManager::pasteFromClipboard);
        connect(viewer, &GraphicsView::paste, this, &FileManager::paste);
        connect(viewer, &GraphicsView::deleteSelectedContent, this, &FileManager::deleteSelectedContent);
        connect(viewer, &GraphicsView::showDetailsOfSelectedContent, this, &FileManager::showDetailsOfSelectedContent);
        connect(viewer, &GraphicsView::zipSelectedContent, this, &FileManager::zipSelectedContent);
        connect(viewer, &GraphicsView::unzipSelectedContent, this, &FileManager::unzipSelectedContent);
        connect(viewer, &GraphicsView::createNewFolderSGNL, this, &FileManager::createNewFolder);
        connect(viewer, &GraphicsView::createNewFileSGNL, this, &FileManager::createNewFile);
        connect(viewer, &GraphicsView::killCurrentBlockingAction, this, &FileManager::killCurrentBlockingAction);
        connect(viewer, &GraphicsView::requestFocusSGNL, this, &FileManager::requestFocus);
        connect(viewer, &GraphicsView::initDragging, this, &FileManager::initDragging);
        connect(viewer, &GraphicsView::keyPressed, this, &FileManager::keyPressed);
        connect(viewer, &GraphicsView::setSelectionToRoot, this, &FileManager::setSelectionToRoot);
        connect(viewer, &GraphicsView::cdUp, this, &FileManager::setParentToRoot);
        connect(viewer, &GraphicsView::zoomFactorChanged, this, &FileManager::zoomFactorChanged);
        connect(viewer, &GraphicsView::requestFileViewerRevalidation, this, &FileManager::requestFileViewerRevalidation);
        connect(viewer, &GraphicsView::zoomFactorChanged,  this, &FileManager::setZoomFactor);
        connect(viewer, &GraphicsView::openTerminalSGNL, this, &FileManager::openTerminal);
        connect(viewer->verticalScrollBar(),   &QScrollBar::valueChanged,  this, &FileManager::saveGraphicsViewVBarValue);
        connect(viewer->horizontalScrollBar(), &QScrollBar::valueChanged,  this, &FileManager::saveGraphicsViewHBarValue);
        connect(viewer, &GraphicsView::requestCloseCurrentTab, this, &FileManager::closeCurrentTab); // signal -> signal!!!

    }else
        qDebug() << "trying to connect m_viewer -> nullptr! [root_path: "  << QString::fromStdString(m_root_path) << "]";
}

void FileManager::disconnectViewer(GraphicsView* viewer)
{
    if(viewer) // if, da m_viewer zu test-zwecken noch auf nullptr gesetzt ist. im eigentlichen programm dann eig nicht mehr notwendig darauf zu testen im konstruktor!
    {
        // FileManager -> GraphicsView:
        disconnect(this, &FileManager::startWaitingLoop,            viewer, &GraphicsView::startWaitingAnimation);
        disconnect(this, &FileManager::exitWaitingLoop,             viewer, &GraphicsView::killWaitingAnimation);
        disconnect(this, &FileManager::focusGraphicsView,           viewer, &GraphicsView::focusId);
        disconnect(this, &FileManager::requestFocusSGNL,            viewer, &GraphicsView::requestFocus);
        disconnect(this, &FileManager::revalidateViewer_Entries,    viewer, &GraphicsView::receiveFileViewers);
        disconnect(this, &FileManager::revalidateViewer_MetaData,   viewer, &GraphicsView::receiveFileManagerMetaData);
        disconnect(this, &FileManager::revalidateViewer_EntireData, viewer, &GraphicsView::receiveFileManagerData);

        // GraphicsView -> FileSearcher:
        disconnect(viewer, &GraphicsView::nextSearchResultSGNL, m_searcher, &FileSearcher::focusNextMatch);
        disconnect(viewer, &GraphicsView::prevSearchResultSGNL, m_searcher, &FileSearcher::focusPreviousMatch);
        disconnect(viewer, &GraphicsView::closeSearchMenuSGNL,  m_searcher, &FileSearcher::exitSearchMode);
        disconnect(viewer, &GraphicsView::searchForKeyWord,     m_searcher, &FileSearcher::search_QString);

        // GraphicsView -> FilesSelector:
        disconnect(viewer, &GraphicsView::selectEntireContent, m_selector, &FileSelector::selectEntireContent);
        disconnect(viewer, &GraphicsView::clearSelection,      m_selector, &FileSelector::clearSelection);
        disconnect(viewer, &GraphicsView::selectButtonUp,      m_selector, &FileSelector::selectPrevious);
        disconnect(viewer, &GraphicsView::selectButtonDown,    m_selector, &FileSelector::selectNext);
        disconnect(viewer, &GraphicsView::selectContent,       m_selector, &FileSelector::select_QString);

        // GraphicsView -> FileManager:
        disconnect(viewer, &GraphicsView::setRootFolder, this, &FileManager::setRoot_QDir);
        disconnect(viewer, &GraphicsView::deepSearch, this, &FileManager::deepSearch);
        disconnect(viewer, &GraphicsView::elapseAllFoldersOfDepthId, this, &FileManager::elapseAllFoldersOfDepthId);
        disconnect(viewer, &GraphicsView::elapseSelectedFoldersRecursively, this, &FileManager::elapseSelectedFoldersRecursively);
        disconnect(viewer, &GraphicsView::elapseSelectedFolders, this, &FileManager::elapseSelectedFolders);
        disconnect(viewer, &GraphicsView::collapseSelectedFoldersRecursively, this, &FileManager::collapseSelectedFoldersRecursively);
        disconnect(viewer, &GraphicsView::collapseSelectedFolders, this, &FileManager::collapseSelectedFolders);
        disconnect(viewer, &GraphicsView::elapseOrCollapseFolderDependingOnCurrentState, this, &FileManager::elapseOrCollapseFolderDependingOnCurrentState);
        disconnect(viewer, &GraphicsView::showHiddenFilesSGNL, this, &FileManager::setIncludeHiddenFiles);
        disconnect(viewer, &GraphicsView::requestOpenFoldersInTab, this, &FileManager::openSelectedFoldersInTab);
        disconnect(viewer, &GraphicsView::sortDir, this, &FileManager::sortDir);
        disconnect(viewer, &GraphicsView::sortAllFolders, this, &FileManager::sortAllFolders);
        disconnect(viewer, &GraphicsView::copySelectedFilePathToClipboard, this, &FileManager::copySelectedFilePathToClipboard);
        disconnect(viewer, &GraphicsView::copySelectedContent, this, &FileManager::copySelectedContent);
        disconnect(viewer, &GraphicsView::cutSelectedContent, this, &FileManager::cutSelectedContent);
        disconnect(viewer, &GraphicsView::duplicateSelectedContent, this, &FileManager::duplicateSelectedContent);
        disconnect(viewer, &GraphicsView::openSelectedContent, this, &FileManager::openSelectedContent);
        disconnect(viewer, &GraphicsView::openSelectedContentWith, this, &FileManager::openSelectedContentWith);
        disconnect(viewer, &GraphicsView::renameSelectedContent, this, &FileManager::renameSelectedContent);
        disconnect(viewer, &GraphicsView::pasteFromClipboard, this, &FileManager::pasteFromClipboard);
        disconnect(viewer, &GraphicsView::paste, this, &FileManager::paste);
        disconnect(viewer, &GraphicsView::deleteSelectedContent, this, &FileManager::deleteSelectedContent);
        disconnect(viewer, &GraphicsView::showDetailsOfSelectedContent, this, &FileManager::showDetailsOfSelectedContent);
        disconnect(viewer, &GraphicsView::zipSelectedContent, this, &FileManager::zipSelectedContent);
        disconnect(viewer, &GraphicsView::unzipSelectedContent, this, &FileManager::unzipSelectedContent);
        disconnect(viewer, &GraphicsView::createNewFolderSGNL, this, &FileManager::createNewFolder);
        disconnect(viewer, &GraphicsView::createNewFileSGNL, this, &FileManager::createNewFile);
        disconnect(viewer, &GraphicsView::killCurrentBlockingAction, this, &FileManager::killCurrentBlockingAction);
        disconnect(viewer, &GraphicsView::requestFocusSGNL, this, &FileManager::requestFocus);
        disconnect(viewer, &GraphicsView::initDragging, this, &FileManager::initDragging);
        disconnect(viewer, &GraphicsView::keyPressed, this, &FileManager::keyPressed);
        disconnect(viewer, &GraphicsView::setSelectionToRoot, this, &FileManager::setSelectionToRoot);
        disconnect(viewer, &GraphicsView::cdUp, this, &FileManager::setParentToRoot);
        disconnect(viewer, &GraphicsView::zoomFactorChanged, this, &FileManager::zoomFactorChanged);
        disconnect(viewer, &GraphicsView::requestFileViewerRevalidation, this, &FileManager::requestFileViewerRevalidation);
        disconnect(viewer, &GraphicsView::zoomFactorChanged,  this, &FileManager::setZoomFactor);
        disconnect(viewer, &GraphicsView::openTerminalSGNL, this, &FileManager::openTerminal);
        disconnect(viewer->verticalScrollBar(),   &QScrollBar::valueChanged,  this, &FileManager::saveGraphicsViewVBarValue);
        disconnect(viewer->horizontalScrollBar(), &QScrollBar::valueChanged,  this, &FileManager::saveGraphicsViewHBarValue);
        disconnect(viewer, &GraphicsView::requestCloseCurrentTab, this, &FileManager::closeCurrentTab); // signal -> signal!!!

    }else
        qDebug() << "trying to DISconnect m_viewer -> nullptr! [root_path: "  << QString::fromStdString(m_root_path) << "]";
}

void FileManager::connectDirectorySelectionPane(DirectorySelectionPane* toolBar)
{
    // DirectoryStack -> DirectorySelectionPane:
    connect(m_dirStack, &DirectoryStack::stackIsEmpty, toolBar, &DirectorySelectionPane::disableUndo);
    connect(m_dirStack, &DirectoryStack::stackContainsPaths, toolBar, &DirectorySelectionPane::enableUndo);

    // FileManager -> DirectorySelectionPane:
    connect(this, SIGNAL(rootDirChanged(std::string)), toolBar, SLOT(setFolder(std::string)));
    connect(this, &FileManager::rootDirChanged,    toolBar, &DirectorySelectionPane::revalidate);

    // DirectorySelectionPane -> FileManager:
    connect(toolBar, &DirectorySelectionPane::undo,           this, &FileManager::setLastPathToRoot);
    connect(toolBar, SIGNAL(buttonClicked(QDir)),  this, SLOT(setRoot_QDir(QDir)));
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

void FileManager::revalidateViewer_entries_hlpr()
{
    emit revalidateViewer_Entries( generateViewerData() );
}

void FileManager::revalidateViewer_metaData_hlpr()
{
    emit revalidateViewer_MetaData( new FileManagerInfo(*this) );
}

void FileManager::revalidateViewer_EntireData_hlpr()
{
    emit revalidateViewer_EntireData( generateViewerData(), new FileManagerInfo(*this) );
}

std::unordered_map<int_bd, FiBDViewer> FileManager::generateViewerData()
{
    std::unordered_map<int_bd, FiBDViewer> entries;
    for(auto i=m_frstDispFile; i <= m_lastDispFile; ++i)
    {
        auto path = m_order_entries_colpsd[i];

        auto depthId = m_path_depthId_colpsd[path];

        // entweder ist es ein dir:
        if(m_folders_colpsd.find(path) != m_folders_colpsd.end())
        {
            auto* fi = m_folders_colpsd[path];

            FiBDViewer fiView(fi, depthId, false);

            bool selected = m_selector->isSelected(path);
            bool searched = m_searcher->isSearched(path);
            fiView.setSelected(selected);
            fiView.setSearched(searched);

            entries.emplace(std::make_pair(i, fiView));
//            entries[i] = fiView;
        }else{
            // dann muss es ein file sein:
            QFileInfo fi(QString::fromStdString(path));

            FiBDViewer fiView(fi, depthId);

            bool selected = m_selector->isSelected(path);
            bool searched = m_searcher->isSearched(path);
            fiView.setSelected(selected);
            fiView.setSearched(searched);

            entries.emplace(std::make_pair(i, fiView));
//            entries[i] = fiView;
        }
    }
    return entries;
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

    m_path_depthId.clear();
    m_path_depthId_colpsd.clear();

    m_path_fileNames.clear();
    m_path_fileNames_colpsd.clear();

    m_depth_folders_colpsd.clear();

//    m_depthId_elapsed.clear();
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

void FileManager::focusPath(string absPath)
{
    if(m_entries_order_colpsd.find(absPath) != m_entries_order_colpsd.end())
    {
        auto order = m_entries_order_colpsd[absPath];
        emit focusGraphicsView(order);
    }
}

QString getPaddingString(int cnt)
{
    QString s("");
    for(int i=0; i < cnt; ++i)
    {
        s = s.append("  ");
    }
    return s;
}

void FileManager::printEntries()
{
    for(std::size_t i=0; i < m_paths_colpsd.size(); ++i)
    {
        auto path = m_order_entries_colpsd[static_cast<int_bd>(i)];
        auto order = m_entries_order[path];
        int depth = m_path_depthId[path];
        QString padding = getPaddingString(depth);
        qDebug() << QString("%1: %2%3   -> order: %4").arg(i)
                                     .arg(padding)
                                     .arg(QString::fromStdString(path))
                                     .arg(order);
    }
}

void FileManager::replaceTree(DirManagerInfo* tree)
{
    if(m_tree)
        delete m_tree;

    m_tree = tree;

    revalidateTree();
}

void FileManager::revalidateTree()
{
    clearEntryContainers();

    int_bd* cntr = new int_bd(0);
    int_bd* cntr_colpsd= new int_bd(0);
    int* maxDepthId = new int(0);

    revalidateTree_hlpr(m_tree, m_tree, false, cntr, cntr_colpsd, 0, maxDepthId);

    while(m_depthId_elapsed.size() <= static_cast<std::size_t>((*maxDepthId)))
    {
        m_depthId_elapsed.push_back( m_depthId_elapsed.size() == 0 ? m_tree->isElapsed : false );
    }
    while( (m_depthId_elapsed.size() > (static_cast<std::size_t>((*maxDepthId)) +  1)) &&
           (m_depthId_elapsed.size() > 0) )
    {
        m_depthId_elapsed.pop_back();
    }

    if(m_selector)
    {
        m_selector->entriesChanged();
    }
    if(m_searcher)
    {
        m_searcher->entriesChanged();
    }

    delete cntr;
    delete cntr_colpsd;
    delete maxDepthId;

//    printEntries();

    revalidateViewer_EntireData_hlpr();
}

void FileManager::revalidateTree_hlpr(DirManagerInfo* entry,
                                   DirManagerInfo* firstNonCollapsedFold,
                                   bool isCollapsed,
                                   int_bd* cntr,
                                   int_bd* cntr_clpsd,
                                   int depthId,
                                   int* maxDepthId)
{
    if(*maxDepthId < depthId)
        *maxDepthId = depthId;

    if( (entry->isHidden && m_showHiddenFiles) || (!entry->isHidden) )
    {
        std::string path = entry->absPath;

        m_folders[path] = entry;

        m_paths.emplace(path);
        m_folder_paths.emplace(path);
        m_entry_to_firstElapsedFolder[path] = firstNonCollapsedFold->absPath;

        m_entries_order[path] = *cntr;
        m_order_entries[*cntr] = path;

        m_path_fileNames[path] = entry->fileName;

        m_path_depthId[path] = depthId;

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
            m_entries_order_colpsd[path] = *cntr_clpsd;
            m_order_entries_colpsd[*cntr_clpsd] = path;
            m_path_fileNames_colpsd[path] = entry->fileName;

            m_path_depthId_colpsd[path] = depthId;

            ++(*cntr_clpsd);
        }

        ++(*cntr);


        bool subs_collapsed = isCollapsed || !entry->isElapsed;

        for(auto* sub_dir: entry->subDirs_sorted)
        {
            DirManagerInfo* fncf = isCollapsed ? firstNonCollapsedFold : sub_dir;

            revalidateTree_hlpr(sub_dir, fncf, subs_collapsed, cntr, cntr_clpsd, depthId + 1, maxDepthId);
        }

        for(const auto& filePath_fileName: entry->files_sorted)
        {
            const auto& file_path = filePath_fileName.first;
            const auto& file_name = filePath_fileName.second;

            m_paths.insert(file_path);

            m_path_depthId[file_path] = depthId + 1;

            m_entries_order[file_path] = *cntr;
            m_order_entries[*cntr] = file_path;

            m_path_fileNames[file_path] = file_name;

            if(!subs_collapsed)
            {
                m_paths_colpsd.insert(file_path);

                m_entries_order_colpsd[file_path] = *cntr_clpsd;
                m_order_entries_colpsd[*cntr_clpsd] = file_path;

                m_path_depthId_colpsd[file_path] = depthId + 1;

                m_path_fileNames_colpsd[file_path] = file_name;

                ++(*cntr_clpsd);
            }

            ++(*cntr);
        }
    }
}

//----------------------------------------------------------

QString FileManager::curRootPath() const
{
    return QString::fromStdString(m_root_path);
}

QString FileManager::curSearchResult() const
{
    return QString::fromStdString(m_searcher->focusedSearchPath());
}

int FileManager::maxDepth() const
{
    return static_cast<int>(m_depthId_elapsed.size());
}

int_bd FileManager::selectionCount() const
{
    return m_selector->selectionCount();
}

int_bd FileManager::displayedFileCount() const
{
    return static_cast<int_bd>(m_paths_colpsd.size());
}

int_bd FileManager::indexCurSearchResult() const
{
    return m_searcher->indexCurSearchResult();
}

int_bd FileManager::searchIndex() const
{
    return m_searcher->searchIndex();
}

int_bd FileManager::searchResultsCount() const
{
    return m_searcher->searchResultsCount();
}

bool FileManager::includeHiddenFiles() const
{
    return m_showHiddenFiles;
}

bool FileManager::inSearchMode() const
{
    return m_searcher->inSearchMode();
}

bool FileManager::foldersSelected() const
{
    return m_selector->foldersSelected();
}

bool FileManager::filesSelected() const
{
    return m_selector->filesSelected();
}

bool FileManager::selectionContainsZipFiles() const
{
    return false;
}

std::vector<bool> FileManager::depthIdElapsed() const
{
    return std::vector<bool>(m_depthId_elapsed.begin(), m_depthId_elapsed.end());
}
