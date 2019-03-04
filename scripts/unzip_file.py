#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Created on Wed Feb 20 13:57:02 2019

@author: hippo
"""

import wx
import os.path
import os
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
from path_func import getFilePathWithoutExtension

from target_dir_creator import *

from password_dialog import PasswordDialog

import zipfile

class UnZipFiles(Worker):
    def __init__(self, absTarZipFilePath, tarExtractDir=None, pwd=None, progress_dialog=None):
        super().__init__(progress_dialog)
        print('in UnZipFiles-constructor!')
        
        self.absTarZipFilePath = absTarZipFilePath
        # folgende zeile bewirkt, dass die archive-datei in einem unterornder mit demselben namen wie die archive-datei
        # (aber ohne file-extension) erstellt wird, falls tarExtractDir=None:
        self.tarExtractDir = tarExtractDir if tarExtractDir else getFilePathWithoutExtension(absTarZipFilePath)

        self.pswrd = pwd
        self.isPwdProtected = False
        
        if not self.createTarDirIfNotExistent() or not self.openArchive():
            self.cancel()
        else:
            self.checkForPassword()
            self.generateWorkers()
                        
        self._post__init__()
        
        self.name = "UnZipFiles"
        
        
    def createTarDirIfNotExistent(self):
        if os.path.exists(self.tarExtractDir):
            return True
        
        baseDir = os.path.dirname(self.tarExtractDir)
        while not os.path.exists(baseDir) and baseDir:
            newBaseDir = os.path.dirname(baseDir)
            if newBaseDir == baseDir:
                return False
            baseDir = newBaseDir
            
        if baseDir:
            relPath = os.path.relpath(self.tarExtractDir, baseDir)
            relPathFrgmnts = relPath.split(os.sep)
            tarDir = None
            success = True
            for frgmnt in relPathFrgmnts:
                tarDir = os.path.join(tarDir, frgmnt) if tarDir else os.path.join(baseDir, frgmnt)
                success = static_functions.createDirectory(tarDir) and success
                if not success:
                    return False
            return True
        else:
            return False
    
    def checkForPassword(self):
        self.isPwdProtected = self.checkIfAnyEntryInArchiveIsPasswordProtected()
        if self.isPwdProtected and not self.pswrd:
            self.askForPassword()
            
    def askForPassword(self):
        dlg = PasswordDialog(parent=self.progress_dialog)
        rslt = dlg.ShowModal()
        if rslt == wx.ID_CANCEL:
            self.cancel()
        elif rslt == wx.ID_OK:
            self.pswrd = dlg.getPassword()
            self.zf.setpassword(self.pswrd)
        dlg.Destroy()
    
    def openArchive(self):
        try:
            self.zf = zipfile.ZipFile(self.absTarZipFilePath, 'r')
            if self.pswrd:
                self.zf.setpassword(self.pswrd)
            return True
        except Exception as e:
            return False
        
    def getArchiveEntries(self):
        return self.zf.infolist()
    
    def generateWorkers(self):
        self.normalizedZipEntryPathsToRealPathName = dict()
        
        relPathToWorker = dict()
        
        for zip_info in self.getArchiveEntries():
            archEntryPath = zip_info.filename
            relArchPath = trimPathSeparator(archEntryPath)
            baseDir  = os.path.dirname(relArchPath)
            entryName = os.path.basename(relArchPath)
            tarBaseDir = os.path.join(self.tarExtractDir, baseDir)
            tarEntryName = entryName
            absRealZipPath = os.path.join(baseDir, entryName)
            self.normalizedZipEntryPathsToRealPathName[absRealZipPath] = archEntryPath
            
            worker = None
            parentWorker = self.getParentWorker(baseDir, absRealZipPath)
            if parentWorker:
                worker = WorkerDirEntry(entryName=entryName, baseDir=None,\
                                        tarEntryName=tarEntryName, tarBaseDir=None,\
                                        parentWorkerDirEntry=parentWorker)
                if zip_info.is_dir():
                    parentWorker.sub_workerDirEntries.append(worker)
                else:
                    parentWorker.sub_workerFileEntries.append(worker)
            else:
                worker = WorkerDirEntry(entryName=entryName, baseDir=baseDir,\
                                        tarEntryName=tarEntryName, tarBaseDir=tarBaseDir)
                self.workers.append(worker)
            
            worker.setProcessFunc(self.extractArchiveEntry)
                
            worker.setSrcMustExist(False)
            
            relPathToWorker[absRealZipPath] = worker
            
#        for worker in self.workers:
#            self.printWorker(worker)
            
    def getParentWorker(self, baseDir, absPath):
        for worker in self.workers:
            w = self.getParentWorker_hlpr(worker, baseDir, absPath)
            if w:
                return w
        return None
    def getParentWorker_hlpr(self, worker, baseDir, absPath):
        if worker.getAbsSrcPath() == baseDir:
            return worker
        for sub_dir in worker.sub_workerDirEntries:
            w = self.getParentWorker_hlpr(sub_dir, baseDir, absPath)
            if w:
                return w
        return None
        
    def printWorker(self, worker, indent='\t'):
        srcPath = os.path.join(worker.baseDir, worker.entryName) if worker.baseDir else worker.entryName
        tarPath = os.path.join(worker.tarBaseDir, worker.tarEntryName) if worker.tarBaseDir else worker.tarEntryName
        print(indent, 'src: ', srcPath, '     tar: ', tarPath)
        for sub_dir in worker.sub_workerDirEntries:
            self.printWorker(sub_dir, indent+'\t')
        for file in worker.sub_workerFileEntries:
            self.printWorker(file, indent+'\t')
        
    def archiveEntryIsDir(self, relArchivePath):
        try:
            normalizedArchivePath = self.normalizedZipEntryPathsToRealPathName(relArchivePath)
            return self.zf.getinfo(normalizedArchivePath).is_dir()
        except:
            return False

    def extractArchiveEntry(self, relZipArchiveEntryPath, absTarExtractionPath):
        # absTarExtractionPath ist schrott....
#        time.sleep(2)
        try:
            normalizedPath = self.getRealArchivePath(relZipArchiveEntryPath)
            zip_info = self.zf.getinfo(normalizedPath)
            if zip_info.is_dir():
                return self.createDir(absTarExtractionPath)
            else:
                return self.writeToFile(relZipArchiveEntryPath, absTarExtractionPath)
        except:
            return False
    
    def writeToFile(self, relZipArchiveEntryPath, absTarExtractionPath):
        try:
            byte_size = 10000
            normalizedPath = self.getRealArchivePath(relZipArchiveEntryPath)
            with self.zf.open(normalizedPath, 'r', pwd= self.pswrd) as srcs:
                with open(absTarExtractionPath, 'wb') as tars:
                    byte = srcs.read(byte_size)
                    while byte:
                        tars.write( byte )
                        byte = srcs.read(byte_size)
            return True
        except:
            return False
    
    def getRealArchivePath(self, normalizedArchivePath):
        return self.normalizedZipEntryPathsToRealPathName[normalizedArchivePath]
    
    def createDir(self, absTarDirPath):
        relPath = os.path.relpath(absTarDirPath, self.tarExtractDir)
        relPathFrgmnts = relPath.split(os.sep)
        tarPath = None
        success = True
        for dir_frmngt in relPathFrgmnts:
            tarPath = os.path.join(tarPath, dir_frmngt) if tarPath else os.path.join(self.tarExtractDir, dir_frmngt)
            if not os.path.exists(tarPath):
                success = static_functions.createDirectory(tarPath) and success
                if not success:
                    break
        return success
        
    def checkIfArchiveEntryIsPasswordProtected(self, relZipArchiveEntryPath):
        try:
            zip_info = self.zf.getinfo(relZipArchiveEntryPath)
            is_encrypted = zip_info.flag_bits & 0x1 
            return is_encrypted
        except:
            return False
    
    def checkIfAnyEntryInArchiveIsPasswordProtected(self):
        try:
            self.zf.setpassword(None)
            self.zf.testzip()
            if self.pswrd:
                self.zf.setpassword(self.pswrd)
            return False
        except RuntimeError as e:
            if 'encrypted' in str(e):
                return True
            else:
                return False
        
    def tidyUpBeforeFinish(self):
        try:
            self.zf.close()
        except:
            pass
        
        
#-----------------------------------------


def main():
    parser = argparse.ArgumentParser()
#    parser.add_argument("-b", "--base_dir", help="base-target-directory: destination folder to copy files to")
    parser.add_argument("-s", "--source", help="path to the archive that will be extracted", required=True)
    parser.add_argument("-t", "--target", help="target dir: the files will be extracted to this directory", required=False)
    args = parser.parse_args()
        
    if args.source is not None:
        print('source is: ', args.source)
    else:
        print('sources not set')
                
    if args.target is not None:
        print('target are: ', args.target)
    else:
        print('target not set')
    
    source = trimPathSeparator(args.source)
    target = trimPathSeparator(args.target) if args.target else None
    
    app = wx.App()
    
    # hier nicht wirklich wichtig  wie mir scheint:
#    sources = checkForSelfContaintingFiles(sources, target)
    
    print('source: ', source)
    print('target: ', target)
    
    pf = ProgressFrame(None, title='Copy Files')
    
    worker = UnZipFiles(source, target, progress_dialog=pf)
    pf.setWorker(worker)
    
    pf.Show()
    
    worker.executeThreaded()

    app.MainLoop()
    


if __name__ == '__main__':
    main()