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
from concurrent.futures import ProcessPoolExecutor, ThreadPoolExecutor

import static_functions
from static_functions import UserAnswer

from progress_dialog_widget import ProgressFrame

from worker import Worker, WorkerDirEntry


class CopyFiles(Worker):
    def __init__(self, source_paths, target_path, progress_dialog=None):
        super().__init__(progress_dialog)
        
        self.source_paths = list()
        for source_path in source_paths:
            self.source_paths.append( WorkerDirEntry.createWorkerDirFromPath(source_path) )
            
        self.target_path = target_path
        self.__cancelled = False
                
    def executeThreaded(self):
        print('executeThreaded')
        self.copyFilesThreaded()
            
    def copyFilesThreaded(self):
        print('copyFilesThreaded')
        self.executor = ThreadPoolExecutor(max_workers=4)
        self.executor.submit(self.copyFiles)
    
    def copyFiles(self):
        print('copyFiles - starting to copy files...')
        success = self.copyFiles_hlpr()
        
        print('copyFiles - success: ', success)
        
        if self.progress_dialog:
            wx.CallAfter(self.progress_dialog.close)
            
        
            
        if self.__cancelled:
            return False
        return success

    def copyFiles_hlpr(self):
        print('in copyFiles_hlpr...')
        self.updateProgDialLabel('process source files...')
        self.entriesToProcess = static_functions.EntryCount(self.source_paths)
        print('in copyFiles_hlpr 2...')
        
        # wenn der targetDirPath bereits existiert, dann:
        #       1. user fragen ob der ordner geloescht werden soll oder
        #       2. user nach einem neuen, nicht-existierenden ordnernamen fragen:
        if os.path.exists(self.target_path):
            tarDirName = static_functions.evalValidDirectoryName(self.target_path, self.progress_dialog)
            if tarDirName is UserAnswer.CANCEL:
                self.__cancelled = True
                return False
            elif tarDirName:
                tarBaseDir = os.path.dirname(self.target_path)
                if tarBaseDir:
                    self.target_path = os.path.join(tarBaseDir, tarDirName)
        
        print('in copyFiles_hlpr 3...')
        # self.target_path duefte jetzt nicht existieren (entweder hat ers nie ober user hat ihn loeschen lassen oder user hat 
        # einen neuen zielordnernamen ausgewaehlt):
        if not os.path.exists(self.target_path):
            os.mkdir(self.target_path)
            if not os.path.exists(self.target_path):
                print("copyFiles_hlpr - target_path: '%s' does not exist and could not be created!" % self.target_path)
                return False
        
        print('in copyFiles_hlpr 4...')
        totalSuccess = True
        for src_pth in self.source_paths: 
            self.updateCurrentlyProcessedEntry(src_pth)
            # copy file:
            if os.path.isfile(src_pth):
                totalSuccess = self.copyFile(src_pth) and totalSuccess
                self.incrementProgress()
            # deep-copy folder:
            elif os.path.isdir(src_pth):
                totalSuccess = self.copyDirectoryRec(src_pth) and totalSuccess
                
            if self.__cancelled:
                return False
        print('in copyFiles_hlpr 5...')
        return totalSuccess
    
    def copyFile(self, absSrcFilePath):
        time.sleep(2)
        if not os.path.exists(absSrcFilePath):
            return False
        
        fileName = os.path.basename(absSrcFilePath)
        if not fileName:
            return False
        copiedFileSuccessfully = static_functions.copyFileAndAskUserIfAlreadyExists(absSrcFilePath, self.target_path, fileName, self.progress_dialog)
        if copiedFileSuccessfully is UserAnswer.CANCEL:
            self.__cancelled = True
        return copiedFileSuccessfully
    
    
    def copyDirectoryRec(self, srcDirPth):
        time.sleep(2)
        baseDirTargetPth = self.target_path
        
        if not os.path.exists(baseDirTargetPth):
            print("copyDirectoryRec - baseDirTargetPth: '%s' does not exist!" % baseDirTargetPth)
            return False
        
        dir_name = os.path.basename(srcDirPth)
        tar_dir_name = dir_name

        absTarDirPath = os.path.join(baseDirTargetPth, tar_dir_name)
        
        valid_tar_dirName = static_functions.evalValidSubDirName(baseDirTargetPth, tar_dir_name, self.progress_dialog)
        
        if valid_tar_dirName == UserAnswer.CANCEL:
            self.__cancelled = True
            return False
        elif valid_tar_dirName:
            absTarDirPath = os.path.join(baseDirTargetPth, valid_tar_dirName)
            
            if os.path.exists(absTarDirPath):
                return False
            else:
                os.mkdir(absTarDirPath)
                self.incrementProgress()
                
                if not os.path.exists(absTarDirPath):
                    return False
                
                return self.copyDirectoryRec_hlpr(srcDirPth, absTarDirPath)
        else:
            return False
        
        # fallback-option:
        return False
    
    def copyDirectoryRec_hlpr(self, srcDirPath, baseDirTargetPth):
        time.sleep(2)
        self.updateCurrentlyProcessedEntry(srcDirPath)
        
        (dirpath, dirnames, filenames) = static_functions.getEntriesInDirectory(srcDirPath)
        successfully_created_subDirs = dict()
        totalSuccess = True
        for sub_dir in dirnames:
            sub_dir = static_functions.evalValidSubDirName(baseDirTargetPth, sub_dir, self.progress_dialog)
            if sub_dir is UserAnswer.CANCEL:
                self.__cancelled = True
                return False
            elif sub_dir:
                tarSubDirPath = os.path.join(baseDirTargetPth, sub_dir)
                successfully_created_subDirs[sub_dir] = static_functions.createDirectory(tarSubDirPath)
                totalSuccess = successfully_created_subDirs[sub_dir] and totalSuccess
                self.incrementProgress()
        for fn in filenames:
            absSrcFilePath = os.path.join(dirpath, fn)
            self.updateCurrentlyProcessedEntry(absSrcFilePath)
            
            copiesFileSuccessfully = static_functions.copyFileAndAskUserIfAlreadyExists(absSrcFilePath, baseDirTargetPth, fn, self.progress_dialog)
            if copiesFileSuccessfully is UserAnswer.CANCEL:
                self.__cancelled = True
                return False
            totalSuccess = copiesFileSuccessfully and totalSuccess
            self.incrementProgress()
            
        for subdir in successfully_created_subDirs:
            if successfully_created_subDirs[subdir]:
                absSrcSubDirPath = os.path.join(srcDirPath, sub_dir)
                absTarSubDirPath = os.path.join(baseDirTargetPth, sub_dir)
                totalSuccess = self.copyDirectoryRec_hlpr(absSrcSubDirPath, absTarSubDirPath) and totalSuccess
                
        return totalSuccess
            
    def incrementProgress(self):
        self.entriesAlrProcessed += 1
        self.updateProgress()
        
    def updateProgress(self):
        if self.progress_dialog:
            progr = self.entriesAlrProcessed / self.entriesToProcess
