#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Created on Wed Feb 27 16:01:33 2019

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

class TargetDirCreator(Worker):
    def __init__(self, target_path, progress_dialog=None):
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

        self.finishedWorkers = list()
        
        print('workers: ', len(self.workers))
            
        self.crntly_prcsd_wrkr = self.workers[0] if len(self.workers) > 0 else None
            
        self.target_path = target_path
        self._threaded = False