#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Created on Wed Feb 27 13:22:24 2019

@author: hippo
"""

import os.path
import argparse

import static_functions

from path_func import generateUniquePath, processPaths

from worker import WorkerDirEntry, Worker

import wx
from progress_dialog_widget import ProgressFrame

class DuplicateFiles(Worker):
    def __init__(self, source_paths, progress_dialog=None):
        super().__init__(progress_dialog=progress_dialog)
        print('in DuplicateFiles-constructor!')
        
        for source_path in source_paths:
            base_dir, unique_entry_name = generateUniquePath(source_path)
            absTarPath = os.path.join(base_dir, unique_entry_name)
            if absTarPath != source_path: # passiert, wenn source_path nicht existiert -> absTarPath == source_path - dann soll natuerlich nicht dupliziert werden!
                self.workers.append( WorkerDirEntry.createWorkerDirFromPath(source_path, tarBaseDir=base_dir, tarEntryName=unique_entry_name, processFunc=static_functions.copyEntry) )
                                
        self._post__init__()


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
    
    worker.executeThreaded()

    app.MainLoop()
    


if __name__ == '__main__':
    main()