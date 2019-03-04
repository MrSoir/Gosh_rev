#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Created on Mon Mar  4 20:30:02 2019

@author: hippo
"""

import argparse
import wx
from path_func import trimPathSeparator

from create_entry import CreateEntry
from valid_entry_name_dialog import ENTRY_TYPE


def CreateFolder(tarDir):     
     CreateEntry(tarDir, 'new_folder', ENTRY_TYPE.DIRECTORY)
         
         
def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("-t", "--target", help="target dir, where the folder will be created", required=True)
    args = parser.parse_args()
        
    if args.target is not None:
        print('target is: ', args.target)
    else:
        print('target not set')
                
    
    target = trimPathSeparator(args.target)
    
    app = wx.App()    
    
    print('target: ', target)
        
    CreateFolder(target)

    app.MainLoop()
    


if __name__ == '__main__':
    main()