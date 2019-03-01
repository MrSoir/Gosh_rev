#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Created on Wed Feb 20 21:56:36 2019

@author: hippo
"""

import os.path

def invalidWindowsCharacters():
    return ('<', '>', ':', '"', '\\', '/', '|', '*')
def invalidWindowsFileNames():
    return ('CON', 'PRN', 'AUX', 'NUL', 'COM1', 'COM2', 'COM3', 'COM4', 'COM5', 'COM6', 'COM7', 'COM8', 'COM9', 'LPT1', 'LPT2', 'LPT3', 'LPT4', 'LPT5', 'LPT6', 'LPT7', 'LPT8', 'LPT9')

#----------------------------------------
    # validation functions:

def isValidFileName(base_dir, file_name):
    # wenns kein valider dir_name ist, ist es auch kein valider file_name!:
    if not isValidDirectoryName(base_dir, file_name):
        return False
        
    if file_name in invalidWindowsFileNames():
        return False
    return True

def isValidDirectoryName(base_dir, dir_name):
    if not dir_name:
        return False
    
    dir_name = dir_name.strip()
    if not dir_name:
        return False
    
    if os.path.exists(os.path.join(base_dir, dir_name)):
        return False
    
    for invalid_char in invalidWindowsCharacters():
        if invalid_char in dir_name:
            return False
    return True

#--------------------------------
    # Warning message functions:

def invalidFileNameWarning(basename, file_name):
    if not file_name or not file_name.strip():
        return "file name must not be empty!"
        
    if os.path.exists(os.path.join(basename, file_name)):
        return "'%s' does already exist!" % file_name
    
    return 'invalid file name - please select another name!'

def invalidDirectoryNameWarning(basename, dir_name):
    if not dir_name or not dir_name.strip():
        return "folder name must not be empty!"
        
    if os.path.exists(os.path.join(basename, dir_name)):
        return "'%s' does already exist!" % dir_name
    
    return 'invalid folder name - please select another name!'