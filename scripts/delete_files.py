#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Created on Wed Feb 27 13:22:24 2019

@author: hippo
"""

import os.path
import argparse
import time

import static_functions

from path_func import processPaths

from worker import WorkerDirEntry, Worker

import wx
from progress_dialog_widget import ProgressFrame

class DeleteFiles(Worker):
    def __init__(self, source_paths, progress_dialog=None):
        super().__init__(progress_dialog=progress_dialog)
        print('in DeleteFiles-constructor!')
        
        for source_path in source_paths:
            worker = WorkerDirEntry.createWorkerDirFromPath(source_path, processFunc=lambda absSrcPath, absTarPath: static_functions.deleteEntry(absSrcPath, recursive=False) )
#            worker = WorkerDirEntry.createWorkerDirFromPath(source_path, processFunc=self.deleteEntry_lazy)
            worker.setBottomUp()
            worker.setTarMustNotExist(False)
            self.workers.append( worker )

        self._post__init__()
    
    def deleteEntry_lazy(self, absSrcPath, absTarPath):
        time.sleep(2)
        return static_functions.deleteEntry(absSrcPath, recursive=False)

#------------------------------------------------


def main():
    parser = argparse.ArgumentParser()
#    parser.add_argument("-b", "--base_dir", help="base-target-directory: destination folder to copy files to")
    parser.add_argument("-s", "--sources", help="source paths of the files to duplicate", nargs="*", required=True)
    args = parser.parse_args()
        
    if args.sources is not None:
        print('sources are: ', args.sources)
    else:
        print('sources not set')
    
    sources = set(processPaths(args.sources))
    
    app = wx.App()
    
    print('sources: ', sources)
    
    pf = ProgressFrame(None, title='Duplicate Files')
    
    worker = DeleteFiles(sources, pf)
    pf.setWorker(worker)
    
    pf.Show()
    
    worker.executeThreaded()

    app.MainLoop()
    


if __name__ == '__main__':
    main()