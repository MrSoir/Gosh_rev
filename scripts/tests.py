#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Created on Sat Dec 29 21:05:29 2018

@author: hippo
"""

#import wx
#
#import input_dialog
#import  entry_name_validation

import os.path
import os
from collections.abc import Iterable

import static_functions

class Worker():
    def __init__(self, progress_dialog = None):
        super().__init__()
        self.progress_dialog = progress_dialog
        self.__cancelled = False
        
    def cancel(self):
        self.__cancelled = True
        
    def cancelled(self):
        return self.__cancelled
    
    def setProgressDialog(self, progress_dialog):
        self.progress_dialog = progress_dialog


class Der(Worker):
    def __init__(self, progress_dialog):
        super().__init__(progress_dialog)
        print("prog_bar is: '%s'" % self.progress_dialog)
        
def listit(x):
    xs = [x,] if isinstance(x, str) else x
    for v in xs:
        print(v)


from worker import WorkerDirEntry

    

if __name__ == '__main__':
    p = '/home/hippo/workspace_Qt/Gosh/Gosh/scripts'

    we = WorkerDirEntry.createWorkerDirFromPath(p)
    print(we)
    
    

#
#import wx 
#import static_functions
# 
#class Mywin(wx.Frame): 
#            
#   def __init__(self, parent, title): 
#      super(Mywin, self).__init__(parent, title = title,size = (300,200))  
#         
#      self.InitUI() 
#         
#   def InitUI(self):    
#      self.count = 0 
#      pnl = wx.Panel(self) 
#      vbox = wx.BoxSizer(wx.VERTICAL) 
#		
#      hbox1 = wx.BoxSizer(wx.HORIZONTAL) 
#      hbox2 = wx.BoxSizer(wx.HORIZONTAL) 
#		
#      self.text = wx.TextCtrl(pnl, size = (250, 25),style = wx.TE_READONLY) 
#      self.btn1 = wx.Button(pnl, label = "Enter Text") 
#      self.Bind(wx.EVT_BUTTON, self.OnClick, self.btn1) 
#		
#      hbox1.Add(self.text, proportion = 1, flag = wx.ALIGN_CENTRE) 
#      hbox2.Add(self.btn1, proportion = 1, flag = wx.RIGHT, border = 10)
#		
#      vbox.Add((0, 30)) 
#      vbox.Add(hbox1, flag = wx.ALIGN_CENTRE) 
#      vbox.Add((0, 20)) 
#      vbox.Add(hbox2, proportion = 1, flag = wx.ALIGN_CENTRE) 
#		
#      pnl.SetSizer(vbox) 
#      self.Centre() 
#      self.Show(True)
#		
#   def OnClick(self, e): 
#      dlg_rslt = static_functions.showInputDialog(self, "please enter a string:", "Heading", "Hallo Welt!")
#      if dlg_rslt: 
#          print(dlg_rslt)
#      else:
#          print('None selected || Cancelled!')
#
##dlg = wx.TextEntryDialog(self, 'Enter Your Name','Text Entry Dialog')
##      dlg =  wx.MessageDialog(self, "This is a Message Box", "caption", wx.YES | wx.NO | wx.CANCEL | wx.ICON_QUESTION) 
#                                    
#ex = wx.App() 
#Mywin(None,'TextEntry Demo') 
#ex.MainLoop()