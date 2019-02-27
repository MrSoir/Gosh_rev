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

EVT_MESSAGE_ID  = wx.NewIdRef()
EVT_PROGRESS_ID = wx.NewIdRef()
EVT_FINISH_ID   = wx.NewIdRef()


class ProgressFrame(wx.Frame):

    def __init__(self, parent, title, worker=None):
        super(ProgressFrame, self).__init__(parent, title=title,size=(350, 150),style=wx.DEFAULT_FRAME_STYLE ^ wx.RESIZE_BORDER)
        self.Centre()
        self.SetIcon(wx.Icon(getPicturePath('MrSoirIcon_cursor.png')))
        
        self.worker = worker
        
        #panel = wx.Panel(self, wx.ID_ANY)
        
        vbox = wx.BoxSizer(wx.VERTICAL)
        self.SetSizer(vbox)
        
        self.gauge = wx.Gauge(self, range = 100, size = (250, 25), style= wx.GA_HORIZONTAL) 
        self.gauge.SetValue(0)
        vbox.Add(self.gauge, 0, wx.ALL|wx.EXPAND|wx.ALIGN_CENTER_HORIZONTAL, 5) 
        
        self.info_lbl = wx.StaticText(self,label = "copying files...", style=wx.GA_HORIZONTAL ) 
        vbox.Add(self.info_lbl, 0, wx.ALL|wx.EXPAND|wx.ALIGN_CENTER_HORIZONTAL, 5) 
        
        self.cncl_btn = wx.Button(self, label="cancel") 
        vbox.Add(self.cncl_btn, 0, wx.EXPAND)
        self.cncl_btn.Bind(wx.EVT_BUTTON, self.onCancelClicked)
        
        self.exe_btn = wx.Button(self, label="execute") 
        vbox.Add(self.exe_btn, 0, wx.EXPAND)
        self.exe_btn.Bind(wx.EVT_BUTTON, self.executeWorker)
        
        self.Bind(EVT_DIALOG_REQUEST, self.updateInfoLabel, id=EVT_MESSAGE_ID)
        self.Bind(EVT_DIALOG_REQUEST, self.updateGauge,     id=EVT_PROGRESS_ID)
        self.Bind(EVT_DIALOG_REQUEST, self.finish,          id=EVT_FINISH_ID)
        
    def setWorker(self, worker):
        self.worker = worker
        
    def updateGauge(self, resultEvnt=None):
        if resultEvnt and resultEvnt.data:
            print('updateGauge: %s' % resultEvnt.data)
            self.gauge.SetValue(resultEvnt.data)
        
    def incrementGauge(self):
        self.gauge.SetValue(self.gauge.GetValue() + 1)
        
    def updateInfoLabel(self, resultEvnt=None):
        if resultEvnt and resultEvnt.data:
            print('updateInfoLabel: %s' % resultEvnt.data)
            self.info_lbl.SetLabel(resultEvnt.data)
        
    def onCancelClicked(self, e):
        self._cancelled = True
        if self.worker:
            self.worker.cancel()
        
    def cancelled(self):
        return self._cancelled
    
    def finish(self, resultEvnt=None):
        print('ProgressDialog.finish!')
        if resultEvnt:
            print('success: ', resultEvnt.data)
        self.close()
        
    def close(self, resultEvnt=None):
        self.Show(False)
        self.Destroy()
        
    def executeWorker(self, e):
        print('executing worker...')
        if self.worker:
            print('worker exists!')
            self.worker.executeThreaded()

def print_test(sources, target, copy_frame):
    time.sleep(2)
    print('print_test... target: ', target)
    copy_frame.updateGauge(30)
    
    
def getPicturePath(fileName):
    gosh_base_dir = os.path.abspath(os.path.join(os.path.dirname(__file__), os.pardir))
    return os.path.abspath(os.path.join(os.path.join(gosh_base_dir, "pics"), fileName))
