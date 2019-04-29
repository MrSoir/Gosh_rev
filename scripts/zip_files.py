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

from valid_entry_name_dialog import ValidEntryNameDialog, ENTRY_TYPE

import threadsafeDialogCreator as DialgCrtr
from   threadsafeDialogCreator import EVT_DIALOG_REQUEST

from progress_dialog_widget import ProgressFrame, EVT_MESSAGE_ID, EVT_PROGRESS_ID, EVT_FINISH_ID

from worker import Worker, WorkerDirEntry

from path_func import *

from target_dir_creator import *

import zipfile

class ZipFiles(Worker):
    def __init__(self, source_paths, absTarZipFilePath, progress_dialog=None):
        super().__init__(progress_dialog)
        print('in ZipFiles-constructor!')
        
        self.absTarZipFilePath = absTarZipFilePath
        
        tarZipFileBaseDir = os.path.dirname(absTarZipFilePath)
        
        self.absTarZipFilePath = absTarZipFilePath

        validZipFileName = self.askForValidZipFilePath();
        if not validZipFileName:
            self.cancel()
            return
        self.absTarZipFilePath = os.path.join(tarZipFileBaseDir, validZipFileName)
        
        zipCreatorWorker = WorkerDirEntry.createWorkerDirFromPath(absTarZipFilePath,\
                                                                  tarBaseDir=tarZipFileBaseDir,\
                                                                  tarEntryName=validZipFileName,\
                                                                  processFunc=lambda absSrcPath,\
                                                                  absTarPath: self.createArchive(absTarPath),\
                                                                  recursive=False)
        zipCreatorWorker.setSrcMustExist(False)
        self.workers.append( zipCreatorWorker )

        self.srcBaseDirs = dict()
        for source_path in source_paths:
            self.createWorkerFromSrcPath(source_path)
                
        self._post__init__()
        
        self.name = "ZipFiles"
        
    def askForValidZipFilePath(self):
        base_dir = os.path.dirname(self.absTarZipFilePath)
        init_entry_name = os.path.basename(self.absTarZipFilePath)
        dlg = ValidEntryNameDialog(init_entry_name, base_dir, parent=self.progress_dialog, entry_type=ENTRY_TYPE.FILE)
        dlg.setMessage('please select a Zip-filename:')
        result = dlg.ShowModal()
        if result == wx.ID_OK:
            selectedEntryName = dlg.getText()
            return selectedEntryName
        elif result == wx.ID_CANCEL:
            return None
        else:
            return None
        
    def createWorkerFromSrcPath(self, source_path):
        if os.path.exists(source_path):
            self.createWorkerFromSrcPath_hlpr(source_path)
            if os.path.isdir(source_path):
                for base_dir, sub_dirs, files in os.walk(source_path):
                    for file in files:
                        self.createWorkerFromSrcPath_hlpr(os.path.join(base_dir, file), source_path)
                    for sub_dir in sub_dirs:
                        self.createWorkerFromSrcPath_hlpr(os.path.join(base_dir, sub_dir), source_path)
        else:
            print("ZipFiles::createWorkerFromSrcPath - source_path '%s' does not exist!" % source_path)
                    
    def createWorkerFromSrcPath_hlpr(self, source_path, baseDir=None):
        if not baseDir:
            baseDir = source_path
        zipEntryWorker = WorkerDirEntry.createWorkerDirFromPath(source_path,\
                                                                processFunc=lambda absSrcPath, absTarPath: self.addEntryToZipArchive(absSrcPath),\
                                                                recursive=False)
        zipEntryWorker.setTarMustNotExist(False)
        self.workers.append( zipEntryWorker )
        self.srcBaseDirs[source_path] = os.path.dirname(baseDir)
        
    def tidyUpBeforeFinish(self):
        try:
            self.zf.close()
        except:
            pass
        
    def createArchive(self, absTarZipFilePath):
#        time.sleep(2)
        self.absTarZipFilePath = absTarZipFilePath
        try:
            self.zf = zipfile.ZipFile(absTarZipFilePath, "w")
            return True
        except:
            self.cancel()
            return False
        
    def addEntryToZipArchive(self, srcToZip):
#        time.sleep(2)
        try:
            srcBasePath =  self.srcBaseDirs[srcToZip]
            relPath = os.path.relpath(srcToZip, srcBasePath)
            self.zf.write(srcToZip, arcname=relPath)
            return True
        except:
            return False
        
        
#-----------------------------------------


def main():
    parser = argparse.ArgumentParser()
#    parser.add_argument("-b", "--base_dir", help="base-target-directory: destination folder to copy files to")
    parser.add_argument("-s", "--sources", help="source paths of the files to zip", nargs="*", required=True)
    parser.add_argument("-t", "--target", help="target zip file path", required=True)
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
    
    # hier nicht wirklich wichtig  wie mir scheint:
#    sources = checkForSelfContaintingFiles(sources, target)
    
    print('sources: ', sources)
    print('target: ', target)
    
    pf = ProgressFrame(None, title='Zip Files')
    
    worker = ZipFiles(sources, target, pf)
    if worker.cancelled():
        return
    pf.setWorker(worker)
    
    pf.Show()
    
    worker.executeThreaded()

    app.MainLoop()
    


if __name__ == '__main__':
    main()