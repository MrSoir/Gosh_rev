#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Created on Sat Feb 23 11:31:10 2019

@author: hippo
"""

import wx
import wx.lib.newevent as NE

import static_functions

DialogRequestEvent, EVT_DIALOG_REQUEST = NE.NewCommandEvent()

# requests:
EVT_ASK_VALID_DIR_NAME_ID   = wx.NewIdRef()
EVT_ASK_VALID_FILE_NAME_ID  = wx.NewIdRef()
EVT_ASK_REPLACE_FILE_ID = wx.NewIdRef()
EVT_ASK_REPLACE_DIR_ID  = wx.NewIdRef()

# responses:
EVT_VALID_DIR_NAME_ID   = wx.NewIdRef()
EVT_VALID_FILE_NAME_ID  = wx.NewIdRef()
EVT_REPLACE_FILE_ID = wx.NewIdRef()
EVT_REPLACE_DIR_ID  = wx.NewIdRef()


class ThreadsafeDialogCreator(wx.EvtHandler):
    
    def __init__(self, evnt_hndlr):
        wx.EvtHandler.__init__(self)
        
        self.evnt_hndlr = evnt_hndlr
        
        self.Bind(EVT_DIALOG_REQUEST, self.askForValidDirName,      id=EVT_ASK_VALID_DIR_NAME_ID)
        self.Bind(EVT_DIALOG_REQUEST, self.askForValidFileName,     id=EVT_ASK_VALID_FILE_NAME_ID)
        self.Bind(EVT_DIALOG_REQUEST, self.askToReplaceFile,        id=EVT_ASK_REPLACE_FILE_ID)
        self.Bind(EVT_DIALOG_REQUEST, self.askToReplaceDirectory,   id=EVT_ASK_REPLACE_DIR_ID)
        
#        EVT_RESULT(self.evnt_hndlr, self.askForValidDirName,    EVT_ASK_VALID_DIR_NAME)
#        EVT_RESULT(self.evnt_hndlr, self.askForValidFileName,   EVT_ASK_VALID_FILE_NAME)
#        EVT_RESULT(self.evnt_hndlr, self.askToReplaceFile,      EVT_ASK_REPLACE_FILE)
#        EVT_RESULT(self.evnt_hndlr, self.askToReplaceDirectory, EVT_ASK_REPLACE_DIR)
        
    def askForValidDirName (self, resultEvnt):#base_dir, tar_dir_name, parent):
        print('askForValidDirName')
        base_dir, tar_dir_name, parent = resultEvnt.data
        validDirName = static_functions.askForValidDirectoryName(base_dir, tar_dir_name, parent)
        self.Emit(validDirName, EVT_VALID_DIR_NAME_ID)
        
    def askForValidFileName(self, resultEvnt):#base_dir, init_file_name, parent):
        print('askForValidFileName')
        base_dir, init_file_name, parent = resultEvnt.data
        validFileName = static_functions.askForValidFileName(base_dir, init_file_name, parent)
        self.Emit(validFileName, EVT_VALID_FILE_NAME_ID)
        
    def askToReplaceFile(self, resultEvnt):#base_dir, tar_file_name, parent):
        print('askToReplaceFile')
        base_dir, tar_file_name, parent = resultEvnt.data
        replace = static_functions.askUserIfHeWantsToReplaceFile(base_dir, tar_file_name, parent)
        self.Emit(replace, EVT_REPLACE_FILE_ID)
    
    def askToReplaceDirectory(self, resultEvnt):#base_dir, tar_dir_name, parent):
        print('askToReplaceDirectory')
        base_dir, tar_dir_name, parent = resultEvnt.data
        replace = static_functions.askUserIfHeWantsToReplaceDir(base_dir, tar_dir_name, parent)
        self.Emit(replace, EVT_REPLACE_DIR_ID)
            
    def Emit(self, value, EVNT_ID):
        wx.PostEvent(self.evnt_hndlr, DialogRequestEvent(EVNT_ID, data=value))
        
        