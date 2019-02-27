#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Created on Sat Dec 29 21:05:29 2018

@author: hippo
"""


import wx 
from progress_bar import ProgressBar
from button import Button


class Mywin(wx.Frame): 
            
   def __init__(self, parent, title): 
      super(Mywin, self).__init__(parent, title = title,size = (500,300))  
      self.InitUI() 
      
         
   def InitUI(self): 
      self.Centre()
      vbox = wx.BoxSizer(wx.VERTICAL)
      self.SetSizer(vbox)
      self.progress_bar = ProgressBar(self, label="processing...", size=(60,60))
      vbox.Add(self.progress_bar, 0, wx.EXPAND)
      self.cncl_btn = Button(self, label="cancel", size=(250, 40))
      vbox.Add(self.cncl_btn, 0, wx.ALIGN_CENTER_HORIZONTAL)
      self.SetBackgroundColour(wx.Colour(0,0,0))
		
ex = wx.App() 
app = Mywin(None,'Drawing demo')
app.Show(True)
ex.MainLoop()