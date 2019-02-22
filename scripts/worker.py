#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Created on Thu Feb 21 13:30:43 2019

@author: hippo
"""

from dataclasses import dataclass, field

import os.path
import os


class Worker(object):
    def __init__(self, progress_dialog = None):
        super().__init__()
        self.progress_dialog = progress_dialog
        self.__cancelled = False
        self.entriesToProcess = 0
        self.entriesAlrProcessed = 0
        
    def cancel(self):
        self.__cancelled = True
        
    def cancelled(self):
        return self.__cancelled
    
    def setProgressDialog(self, progress_dialog):
        self.progress_dialog = progress_dialog
        
    def executeThreaded(self):
        print('Worker.executeThreaded')
        pass
    
@dataclass
class WorkerDirEntry:
    absBaseDir: str
    entryName: str
    parentWorkerWirEntry: object = None
    sub_workerDirEntries: list = field(default_factory=list)
    files: list = field(default_factory=list)
    failed: bool = False
    
    @staticmethod
    def createWorkerDirFromPath(absBaseDir, entryName=''):
        """ createWorkerDirFromPath kann foldengermassen aufgerufen werden:
                1. mit 2 argumenten: absBaseDir + entryName = absPath
                2. mit 1 argument:   absBaseDir = absPath """        
        abd = absBaseDir
        en  = entryName
        absBaseDir = abd if en else os.path.dirname (abd)
        entryName  = en  if en else os.path.basename(abd)
        
        absPath = os.path.join(absBaseDir, entryName)
        
        we = WorkerDirEntry(absBaseDir, entryName)
        
        if os.path.exists(absPath) and os.path.isdir(absPath):
            for entry in os.listdir(absPath):
                absEntryPath = os.path.join(absPath, entry)
                if os.path.isfile(absEntryPath):
                    we.files.append(entry)
                elif os.path.isdir(absEntryPath):
                    sub_workerEntry = WorkerDirEntry.createWorkerDirFromPath(absBaseDir=absPath, entryName=entry)
                    sub_workerEntry.parentWorkerWirEntry = we
                    we.sub_workerDirEntries.append( sub_workerEntry )
                    
        return we
                

    