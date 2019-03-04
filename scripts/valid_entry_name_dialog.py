#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Created on Thu Feb 28 20:00:22 2019

@author: hippo
"""

import wx
#import wx.lib.newevent as NE
from wx.lib import sized_controls

import os.path

from enum import Enum

class ENTRY_TYPE(Enum):
    FILE = 1
    DIRECTORY = 2

#DialogEvent, EVT_DIALOG = NE.NewCommandEvent()

from entry_name_validation import isValidDirectoryName, isValidFileName, invalidFileNameWarning, invalidDirectoryNameWarning

class ValidEntryNameDialog(sized_controls.SizedDialog):

    def __init__(self, entry_name, entry_baseDir, validatorFunc=None, warningFunc=None, entry_type=ENTRY_TYPE.FILE, size=(600,250), parent=None):
        super(ValidEntryNameDialog, self).__init__(parent, size=size, title='Select %s Name' % ('File' if entry_type == ENTRY_TYPE.FILE else 'Folder'))
                
        init_abs_path = os.path.join(entry_baseDir, entry_name)
        isFile = os.path.isfile(init_abs_path) if os.path.exists(init_abs_path) else entry_type == ENTRY_TYPE.FILE
        
        self.entry_baseDir = entry_baseDir
        self.entry_name    = entry_name
        
        self.validatorFunc = validatorFunc if validatorFunc else (isValidFileName if isFile else isValidDirectoryName)
        self.warningFunc   = warningFunc if warningFunc else (invalidFileNameWarning if isFile else invalidDirectoryNameWarning)
        
        self.FONT_NAME = 'Samantana'
        self.FONT_WEIGHT = wx.FONTWEIGHT_NORMAL
        self.FONT_SIZE = 15
        self.FONT_COLOR = (0,0,0)
        self.FONT_HIGHLIGHT_COLOR = (100,0,0)
        self.FONT_WARNING_COLOR = (255,0,0)
        
        self.BACKGR_FRAME_COLOR = (255,255,255)
        self.BACKGR_COLOR = (255,255,255)
        self.BUTTONS_BACKGR_COLOR = (0,0,0)
        
        pane = self.GetContentsPane()
        
        vbox = wx.BoxSizer(wx.VERTICAL)
        self.SetSizer(vbox)
        
        msg_vbox = wx.BoxSizer(wx.VERTICAL)
        self.addMessage('\nPlease select a valid %s name:' % ('file' if isFile else 'folder'), wx.ALIGN_LEFT, msg_vbox, fontSize=20, fontColor=self.FONT_HIGHLIGHT_COLOR, fontWeight=wx.FONTWEIGHT_BOLD)
        self.addMessage('', wx.ALIGN_CENTER_HORIZONTAL, msg_vbox, fontSize=4)
        vbox.Add(msg_vbox, 0, wx.ALIGN_CENTER_HORIZONTAL)
        
        #--------------------------
        
        self.text_ctrl = wx.TextCtrl(self, size=(size[0] * 0.7, 30), value = entry_name, style = wx.TE_CENTER) 
        self.text_ctrl.Bind(wx.EVT_TEXT, self.onTextChanged)
        vbox.Add(self.text_ctrl, 0, wx.ALIGN_CENTER_HORIZONTAL)
        
        warning_vbox = wx.BoxSizer(wx.VERTICAL)
        self.addMessage('', wx.ALIGN_CENTER_HORIZONTAL, warning_vbox, fontSize=4)
        self.warningLabel = self.addMessage('', wx.ALIGN_LEFT, warning_vbox, fontSize=15, fontColor=self.FONT_WARNING_COLOR, fontWeight=wx.FONTWEIGHT_BOLD)
        self.addMessage('', wx.ALIGN_CENTER_HORIZONTAL, warning_vbox, fontSize=20)
        vbox.Add(warning_vbox, 0, wx.ALIGN_CENTER_HORIZONTAL)
        
        
        #--------------------------
        
        btn_box = wx.BoxSizer(wx.HORIZONTAL)
        button_ok = wx.Button(self, wx.ID_OK, label='OK')
        button_ok.Bind(wx.EVT_BUTTON, self.on_button)
        btn_box.Add(button_ok, 0, wx.ALIGN_CENTER_HORIZONTAL)
        button_ok.SetForegroundColour(wx.Colour(255,0,0))
        self.okBtn = button_ok
        
        button_ok = wx.Button(self, wx.ID_CANCEL, label='CANCEL')
        button_ok.Bind(wx.EVT_BUTTON, self.on_button)
        btn_box.Add(button_ok, 0, wx.ALIGN_CENTER_HORIZONTAL)
        
        vbox.Add(btn_box, 0, wx.ALIGN_CENTER_HORIZONTAL)
        
        self.SetBackgroundColour(wx.Colour(255,255,255, 0))
        pane.SetBackgroundColour(wx.Colour(255,255,255, 0))
        
        self.validateUserInput()

        vbox.FitInside(self)
#        self.Fit()
        
        self.Bind(wx.EVT_PAINT, self.OnPaint)
        
    def validateUserInput(self):
        user_input = self.getText()
        isValid = self.validatorFunc(self.entry_baseDir, user_input)
        self.okBtn.Enable() if isValid else self.okBtn.Disable()
        if isValid:
            self.warningLabel.SetLabel("")
            self.warningLabel.Disable()
            self.warningLabel.Hide()
        else:
            self.warningLabel.SetLabel( self.warningFunc(self.entry_baseDir, user_input) )
            self.warningLabel.Enable()
            self.warningLabel.Show()
            
    def getText(self):
        return self.text_ctrl.GetValue().strip()
        
    def onTextChanged(self, e):
        self.validateUserInput()
    
    def addMessage(self, msg, flags, szr, fontSize=None, fontColor=None, fontWeight=None):
        label = wx.StaticText(self, label=msg)
        label.SetFont(self.getFont(fontSize=fontSize, fontWeight=fontWeight))
        label.SetForegroundColour(fontColor if fontColor else self.FONT_COLOR)
        szr.Add(label, 0, flags)
        return label
        
    def getFont(self, fontSize=None, fontWeight=None):
        return wx.Font(fontSize if fontSize else self.FONT_SIZE, wx.FONTFAMILY_DECORATIVE, style=wx.FONTSTYLE_NORMAL, weight=fontWeight if fontWeight else self.FONT_WEIGHT, faceName=self.FONT_NAME)
    def on_button(self, event):
        if self.IsModal():
            self.EndModal(event.EventObject.Id)
            self.Destroy()
        else:
            self.Close()
            self.Destroy()

    def OnPaint(self, event):
        dc = wx.BufferedPaintDC(self)
        self.Draw(dc)

    def Draw(self, dc):
        width, height = self.GetClientSize()

        if not width or not height:
            # Nothing to do, we still don't have dimensions!
            return
        backColour = self.BACKGR_FRAME_COLOR
        backBrush = wx.Brush(backColour, wx.SOLID)
        dc.SetBackground(backBrush)
        dc.Clear()

        # paint bar-background:
        dc.SetBrush(wx.Brush(self.BACKGR_COLOR, wx.SOLID))
        padding = 20
        dc.DrawRectangle(padding,padding, width-padding*2, height-padding*2)
        
        dc.SetBrush(wx.Brush(self.BUTTONS_BACKGR_COLOR, wx.SOLID))
        btn_padding = 0
        btn_height = 60
        dc.DrawRectangle(padding + btn_padding,height-padding - btn_height, width-padding*2-2*btn_padding, btn_height)

if __name__ == '__main__':
    app = wx.App(False)
    entry_name = 'script.py'
    entry_baseDir = '/home/hippo/Documents/tests_src'
    dlg = ValidEntryNameDialog(entry_name=entry_name, entry_baseDir=entry_baseDir)
    dlg.Show()
    app.MainLoop()