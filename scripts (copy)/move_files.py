#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Created on Wed Feb 27 15:22:01 2019

@author: hippo
"""

import os.path
import argparse

import static_functions

from path_func import *

from worker import WorkerDirEntry

from copy_files import CopyFiles

import wx
from progress_dialog_widget import ProgressFrame

class MoveFiles(CopyFiles):
    def __init__(self, source_paths, target_path, progress_dialog=None):
        super().__init__(source_paths, target_path, progress_dialog=progress_dialog)
        print('in MoveFiles-constructor!')
        
        target_worker = self.workers[0]
        self.workers = list()
        self.workers.append(target_worker)
        for source_path in source_paths:
            self.workers.append( WorkerDirEntry.createWorkerDirFromPath(source_path, tarBaseDir=target_path, processFunc=static_functions.moveEntry) )



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
    
    worker = DuplicateFiles(sources, pf)
    pf.setWorker(worker)
    
    pf.Show()
    
#    worker.copyFilesThreaded()

    app.MainLoop()
    


if __name__ == '__main__':
    main()