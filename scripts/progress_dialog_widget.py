#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Created on Wed Dec 26 14:25:07 2018

@author: hippo
"""

import wx
import os.path
#import shutil
#import sys, traceback
#from shutil import copy2
#import argparse
import time
import concurrent.futures
from threadsafeDialogCreator import DialogRequestEvent, EVT_DIALOG_REQUEST

from button import Button, BDButtonEvent, EVT_BDButton, EVT_BDCLICKED_ID
from progress_bar import ProgressBar, ProgBarEvent, EVT_VALUE_CHANGED_ID, EVT_LABEL_CHANGED_ID

EVT_MESSAGE_ID  = wx.NewIdRef()
EVT_PROGRESS_ID = wx.NewIdRef()
EVT_FINISH_ID   = wx.NewIdRef()


class ProgressFrame(wx.Frame):

    def __init__(self, parent, title, worker=None):
        super(ProgressFrame, self).__init__(parent, title=title,size=(450, 110),style=wx.DEFAULT_FRAME_STYLE & wx.BORDER_SIMPLE & ~(wx.RESIZE_BORDER | wx.MAXIMIZE_BOX))
        self.Centre()
        self.SetIcon(wx.Icon(getPicturePath('MrSoirIcon_cursor.png')))
        
        self.FONT_NAME = 'Samantana'
        self.SetFont(wx.Font(15,wx.FONTFAMILY_MODERN,wx.FONTSTYLE_NORMAL,wx.FONTWEIGHT_NORMAL, faceName=self.FONT_NAME)) 
        
        self.worker = worker
        
        #panel = wx.Panel(self, wx.ID_ANY)
        
        vbox = wx.BoxSizer(wx.VERTICAL)
        self.SetSizer(vbox)
        
        self.progress_bar = ProgressBar(self, label="processing...", size=(60,60))
        vbox.Add(self.progress_bar, 0, wx.EXPAND)
        
        self.cncl_btn = Button(self, label="cancel", size=(250, 40))
        vbox.Add(self.cncl_btn, 0, wx.ALIGN_CENTER_HORIZONTAL)
        self.SetBackgroundColour(wx.Colour(0,0,0))
        
        self.Bind(EVT_DIALOG_REQUEST, self.updateInfoLabel, id=EVT_MESSAGE_ID)
        self.Bind(EVT_DIALOG_REQUEST, self.updateGauge,     id=EVT_PROGRESS_ID)
        self.Bind(EVT_DIALOG_REQUEST, self.finish,          id=EVT_FINISH_ID)
        
        self.Bind(EVT_BDButton, self.OnCancelClicked, id=EVT_BDCLICKED_ID)
        
        self.SetBackgroundColour((0,0,0))
        
        vbox.FitInside(self)
        
        self._cancelled = False
        
    def setWorker(self, worker):
        self.worker = worker
        
    def updateGauge(self, resultEvnt=None):
        if resultEvnt and resultEvnt.data:
            print('updateGauge: %s' % resultEvnt.data)
            wx.PostEvent(self.progress_bar, ProgBarEvent(EVT_VALUE_CHANGED_ID, data=resultEvnt.data))
#            self.progress_bar.SetValue(resultEvnt.data)
        
    def incrementGauge(self):
        wx.PostEvent(self.progress_bar, ProgBarEvent(EVT_VALUE_CHANGED_ID, data=self.progress_bar.GetValue() + 1))
#        self.progress_bar.SetValue(self.progress_bar.GetValue() + 1)
        
    def updateInfoLabel(self, resultEvnt=None):
        if resultEvnt and resultEvnt.data:
            print('updateInfoLabel: %s' % resultEvnt.data)
            self.__setInfoLabel(resultEvnt.data)
    def __setInfoLabel(self, txt):
        wx.PostEvent(self.progress_bar, ProgBarEvent(EVT_LABEL_CHANGED_ID, data=txt))
#        self.progress_bar.SetInfoLabel(txt)
        
    def OnCancelClicked(self, e):
        self._cancelled = True
        if self.worker:
            self.worker.cancel()
        
    def cancelled(self):
        return self._cancelled
    
    def finish(self, resultEvnt=None):
        success = resultEvnt.data if resultEvnt else False
        print('ProgressDialog.finish! - success: ', success)
        self.__setInfoLabel('canceld!' if self._cancelled else 'finished!')
        self.close()
        
    def close(self, resultEvnt=None):
        self.Show(False)
        self.Destroy()
        
    def executeWorker(self, e):
        if self.worker:
            self.worker.executeThreaded()

def print_test(sources, target, copy_frame):
    time.sleep(2)
    copy_frame.updateGauge(30)
    
    
def getPicturePath(fileName):
    gosh_base_dir = os.path.abspath(os.path.join(os.path.dirname(__file__), os.pardir))
    return os.path.abspath(os.path.join(os.path.join(gosh_base_dir, "pics"), fileName))
