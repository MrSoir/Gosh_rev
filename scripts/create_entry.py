#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Created on Mon Mar  4 20:30:02 2019

@author: hippo
"""

import os.path

import static_functions

from valid_entry_name_dialog import ENTRY_TYPE
 

def CreateEntry(tarDir, init_entry_name, creation_type):
    entry_name = init_entry_name
    
    if creation_type == ENTRY_TYPE.DIRECTORY:
        validEntryName = static_functions.askForValidDirectoryName(tarDir, entry_name)
    else:
        validEntryName = static_functions.askForValidFileName(tarDir, entry_name)

    if validEntryName:
        func = static_functions.createFile if creation_type == ENTRY_TYPE.FILE else static_functions.createDirectory
        success = func( os.path.join(tarDir, validEntryName) )
        print('created successfully!' if success else 'creation failed!')
    else:
        print('creation failed!')