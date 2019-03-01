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

from target_dir_creator import *
    
class CopyFiles(Worker):
    def __init__(self, source_paths, target_path, progress_dialog=None):
        super().__init__(progress_dialog)
        print('in CopyFiles-constructor!')
        
        for source_path in source_paths:
            self.workers.append( WorkerDirEntry.createWorkerDirFromPath(source_path, tarBaseDir=target_path, processFunc=static_functions.copyEntry) )
        
        self.target_path = target_path
                
        self._post__init__()
        
        self.name = "CopyFiles"
        
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
    worker.executeThreaded()

    app.MainLoop()
    


if __name__ == '__main__':
    main()