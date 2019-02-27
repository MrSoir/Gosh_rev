#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Created on Wed Feb 20 13:57:02 2019

@author: hippo
"""

import wx
import os.path
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

from worker import Worker, WorkerDirEntry

from path_func import *
    
class CopyFiles(Worker):
    def __init__(self, source_paths=list(), target_path='', progress_dialog=None):
        super().__init__(progress_dialog)
        print('in CopyFiles-constructor!')
        
        self.TsDC = DialgCrtr.ThreadsafeDialogCreator(self)
        self.Bind(EVT_DIALOG_REQUEST, self.replacePathResponse,   id=DialgCrtr.EVT_REPLACE_DIR_ID)
        self.Bind(EVT_DIALOG_REQUEST, self.replacePathResponse,   id=DialgCrtr.EVT_REPLACE_FILE_ID)
        self.Bind(EVT_DIALOG_REQUEST, self.receiveValidEntryName, id=DialgCrtr.EVT_VALID_DIR_NAME_ID)
        self.Bind(EVT_DIALOG_REQUEST, self.receiveValidEntryName, id=DialgCrtr.EVT_VALID_FILE_NAME_ID)
        
        self.workers = list()
        if target_path:
            targetDirBasePath = os.path.dirname(target_path)
            targetDirName = os.path.basename(target_path)
            targetDirWorker = WorkerDirEntry(entryName=targetDirName, baseDir=targetDirBasePath, tarBaseDir=targetDirBasePath, tarEntryName=targetDirName)
            targetDirWorker.setAbortIfFailed(True)
            targetDirWorker.setProcessFunc(lambda absSrcPath, absTarPath: static_functions.createDirectory(absTarPath))
            targetDirWorker.setIsDir(True)
            targetDirWorker.postProcessFunc = self.setTargetBasePathToWorkers
            self.workers.append( targetDirWorker )
        for source_path in source_paths:
            self.workers.append( WorkerDirEntry.createWorkerDirFromPath(source_path, tarBaseDir=target_path, processFunc=static_functions.copyEntry) )
            
        self.finishedWorkers = list()
        
        print('workers: ', len(self.workers))
            
        self.crntly_prcsd_wrkr = self.workers[0] if len(self.workers) > 0 else None
            
        self.target_path = target_path
        self._threaded = False
    
    def executeThreaded(self):
        print('executeThreaded')
        self.copyFilesThreaded()
            
    def copyFilesThreaded(self):
        print('copyFilesThreaded')
        self._threaded = True
        Thread(target=self.copyFiles, name='COPY_FILES_THREAD').start()
#        self.copyFiles()
    
    def copyFiles(self):
        print('\ncopyFiles - starting to copy files...')
        self.evalEntryCount()
        
        self.copyCurrentWorkerEntry()
    
    def setTargetBasePathToWorkers(self, worker):
        print('setTargetPathToWorkers - target_path: ', self.target_path)
        target_path = worker.getAbsTarPath()
        for w in self.workers:
            if w != worker:
                w.tarBaseDir = target_path
    
    def postProcessWorker(self, worker):
        try:
            worker.postProcessFunc(worker)
        except:
            pass
        
        
    def __cancel(self):
        self._cancelled = True
        self.finish()
        
    
    def evalEntryCount(self):
        if self.progress_dialog:
            wx.PostEvent(self.progress_dialog, DialgCrtr.DialogRequestEvent(EVT_MESSAGE_ID, data='processing source paths...'))
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
            print('in yes')
            if self.crntly_prcsd_wrkr:
                print('I')
                absTarPath = self.crntly_prcsd_wrkr.getAbsTarPath()
                print('absTarPath: ', absTarPath)
                if absTarPath and os.path.exists(absTarPath):
                    static_functions.deleteEntry(absTarPath)
                if os.path.exists(absTarPath):
                    self.finishCurrentWorkerEntry(False)
                else:
                    print('III')
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
#            success = static_functions.copyEntry(absSrcPath, absTarPath) if not absTarPath == self.target_path else static_functions.createDirectory(absTarPath)
            self.finishCurrentWorkerEntry(success)
            
#    def finishCurrentWorkerEntry(self, success):
#        worker = self.crntly_prcsd_wrkr
#        if not worker:
#            raise Exception('finishCurrentWorkerEntry - self.crntly_prcsd_wrkr is None!!!')
#        worker.setFinished(success)
#        self.finishedWorkers.append( worker)
#        self.postProcessWorker(worker)
#        if not success and worker.abortIfFailed():
#            self.__cancel()
#        else:
#            self.processNextWorkerEntry()
#    
#    def processNextWorkerEntry(self):
#        if self._cancelled:
#            self.finish()
#            
#        self.incrementProgress()
#        
#        if self.crntly_prcsd_wrkr:
#            self.crntly_prcsd_wrkr = self.crntly_prcsd_wrkr.getNext()
#             # returns the next worker if avaliable, else None!
#            
#        if not self.crntly_prcsd_wrkr:
#            for worker in self.workers:
#                if worker not in self.finishedWorkers:
#                    self.crntly_prcsd_wrkr = worker
#                    break
#        
#        if self.crntly_prcsd_wrkr:
#            msg = "copying '%s'" % self.crntly_prcsd_wrkr.entryName
#            wx.PostEvent(self.progress_dialog, DialgCrtr.DialogRequestEvent(EVT_MESSAGE_ID, data=msg))
#            self.copyCurrentWorkerEntry()
#        else:
#            self.finish()
#            
#    def finish(self):
#        print('CopyFiles.finish - ', self._cancelled)
#        totalSuccess = not self._cancelled
#        if totalSuccess:
#            for worker in self.workers:
#                totalSuccess = worker.evalSuccess() and totalSuccess
#                if not totalSuccess:
#                    break
#        wx.PostEvent(self.progress_dialog, DialgCrtr.DialogRequestEvent(EVT_FINISH_ID, data=totalSuccess))
#        
#        
#    def incrementProgress(self):
#        self.entriesAlrProcessed += 1
#        self.updateProgress()
#        
#    def updateProgress(self):
#        if self.progress_dialog and self.entriesToProcess > 0:
#            progr = self.entriesAlrProcessed / self.entriesToProcess * 100
#            wx.PostEvent(self.progress_dialog, DialgCrtr.DialogRequestEvent(EVT_PROGRESS_ID, data=progr))
##            self.progress_dialog.updateGauge(progr)
#            
#    def updateCurrentlyProcessedEntry(self, absPath):
#        if absPath and self.progress_dialog:
#            entryName = os.path.basename(absPath)
#            if entryName:
#                wx.PostEvent(self.progress_dialog, DialgCrtr.DialogRequestEvent(EVT_MESSAGE_ID, data=entryName))
#            else:
#                wx.PostEvent(self.progress_dialog, DialgCrtr.DialogRequestEvent(EVT_MESSAGE_ID, data=absPath))
      
                
        
#-----------------------------------------


def main():
    parser = argparse.ArgumentParser()
#    parser.add_argument("-b", "--base_dir", help="base-target-directory: destination folder to copy files to")
    parser.add_argument("-s", "--sources", help="source paths of the files to copy", nargs="*", required=True)
    parser.add_argument("-t", "--target", help="target path", required=True)
    args = parser.parse_args()
        
    if args.sources is not None:
        print('sources are: ', args.sources)
    else:
        print('sources not set')
                
    if args.target is not None:
        print('target are: ', args.target)
    else:
        print('target not set')
    
    sources = set(processPaths(args.sources))
    target  = trimPathSeparator(args.target)
    
    app = wx.App()
    
    # parent-folder must not be copied into sub-folder
    sources = checkForSelfContaintingFiles(sources, target)
    
    
    print('sources: ', sources)
    print('target: ', target)
    
    pf = ProgressFrame(None, title='Copy Files')
    
    worker = CopyFiles(sources, target, pf)
    pf.setWorker(worker)
    
    pf.Show()
    
#    worker.copyFilesThreaded()

    app.MainLoop()
    


if __name__ == '__main__':
    main()