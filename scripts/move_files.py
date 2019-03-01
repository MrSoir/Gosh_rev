#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Created on Wed Feb 27 15:22:01 2019

@author: hippo
"""

import os.path
import argparse
from threading import Thread

import static_functions

from path_func import *

from worker import Worker, WorkerDirEntry

from copy_files import CopyFiles

import wx
import wx.lib.newevent as NE

from progress_dialog_widget import ProgressFrame

#---------------------------------------------------------

MoveFileEvent, EVT_MoveFile = NE.NewCommandEvent()

EVT_AFTER_MOVING_EVENT_ID = wx.NewIdRef()
EVT_AFTER_BACKUP_COPYING_EVENT_ID = wx.NewIdRef()


class MoveFiles(Worker):
    def __init__(self, source_paths, target_path, progress_dialog=None):
        super().__init__(progress_dialog=progress_dialog)
        print('in MoveFiles-constructor!')
        
        self.source_paths = source_paths
        self.target_path  = target_path
        
        for source_path in source_paths:
            self.workers.append( WorkerDirEntry.createWorkerDirFromPath(source_path, tarBaseDir=target_path, processFunc=static_functions.moveEntry,recursive=False) )
#            self.workers.append( WorkerDirEntry.createWorkerDirFromPath(source_path, tarBaseDir=target_path, processFunc=lambda src, tar: False,recursive=False) )
        
#        self.callOnFinished = self.callAfterMoving
        self.callOnFinished = lambda totalSuccess: wx.PostEvent(self, MoveFileEvent(EVT_AFTER_MOVING_EVENT_ID, data=totalSuccess))
        
        self.Bind(EVT_MoveFile, self.callAfterMoving,           EVT_AFTER_MOVING_EVENT_ID)
        self.Bind(EVT_MoveFile, self.callAfterCopyBackupWorker, EVT_AFTER_BACKUP_COPYING_EVENT_ID)

        self._post__init__()
        
        self.name = "MoveFiles"
        
    def printWorker(self, worker, indent='\t'):
        print(indent, worker.getAbsSrcPath(), '\tparent: ', 'None' if not worker.parentWorkerDirEntry else worker.parentWorkerDirEntry.entryName)
        for file_worker in worker.sub_workerFileEntries:
#            print('\nfile_worker: ', file_worker, '\n')
            self.printWorker(file_worker, indent+'\ŧ')
        for dir_worker in worker.sub_workerDirEntries:
            self.printWorker(dir_worker, indent+'\ŧ')
            
    def callAfterMoving(self, event):
        totalSuccess = event.data
        print('\ncallAfterMoving\n')
        
        if self._threaded:
            Thread(target=self.callAfterMoving_hlpr, args=(totalSuccess,)).start()
        else:
            self.callAfterMoving_hlpr(totalSuccess)
            
    def callAfterMoving_hlpr(self, totalSuccess):
        if self._cancelled:
            self.finish()
        else:
            self.postMessage('processing...')
            
            failedMovementWorkers = list()
            for fw in self.finishedWorkers:
                succeeded = fw.evalSuccess()
                if not succeeded:
                    fw.evalSubEntries()
                    fw.setProcessFunc(static_functions.copyEntry)
                    fw.reset()
                    failedMovementWorkers.append(fw)

            if len(failedMovementWorkers) > 0:
                print('\nfailedWorkers:')
                for fw in failedMovementWorkers:
                    self.printWorker(fw)
                    print()
                print('threaded: ', self._threaded)
                self.setWorkers(failedMovementWorkers)
#                self.callOnFinished = self.callAfterCopyBackupWorker
                self.callOnFinished = lambda totalSuccess: wx.PostEvent(self, MoveFileEvent(EVT_AFTER_BACKUP_COPYING_EVENT_ID, data=totalSuccess))
                if self._threaded:
                    self.copyFilesThreaded()
                else:
                    self.copyFiles()
            else:
                self.finish()
    
    def callAfterCopyBackupWorker(self, event):
        succeeded = event.data
        if self._threaded:
            Thread(target=self.callAfterCopyBackupWorker_hlpr, args=(succeeded,)).start()
        else:
            self.callAfterCopyBackupWorker_hlpr(succeeded)
            
    def callAfterCopyBackupWorker_hlpr(self, succeeded):
        print('MoveFiles::callAfterCopyBackupWorker_hlpr - succeeded: ', succeeded)
        
        self.resetOnFinishedCaller()
        
        if self._cancelled:
            self.finish()
        else:
            workers_failedToCopy = list()
            workers_failedToDeleteSrc = list()
            for worker in self.workers:
                worker_totalSuccess = worker.evalSuccess()
                if worker_totalSuccess:
                    success = static_functions.deleteEntry(worker.getAbsSrcPath())
                    if not success:
                        workers_failedToDeleteSrc.append(worker)
                else:
                    workers_failedToCopy.append(worker)
                    
            print('\nworkers_failedToCopy.len: ', len(workers_failedToCopy), '    workers_failedToDeleteSrc.len: ', len(workers_failedToDeleteSrc), '\n')

#            if len(workers_failedToCopy) > 0 and len(workers_failedToDeleteSrc) > 0:
#                self.showFailureMessage(workers_failedToCopy, workers_failedToDeleteSrc)
            
            self.finish()
    
    def showFailureMessage(self, workers_failedToCopy, workers_failedToDeleteSrc):
        msg  = ''
        if len(workers_failedToCopy) > 0:
            msg += "The following entries could not be copied to '%s':" % self.target_path
            for i, worker in enumerate(workers_failedToCopy):
                msg += '\nsource: %s\ntarget: %s\n' % (worker.getAbsSrcPath(), worker.getAbsTarPath())
                if i == len(workers_failedToCopy)-1 and len(workers_failedToDeleteSrc):
                    msg += '\n\t------\n'
        if len(workers_failedToDeleteSrc) > 0:
            msg += 'sorry but, the following entries were successfully copied, but their source-paths still exist:'
            for i, worker in enumerate(workers_failedToDeleteSrc):
                msg += '\nsource: %s\ntarget: %s\n' % (worker.getAbsSrcPath(), worker.getAbsTarPath())
        if msg:
            static_functions.showInfoMessage(msg, 'Issues occured while moving entres!', self.progress_dialog)
    

#------------------------------------------------


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
    
    worker = MoveFiles(sources, target, pf)
    pf.setWorker(worker)
    
    pf.Show()
    
    worker.executeThreaded()
#    worker.execute()

    app.MainLoop()
    


if __name__ == '__main__':
    main()