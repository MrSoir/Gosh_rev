#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Created on Sat Dec 29 21:05:29 2018

@author: hippo
"""


import os
import os.path
import zipfile

def getEntries(absp):
    entries = list()
    for baseDir, sub_dirs, files in os.walk(absp):     
        for sub_dir in sub_dirs:
            entries.append(os.path.join(baseDir, sub_dir))
        for file in files:
            entries.append(os.path.join(baseDir, file))
    return entries

def zipEntries(tarZipFilePath, srcToZip, srcBaseDir):
    with zipfile.ZipFile(tarZipFilePath, "w") as zf:
        for src in srcToZip:
            addEntryToZipArchive(zf, src, srcBaseDir)
            
def addEntryToZipArchive(zip_archive, srcToZip, srcBaseDir):
    try:
        relPath =  os.path.relpath(srcToZip, srcBaseDir)
        zip_archive.write(srcToZip, arcname=relPath)
        return True
    except:
        return False

def iterArchive(zip_archive):
    with zipfile.ZipFile(zip_archive, 'r') as zf:
        entries = zf.infolist()
        for e in entries:
            print(e.filename)
    

zip_srcFilesDir = '/home/hippo/Documents/tests_tar/extraction_dir/fold'
zip_base        = zip_srcFilesDir#'/home/hippo/Documents/tests_tar'
zip_tarDir = '/home/hippo/Documents/tests_src/zipped_files'
zip_name = 'test_zip.zip'
absZipPath = os.path.join(zip_tarDir, zip_name)

entriesToZip = getEntries(zip_srcFilesDir)

#zipEntries(absZipPath, entriesToZip, srcBaseDir=zip_base)
iterArchive('/home/hippo/Documents/tests_src/zipped_files/test_zip.zip')



