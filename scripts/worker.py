#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Created on Thu Feb 21 13:30:43 2019

@author: hippo
"""

from dataclasses import dataclass, field

import os.path
import os
import wx

import shutil
import sys, traceback
#from shutil import copy2
import argparse
import time
import concurrent.futures
import threading
from threading import Thread
from concurrent.futures import ProcessPoolExecutor, ThreadPoolExecutor

import static_functions
from static_functions import UserAnswer
                                    
import threadsafeDialogCreator as DialgCrtr
from   threadsafeDialogCreator import EVT_DIALOG_REQUEST

from progress_dialog_widget import ProgressFrame, EVT_MESSAGE_ID, EVT_PROGRESS_ID, EVT_FINISH_ID

from path_func import *


class Worker(wx.EvtHandler):
    def __init__(self, progress_dialog = None):
        super().__init__()
        self.progress_dialog = progress_dialog
        self._cancelled = False
        self.entriesToProcess = 0
        self.entriesAlrProcessed = 0
        self.workers = list()
        self.finishedWorkers = list()
        self._threaded = False
        
        self.TsDC = DialgCrtr.ThreadsafeDialogCreator(self)
        self.Bind(EVT_DIALOG_REQUEST, self.replacePathResponse,   id=DialgCrtr.EVT_REPLACE_DIR_ID)
        self.Bind(EVT_DIALOG_REQUEST, self.replacePathResponse,   id=DialgCrtr.EVT_REPLACE_FILE_ID)
        self.Bind(EVT_DIALOG_REQUEST, self.receiveValidEntryName, id=DialgCrtr.EVT_VALID_DIR_NAME_ID)
        self.Bind(EVT_DIALOG_REQUEST, self.receiveValidEntryName, id=DialgCrtr.EVT_VALID_FILE_NAME_ID)
        
    def _post__init__(self):
        self.crntly_prcsd_wrkr = self.workers[0] if len(self.workers) > 0 else None
        
    def cancel(self):
        print('worker - cancel!')
        self._cancelled = True
        
    def cancelled(self):
        return self._cancelled
    
    def setProgressDialog(self, progress_dialog):
        self.progress_dialog = progress_dialog
    
    def executeThreaded(self):
        self.copyFilesThreaded()
            
    def copyFilesThreaded(self):
        print('copyFilesThreaded')
        self._threaded = True
        Thread(target=self.copyFiles, name='COPY_FILES_THREAD').start()
#        self.copyFiles()
    
    def copyFiles(self):
        self.evalEntryCount()
        
        self.copyCurrentWorkerEntry()
    
    def postProcessWorker(self, worker):
        try:
            worker.postProcessFunc(worker)
        except:
            pass
        
        
    def __cancel(self):
        self._cancelled = True
        self.finish()
        
    
    def evalEntryCount(self):
        self.postMessage('processing...')
        self.entriesToProcess = 0
        for worker in self.workers:
            self.entriesToProcess += worker.evalEntryCount()
            
    def copyCurrentWorkerEntry(self):
        worker_entry = self.crntly_prcsd_wrkr
                
        if not worker_entry:
            print('something went horribly wrong...')
            return False

        absSrcPath = worker_entry.getAbsSrcPath()
        
        self.updateCurrentlyProcessedEntry(absSrcPath)

        if absSrcPath != self.target_path and not os.path.exists(absSrcPath):
            print('copyWorkerEntry: absSrcPath "%s" does not exist!' % absSrcPath)
            return False

        absSrcPath    = worker_entry.getAbsSrcPath()
        absTarBaseDir = worker_entry.getAbsTarBasePath()
        tarEntryName  = worker_entry.tarEntryName
            
        absTarPath = os.path.join(absTarBaseDir, tarEntryName)
        
        print('absSrcPath: ', absSrcPath)
        print('absTarPath: ', absTarPath)
        
#        srcIsDir  = self.crntly_prcsd_wrkr.isDir()
#        srcIsFile = self.crntly_prcsd_wrkr.isFile()

        # wenn absTarPath noch nicht existiert - einfach kopieren
        if not os.path.exists(absTarPath):
            success = self.crntly_prcsd_wrkr.getProcessFunc()(absSrcPath, absTarPath)
            print('absTarPath does not exist - success: ', success)
            self.finishCurrentWorkerEntry(success)
        else:
            """ falls absTarPath bereits exisitert, user fragen ob:
                    1. absTarPath geloescht werden soll,
                    2. falls nicht geloscht werden soll, user nach alternativem tarEntryName fragen:"""
            self.askUserIfTarPathShouldBeReplaced(absTarBaseDir, tarEntryName)
        

    def askUserIfTarPathShouldBeReplaced(self, absTarBaseDir, tarEntryName):
        print('askUserIfTarPathShouldBeReplaced: ', absTarBaseDir, '/', tarEntryName)
        absTarPath = os.path.join(absTarBaseDir, tarEntryName)
        if os.path.isfile(absTarPath):
            wx.PostEvent(self.TsDC, DialgCrtr.DialogRequestEvent(DialgCrtr.EVT_ASK_REPLACE_FILE_ID, data=[absTarBaseDir, tarEntryName, self.progress_dialog]))
        elif os.path.isdir(absTarPath):
            wx.PostEvent(self.TsDC, DialgCrtr.DialogRequestEvent(DialgCrtr.EVT_ASK_REPLACE_DIR_ID,  data=[absTarBaseDir, tarEntryName, self.progress_dialog]))
        else:
            raise Exception('askUserIfTarPathShouldBeReplaced - absTarPath "%s" is neither a file nor a dir!' % absTarPath)
    
    def replacePathResponse(self, dialogReqstEvnt):
        if self._threaded:
            Thread(target=self.replacePathResponse_hlpr, args=(dialogReqstEvnt,), name='COPY_FILES_THREAD').start()
        else:
            self.replacePathResponse_hlpr(dialogReqstEvnt)
            
    def replacePathResponse_hlpr(self, dialogReqstEvnt):
        replace = dialogReqstEvnt.data
        if replace == wx.ID_CANCEL:
            self.__cancel()
            return
            
        if replace == wx.ID_YES:
            if self.crntly_prcsd_wrkr:
                absTarPath = self.crntly_prcsd_wrkr.getAbsTarPath()
                if absTarPath and os.path.exists(absTarPath):
                    static_functions.deleteEntry(absTarPath)
                if os.path.exists(absTarPath):
                    self.finishCurrentWorkerEntry(False)
                else:
                    absSrcPath = self.crntly_prcsd_wrkr.getAbsSrcPath()
                    copied_successfully = self.crntly_prcsd_wrkr.getProcessFunc()(absSrcPath, absTarPath)                        
                    self.finishCurrentWorkerEntry(copied_successfully)
                return
            else:
                raise Exception('replacePathResponse - replcae==True -> bud self.crntly_prcsd_wrkr is invalid!!!')
        elif wx.ID_NO:
            self.askForAlternativeEntryName()
        else:
            raise ValueError('replacePathResponse - replace-value unbekannt!')
    
    def askForAlternativeEntryName(self):
        if not self.crntly_prcsd_wrkr:
            raise Exception('askForAlternativeEntryName - self.crntly_prcsd_wrkr is None!!!')
    
        baseTarDir = self.crntly_prcsd_wrkr.getAbsTarBasePath()
        entryName = self.crntly_prcsd_wrkr.tarEntryName
        isFile = self.crntly_prcsd_wrkr.isFile()
        isDir = self.crntly_prcsd_wrkr.isDir()
        if isFile:
            wx.PostEvent(self.TsDC, DialgCrtr.DialogRequestEvent(DialgCrtr.EVT_ASK_VALID_FILE_NAME_ID, data=[baseTarDir, entryName, self.progress_dialog]))
        elif isDir:
            wx.PostEvent(self.TsDC, DialgCrtr.DialogRequestEvent(DialgCrtr.EVT_ASK_VALID_DIR_NAME_ID,  data=[baseTarDir, entryName, self.progress_dialog]))
        else:
            raise Exception("'%s' is neither a file nor a dir!!!" % self.crntly_prcsd_wrkr.getAbsTarPath())
            
    def receiveValidEntryName(self, dialogRqstEvnt):
        if self._threaded:
            Thread(target=self.receiveValidEntryName_hlpr, args=(dialogRqstEvnt,), name='COPY_FILES_THREAD').start()
        else:
            self.receiveValidEntryName_hlpr(dialogRqstEvnt)
            
    def receiveValidEntryName_hlpr(self, dialogRqstEvnt):
        usrSlctedEntryName = dialogRqstEvnt.data
        
        if not usrSlctedEntryName:
            # user selected wx.Cancel!!!:
            self.__cancel()
            return
        
        if not self.crntly_prcsd_wrkr:
            raise Exception('receiveValidEntryName - self.crntly_prcsd_wrkr is None!!!')
            
        self.crntly_prcsd_wrkr.tarEntryName = usrSlctedEntryName
        
        absTarPath = self.crntly_prcsd_wrkr.getAbsTarPath()
        absSrcPath = self.crntly_prcsd_wrkr.getAbsSrcPath()
        
        if os.path.exists(absTarPath):
            print('receiveValidEntryName - received entryName "%s" that already exists!!!' % absTarPath)
            self.finishCurrentWorkerEntry(False)
        else:
            success = self.crntly_prcsd_wrkr.getProcessFunc()(absSrcPath, absTarPath)
            self.finishCurrentWorkerEntry(success)
            

    def finishCurrentWorkerEntry(self, success):
        worker = self.crntly_prcsd_wrkr
        if not worker:
            raise Exception('finishCurrentWorkerEntry - self.crntly_prcsd_wrkr is None!!!')
        worker.setFinished(success)
        self.finishedWorkers.append( worker)
        self.postProcessWorker(worker)
        if not success and worker.abortIfFailed():
            self.__cancel()
        else:
            self.processNextWorkerEntry()
    
    def processNextWorkerEntry(self):
        if self._cancelled:
            self.finish()
            return
            
        self.incrementProgress()
        
        if self.crntly_prcsd_wrkr:
            self.crntly_prcsd_wrkr = self.crntly_prcsd_wrkr.getNext()
             # returns the next worker if avaliable, else None!
            
        if not self.crntly_prcsd_wrkr:
            for worker in self.workers:
                if worker not in self.finishedWorkers:
                    self.crntly_prcsd_wrkr = worker
                    break
        
        if self.crntly_prcsd_wrkr:
            msg = "copying '%s'" % self.crntly_prcsd_wrkr.entryName
            wx.PostEvent(self.progress_dialog, DialgCrtr.DialogRequestEvent(EVT_MESSAGE_ID, data=msg))
            self.copyCurrentWorkerEntry()
        else:
            self.finish()
            
    def finish(self):
        print('CopyFiles.finish - cancelled: ', self._cancelled)
        totalSuccess = not self._cancelled
        if totalSuccess:
            for worker in self.workers:
                totalSuccess = worker.evalSuccess() and totalSuccess
                if not totalSuccess:
                    break
        try:
            self.callOnFinished(totalSuccess)
        except:
            wx.PostEvent(self.progress_dialog, DialgCrtr.DialogRequestEvent(EVT_FINISH_ID, data=totalSuccess))
        
        
    def incrementProgress(self):
        self.entriesAlrProcessed += 1
        self.updateProgress()
        
    def updateProgress(self):
        if self.progress_dialog and self.entriesToProcess > 0:
            progr = self.entriesAlrProcessed / self.entriesToProcess * 100
            wx.PostEvent(self.progress_dialog, DialgCrtr.DialogRequestEvent(EVT_PROGRESS_ID, data=progr))
            
    def updateCurrentlyProcessedEntry(self, absPath):
        if absPath and self.progress_dialog:
            entryName = os.path.basename(absPath)
            if entryName:
                wx.PostEvent(self.progress_dialog, DialgCrtr.DialogRequestEvent(EVT_MESSAGE_ID, data=entryName))
            else:
                wx.PostEvent(self.progress_dialog, DialgCrtr.DialogRequestEvent(EVT_MESSAGE_ID, data=absPath))

    def postMessage(self, msg):
        if self.progress_dialog:
                wx.PostEvent(self.progress_dialog, DialgCrtr.DialogRequestEvent(EVT_MESSAGE_ID, data=msg))


#-------------------------------------------------------------------




    
@dataclass
class WorkerDirEntry:
    entryName: str
    baseDir: str = None
    
    tarBaseDir: str = None
    tarEntryName: str = None
    
    parentWorkerDirEntry: object = None
    
    sub_workerDirEntries:  list = field(default_factory=list)
    sub_workerFileEntries: list = field(default_factory=list)
    
    finishedWorkers: list = field(default_factory=list)
    
    __isFile: bool = False
    __isDir:  bool = False
    __isLink: bool = False

    __failed:   bool = False
    __finished: bool = False
    
    __abortIfFailed: bool = False
    
    #-------
    
    def __post_init__(self):
        self.__processFunc = lambda absSrcPath, absTarPath: True
        
    #-------
    
    def abortIfFailed(self):
        return self.__abortIfFailed
    def setAbortIfFailed(self, abortIfFailed):
        self.__abortIfFailed = abortIfFailed
    
    #-------
    
    def getProcessFunc(self):
        return self.__processFunc
    def setProcessFunc(self, func):
        self.__processFunc = func
    
    #-------
    
    def reset(self):
        self.parentWorkerDirEntry.reset() if self.parentWorkerDirEntry else self.reset_hlpr()
        
    def reset_hlpr(self):
        self.__finished = False
        self.__failed   = False
        self.parentWorkerDirEntry.finishedWorkers.clear()
        
        for sub_worker in self.sub_workerDirEntries:
            sub_worker.reset_hlpr()
        for sub_worker in self.sub_workerFileEntries:
            sub_worker.reset_hlpr()
        
    #-------
    
    def setFinished(self, success):
        self.__finished = True
        self.__failed = not success
        self.tellParentThisWorkerHasFinished()
        
    def setFailed(self):
        self.setFinished(False)
        
    def setSucceeded(self):
        self.setFinished(True)
    #--    
    def failed(self):
        return self.__failed
    def finished(self):
        return self.__finished
    
    #-------
    
    def tellParentThisWorkerHasFinished(self):
        if self.parentWorkerDirEntry:
            self.parentWorkerDirEntry.finishedWorkers.append(self)
            
    def evalSuccess(self):
        return self.parentWorkerDirEntry.evalSuccess() if  self.parentWorkerDirEntry else self.evalSuccess_hlpr()
    
    def evalSuccess_hlpr(self):
        return not self.failed() and self.__finished \
               and all([sub_dir.evalSuccess_hlpr() for sub_dir in self.sub_workerDirEntries])\
               and all([file   .evalSuccess_hlpr() for file    in self.sub_workerFileEntries])
    
    #-------
    
    def isFile(self):
        return self.__isFile
    
    def isDir(self):
        return self.__isDir
    
    def isLink(self):
        return self.__isLink
    
    def setIsFile(self, isFile):
        self.__isFile = isFile
    def setIsDir(self, isDir):
        self.__isDir = isDir
    def setIsLink(self, isLink):
        self.__isLink = isLink
    #-------
    
    def evalDirCount(self):
        return self.evalEntryCount_hlpr()[0]
    
    def evalFileCount(self):
        return self.evalEntryCount_hlpr()[1]
    
    def evalEntryCount(self):
        return sum(self.evalEntryCount_hlpr())
    
    
    def evalEntryCount_hlpr(self):
        dirCnt, fileCnt = 0, len(self.sub_workerFileEntries)
        for sub_dir in self.sub_workerDirEntries:
            sub_dirCnt, sub_fileCnt = sub_dir.evalEntryCount_hlpr()
            dirCnt  += sub_dirCnt
            fileCnt += sub_fileCnt
        
        dirCnt  = dirCnt  + 1 if self.__isDir  else dirCnt
        fileCnt = fileCnt + 1 if self.__isFile else fileCnt
        return dirCnt, fileCnt
    #-------
    
    def getAbsSrcPath(self):
        baseDir = self.baseDir if self.baseDir else self.parentWorkerDirEntry.getAbsSrcPath()
        entryName = self.entryName
        return os.path.join(baseDir, entryName)
    
    def getAbsSrcBasePath(self):
        return self.baseDir if self.baseDir else self.parentWorkerDirEntry.getAbsSrcPath()
    
    #-------
    
    def getAbsTarPath(self):
        tarBaseDir = self.tarBaseDir if self.tarBaseDir else self.parentWorkerDirEntry.getAbsTarPath()
        tarEntryName = self.tarEntryName
        if tarBaseDir:
            return os.path.join(tarBaseDir, tarEntryName)
        else:
            return None
        
    def getAbsTarBasePath(self):
        return self.tarBaseDir if self.tarBaseDir else self.parentWorkerDirEntry.getAbsTarPath()
    
    #-------
    
    def getRelTarPath(self):
        relTarBaseDir = '' if self.tarBaseDir else self.parentWorkerDirEntry.getRelTarPath()
        tarEntryName = self.tarEntryName
        return os.path.join(relTarBaseDir, tarEntryName)
    def getRelTarBasePath(self):
        return '' if self.tarBaseDir else self.parentWorkerDirEntry.getRelTarPath()
    
    #-------
    
    def hasNext(self):
        return self.getNext() is not None
            
    
    def getNext(self):
        if not self.__failed:
            for subDirWorker in self.sub_workerDirEntries:
                if subDirWorker not in self.finishedWorkers:
                    return subDirWorker
            for subFileWorker in self.sub_workerFileEntries:
                if subFileWorker not in self.finishedWorkers:
                    return subFileWorker
        return self.parentWorkerDirEntry.getNext() if self.parentWorkerDirEntry else None 
    
    #-------
    
    def evalSubEntries(self):
        absPath = self.getAbsSrcPath()
        if os.path.exists(absPath) and os.path.isdir(absPath):
            for entry in os.listdir(absPath):
                absEntryPath = os.path.join(absPath, entry)
                sub_workerEntry = WorkerDirEntry.createWorkerDirFromPath(baseDir=None, entryName=entry, parentWorkerDirEntry=self, processFunc=self.__processFunc)
                if os.path.isdir(absEntryPath):
                    self.sub_workerDirEntries.append( sub_workerEntry )
                elif os.path.isfile(absEntryPath):
                    self.sub_workerFileEntries.append( sub_workerEntry )
    #-------
    
    @staticmethod
    def createWorkerDirFromPath(baseDir, entryName='', parentWorkerDirEntry=None, tarBaseDir=None, tarEntryName=None, 
                                processFunc=lambda absSrcPath, absTarPath: True,
                                recursive=True):
        """ createWorkerDirFromPath kann foldengermassen aufgerufen werden:
                1. mit 2 argumenten: baseDir + entryName = absPath
                2. mit 1 argument:   baseDir = absPath """        
        bd = baseDir
        en = entryName
        baseDir    = None if en else os.path.dirname (bd)
        entryName  = en   if en else os.path.basename(bd)
        tarEntryName = tarEntryName if tarEntryName else entryName
                
        we = WorkerDirEntry(entryName=entryName, baseDir=baseDir,
                            parentWorkerDirEntry=parentWorkerDirEntry, 
                            tarBaseDir=tarBaseDir, tarEntryName=tarEntryName)
        we.setProcessFunc(processFunc)
        
        absPath = we.getAbsSrcPath()
        
        we.__isDir  = os.path.isdir (absPath)
        we.__isFile = os.path.isfile(absPath)
        we.__isLink = os.path.islink(absPath)
        
        if recursive:
            we.evalSubEntries()
                    
        return we
                

    