#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Created on Wed Feb 20 14:17:08 2019

@author: hippo
"""

import wx
import os.path
import time
import threading
from os import sep
import shutil
import sys, traceback
from collections.abc import Iterable
from enum import Enum

import input_dialog
import entry_name_validation

class UserAnswer(Enum):
    OK = 0
    NO = 1
    CANCEL = 2
    
def FileCount(absPaths):
    fileCnt, _ = EntryCount_hlpr(absPaths)
    return fileCnt

def DirCount(absPaths):
    _, dirCnt = EntryCount_hlpr(absPaths)
    return dirCnt
    
def EntryCount(absPaths):
    fileCnt, dirCnt = EntryCount_hlpr(absPaths)
    return fileCnt + dirCnt

def EntryCount_hlpr(absPaths):
    absPaths = [absPaths,] if isinstance(absPaths, str) else absPaths
    fileCount = 0
    dirsCount = 0
    for absPath in absPaths:
        if os.path.isfile(absPath):
            fileCount += 1
        elif os.path.isdir(absPath):
            dirsCount += 1
            for root, dirs, files in os.walk(absPath):
                fileCount += len(files)
                dirsCount += len(dirs)
    return (fileCount, dirsCount)
    

def generateDirIfNotAlrExistent(baseDirPath, absTarDirPath):
    if not os.path.exists(baseDirPath):
        return False
    relPath = os.path.relpath(absTarDirPath, baseDirPath)
    relPathDirs = relPath.split(os.sep)
    
    curTarDir = baseDirPath
    for dir in relPathDirs:
        curTarDir = os.path.join(curTarDir, dir)
        if not os.path.exists(curTarDir):
            success = createDirectory(curTarDir)
            if not success:
                return False
        
def createDirectory(absDirPath):
    try:
        os.mkdir(absDirPath)
        return True
    except:
        return False
    
def getEntriesInDirectory(absDirPath):
    files    = [file    for file    in os.listdir(absDirPath) if os.path.isfile(os.path.join(absDirPath, file))]
    sub_dirs = [sub_dir for sub_dir in os.listdir(absDirPath) if os.path.isdir (os.path.join(absDirPath, sub_dir))]
    return (absDirPath, sub_dirs, files)

def copyFile(absSrcPath, absTarPath):
    if not absSrcPath or not absTarPath:
        return False
    try:
        shutil.copyfile(absSrcPath, absTarPath, follow_symlinks=False) # follow_symlinks=False -> bei symbolic links wird nur ein symbolic link erstellt/kopiert, nicht die ganze datei - denke das ist genau das, was es braucht
        return True
    except:
        return False
    
def copyEntry(absSrcPath, absTarPath):
    time.sleep(2)
    print('\nin copyEntry - thread: ', threading.current_thread().name, '\n')
    if not absSrcPath or not absTarPath:
        return False
    if os.path.isdir(absSrcPath):
        return createDirectory(absTarPath)
    elif os.path.isfile(absSrcPath):
        return copyFile(absSrcPath, absTarPath)
    else:
        return False

#---------------------------------------   

def moveEntry(absSrcPath, absTarPath):
    try:
        os.rename(absSrcPath, absTarPath)
        return not os.path.exists(absSrcPath) and os.path.exists(absTarPath)
    except:
        return False
    

#---------------------------------------
    
def copyFileAndAskUserIfAlreadyExists(absSrcFilePath, baseDirTargetPth, fileName, parent=None):    
    validFileName = evalValidFileName(baseDirTargetPth, fileName, parent)
    if validFileName == UserAnswer.CANCEL:
        return UserAnswer.CANCEL
    elif validFileName:
        absTarFilePath = os.path.join(baseDirTargetPth, validFileName)
        if os.path.exists(absTarFilePath):
            return False
        return copyFile(absSrcFilePath, absTarFilePath)
    return False
    
def showYesNoCancelDialog(message, caption, parent=None):
    dlg =  wx.MessageDialog(parent, message, caption, wx.YES | wx.NO | wx.CANCEL | wx.ICON_QUESTION) 
    		
    dlg_rslt = dlg.ShowModal()
    dlg.Show(False)
    dlg.Destroy()
    return dlg_rslt
  
def showInputDialog(parent, message, caption, initVal):
    dlg = wx.TextEntryDialog(parent, message, caption)
    dlg.SetValue(initVal)
    retVal = dlg.GetValue() if  dlg.ShowModal() == wx.ID_OK else None
    dlg.Show(False)
    dlg.Destroy()
    return retVal

def deleteDir(absDirPath):
    if not absDirPath or absDirPath == os.sep or  (not os.path.isdir(absDirPath)): # Linux-Base_Directory '/' must not be delted!!
        return False
    try:
        for root, dirs, files in os.walk(absDirPath, topdown=False):
            for name in files:
                os.remove(os.path.join(root, name))
            for name in dirs:
                os.rmdir(os.path.join(root, name))
    except:
        return False
    
    os.rmdir(absDirPath)
    
    return not os.path.exists(absDirPath)

def deleteFile(absFilePath):
    if not absFilePath or absFilePath == os.sep:
        return False
    
    try:
        os.remove(absFilePath)
    except:
        return False
    return not os.path.exists(absFilePath)

def deleteEntry(absFilePath):
    if not absFilePath or not os.path.exists(absFilePath):
        return False
    
    if os.path.isfile(absFilePath):
        return deleteFile(absFilePath)
    elif os.path.isdir(absFilePath):
        return deleteDir(absFilePath)
    return False
           
            
def askForValidFileName(base_dir, init_file_name, parent=None):
    return askForValidEntryName(base_dir=base_dir,
                                init_entry_name=init_file_name,
                                isValidEntryNameFunc=entry_name_validation.isValidFileName,
                                invalidEntryNameWarningFunc=entry_name_validation.invalidFileNameWarning,
                                entryType='File',
                                parent=parent)
    
def askForValidDirectoryName(base_dir, init_dir_name, parent=None):
    return askForValidEntryName(base_dir=base_dir,
                                init_entry_name=init_dir_name,
                                isValidEntryNameFunc=entry_name_validation.isValidDirectoryName,
                                invalidEntryNameWarningFunc=entry_name_validation.invalidDirectoryNameWarning,
                                entryType='Folder',
                                parent=parent)
    
def askForValidEntryName(base_dir, init_entry_name, isValidEntryNameFunc, invalidEntryNameWarningFunc, entryType='Folder', parent=None):
    validatorFunc = lambda dir_name: isValidEntryNameFunc(base_dir, dir_name)
    warningFunc   = lambda dir_name: invalidEntryNameWarningFunc(base_dir, dir_name)
    message = 'select a %s name:' % entryType.lower()
    dlg = input_dialog.InputDialog(message=message, init_val=init_entry_name, validatorFunc=validatorFunc, warningFunc=warningFunc, parent=parent, title='Select a %s Name' % entryType)
    result = dlg.ShowModal()
    if result == wx.ID_OK:
        selectedEntryName = dlg.getText()
        return selectedEntryName
    elif result == wx.ID_CANCEL:
        return None
    
def evalValidDirectoryName(absTarDirPath, parent=None):
    dirName = os.path.basename(absTarDirPath)
    tarBaseDir = os.path.dirname(absTarDirPath)
    if dirName and tarBaseDir and os.path.join(tarBaseDir, dirName) == absTarDirPath:
        return evalValidSubDirName(tarBaseDir, dirName, parent)
    print('\n\nretrun false condition!!!\n\n')
    return None

def evalValidSubDirName(tarDir, sub_dir_name, parent=None):
    return evalValidEntryName(tarDir, sub_dir_name, askUserIfHeWantsToReplaceDir, askForValidDirectoryName, deleteDir, parent)


def evalValidFileName(tarDir, fileName, parent=None):
    return evalValidEntryName(tarDir, fileName, askUserIfHeWantsToReplaceFile, askForValidFileName, deleteFile, parent)

def evalValidEntryName(tarDir, entryName, aksForReplacementFunc, askForNewEntryNameFunc, deleteEntryFunc, parent=None):
    absTarEntryPath = os.path.join(tarDir, entryName)
    if not os.path.exists(absTarEntryPath):
        return entryName
    else:
        replace = aksForReplacementFunc(tarDir, entryName, parent)
        if replace == wx.ID_YES: 
            deleteEntryFunc(absTarEntryPath)
            if os.path.exists(absTarEntryPath):
                return None
            else:
                return entryName
        elif replace == wx.ID_NO:
            userSlctedEntryName = askForNewEntryNameFunc(tarDir, entryName, parent)
            return userSlctedEntryName if userSlctedEntryName else UserAnswer.CANCEL
        elif replace == wx.ID_CANCEL:
            print('cancel clicked')
            return UserAnswer.CANCEL
    return None

def askUserIfHeWantsToReplaceDir(tarBaseDir, tarDirName, parent=None):
    message = "'%s' does already exist in directory\n'%s'\ndo you want to replace the folder?" % (tarDirName, tarBaseDir)
    caption = "replace folder?"
    return showYesNoCancelDialog(message, caption, parent)

def askUserIfHeWantsToReplaceFile(tarBaseDir, tarFileName, parent=None):
    message = "'%s' does already exist in directory\n'%s'\ndo you want to replace the file?" % (tarFileName, tarBaseDir)
    caption = "replace file?"
    return showYesNoCancelDialog(message, caption, parent)