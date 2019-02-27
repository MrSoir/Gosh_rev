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
        self.crntly_prcsd_wrkr = None
        
    def cancel(self):
        self._cancelled = True
        
    def cancelled(self):
        return self._cancelled
    
    def setProgressDialog(self, progress_dialog):
        self.progress_dialog = progress_dialog
        
    def executeThreaded(self):
        print('Worker.executeThreaded')
        pass

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
        print('CopyFiles.finish - ', self._cancelled)
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
            
    @staticmethod
    def createWorkerDirFromPath(baseDir, entryName='', parentWorkerDirEntry=None, tarBaseDir=None, tarEntryName=None, processFunc=lambda absSrcPath, absTarPath: True):
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
        
        if os.path.exists(absPath) and os.path.isdir(absPath):
            for entry in os.listdir(absPath):
                absEntryPath = os.path.join(absPath, entry)
                sub_workerEntry = WorkerDirEntry.createWorkerDirFromPath(baseDir=None, entryName=entry, parentWorkerDirEntry=we, processFunc=processFunc)
                if os.path.isdir(absEntryPath):
                    we.sub_workerDirEntries.append( sub_workerEntry )
                elif os.path.isfile(absEntryPath):
                    we.sub_workerFileEntries.append( sub_workerEntry )
                    
        return we
                

    