#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Created on Wed Jan 30 11:41:32 2019

@author: hippo
"""

import numpy as np
from os import listdir
from os.path import dirname, basename, isfile, isdir, join, relpath

base_dir = dirname(__file__)

def getFilesInDir(dir_path, file_extension=''):
    files = set()
    files.update( [join(dir_path, file) for file in listdir(dir_path) if isfile(join(dir_path, file)) and file.endswith(file_extension) and not file.startswith('.')] )
    sub_dirs = [join(dir_path, d) for d in listdir(dir_path) if isdir(d) and not d.startswith('.')]
    for sub_dir in sub_dirs:
        files.update( getFilesInDir(sub_dir, file_extension) )
    return files

def sortFancy(ps):
    dirs = [d for d in ps if '/' in d]
    files = [f for f in ps if '/' not in f]
    l = sorted(files)
    l.extend( sorted(dirs) )
    return l

cpp = getFilesInDir(base_dir, '.cpp')
h   = getFilesInDir(base_dir, '.h')

cpp = sortFancy([relpath(p, base_dir) for p in cpp])
h   = sortFancy([relpath(p, base_dir) for p in h])

for p in cpp:
    print('\t' * 1, p, '\\')
print()

for p in h:
    print('\t' * 1, p, '\\')
    
print()
print('cpp: ', len(cpp))
print('h: ', len(h))