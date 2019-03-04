#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Created on Mon Mar  4 20:30:02 2019

@author: hippo
"""

import os.path
import argparse
import wx
from path_func import trimPathSeparator

import static_functions

def RenameFile(toRenamePath):
     entry_name = os.path.basename(toRenamePath)
     base_dir = os.path.dirname(toRenamePath)
     
     getValidEntryName = static_functions.askForValidEntryName(base_dir, entry_name)
     
     if getValidEntryName:
         success = static_functions.renameEntry(toRenamePath, getValidEntryName)
         print('renamed successfully!' if success else 'renaming failed!')
     else:
         print('renaming failed!')
         
         
def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("-t", "--target", help="target path that will be renamed", required=True)
    args = parser.parse_args()
        
    if args.target is not None:
        print('target is: ', args.target)
    else:
        print('target not set')
                
    
    target = trimPathSeparator(args.target)
    
    app = wx.App()    
    
    print('target: ', target)
        
    RenameFile(target)

    app.MainLoop()
    


if __name__ == '__main__':
    main()