#            self.progress_dialog.updateGauge(progr)
            
    def updateProgDialLabel(self, txt):
        self.setProgLabelThreadSafe(txt)
            
    def updateCurrentlyProcessedEntry(self, absPath):
        if absPath:
            baseName = os.path.basename(absPath)
            self.setProgLabelThreadSafe(baseName)

    def setProgLabelThreadSafe(self, txt):
        if txt and self.progress_dialog:
            pass
#            wx.CallAfter(self.progress_dialog.updateInfoLabel, txt)
                
        
#-----------------------------------------

def createUniqueFilePath(tar_path, file_name):
    cntr = 1
    name, ext = os.path.splitext(file_name)
    
    tar_file_path = os.path.join(tar_path, file_name)
    
    while os.path.exists(tar_file_path):
        tar_file_path = os.path.join(tar_path, name + "_(" + str(cntr) + ")" + ext)
        cntr += 1
    return tar_file_path



def onEntryExistsAskForOverride(path):
    return true

def deleteFolderRec(path):
    pass
def deleteFile(path):
    pass
# /home/hippo/Documents/ -> /home/hippo/Documents -> eleminiert unnoetige path-separator am ende eines file-paths

def trimPathSeparator(pth):
    if pth[-1] == os.path.sep:
        return pth[:-1]
    return pth

def processPaths(paths):
    return [trimPathSeparator(p) for p in paths]

def isParentFolder(parent, sub):
    while len(sub) >= len(parent):
        if sub == parent:
            return True
        sub = os.path.dirname(sub)
    return False

def showWarningDialog(warning_msg):
    warning_dialog = wx.MessageDialog(None, warning_msg, style=wx.ICON_EXCLAMATION)
    warning_dialog.ShowModal()

def checkForSelfContaintingFiles(source_paths, tar_path):
    # checkt, dass ein source_path nicht teil eiens tar_paths ist. denn sonst 
    invalid_paths = set()
    tp = tar_path
    for s in source_paths:
        if isParentFolder(s, tp):
            chr_lmt = 200
            s_msg = "..." + s[-chr_lmt] if len(s) > chr_lmt else s
            t_msg = "..." + tar_path[-chr_lmt:] if len(tar_path) > chr_lmt else tar_path
            warning_msg = "You must not copy a parent folder into itself!\n " \
                        + '"' + s_msg + '"   ->    "' + t_msg + '"'
            showWarningDialog(warning_msg)
            invalid_paths.add(s)
    return source_paths - invalid_paths


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