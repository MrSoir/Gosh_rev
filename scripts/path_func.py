#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Created on Wed Feb 27 14:03:55 2019

@author: hippo
"""

import os.path
import os
from pathlib import PurePath
from functools import reduce
import operator

def getExtension(path):
    pp = PurePath(path)
    exs = pp.suffixes
    ex_concat = reduce(operator.add, exs) if exs else ''
    base_dir   = os.path.dirname(path)
    entry_name = os.path.basename(path)
    entry_name_we = entry_name[:len(entry_name) - len(ex_concat)]
    return (base_dir, entry_name_we, ex_concat)

def getFileNameWithoutExtension(path):
    return getExtension(path)[1]

def getFilePathWithoutExtension(path):
    base_dir, file_name, ext = getExtension(path)
    return os.path.join(base_dir, file_name)

def generateUniquePath(path):
    baseDir, entry_name, ext = getExtension(path)
    cntr = 2
    maninp_entry_name = entry_name
    while os.path.exists(os.path.join(baseDir, maninp_entry_name + ext)):
        maninp_entry_name = entry_name + '_(' + str(cntr) + ')'
        cntr += 1
    return (baseDir, maninp_entry_name + ext)


#--------------------------------------

# copy_files & duplicate_files:
    
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