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

#DialogEvent, EVT_DIALOG = NE.NewCommandEvent()

EVT_SCIP_ID   = wx.NewIdRef()

class ReplaceEntryDialog(sized_controls.SizedDialog):

    def __init__(self, entry_name, entry_baseDir, size=(600,320), parent=None):
        super(ReplaceEntryDialog, self).__init__(parent, size=size, title='Replace File')
                
        isFile = os.path.isfile(os.path.join(entry_baseDir, entry_name))
        
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
        self.addMessage('\nSorry, but the %s:' % ('file' if isFile else 'folder'), wx.ALIGN_LEFT, msg_vbox)
        self.addMessage('', wx.ALIGN_CENTER_HORIZONTAL, msg_vbox, fontSize=4)
        self.addMessage('%s' % entry_name, wx.ALIGN_CENTER_HORIZONTAL, msg_vbox, fontSize=20, fontColor=self.FONT_HIGHLIGHT_COLOR, fontWeight=wx.FONTWEIGHT_BOLD)
        self.addMessage('', wx.ALIGN_CENTER_HORIZONTAL, msg_vbox, fontSize=4)
        self.addMessage('does alredy exist in:', wx.ALIGN_LEFT, msg_vbox)
        self.addMessage('', wx.ALIGN_CENTER_HORIZONTAL, msg_vbox, fontSize=4)
        maxBaseDirLen = 50
        disp_entry_baseDir = entry_baseDir if len(entry_baseDir) < maxBaseDirLen else '...' + entry_baseDir[max(0,len(entry_baseDir)-50):]
        self.addMessage('%s' % disp_entry_baseDir, wx.ALIGN_CENTER_HORIZONTAL, msg_vbox, fontSize=12)
        self.addMessage('----------------------------------------------------------------', wx.ALIGN_CENTER_HORIZONTAL, msg_vbox, fontSize=15)
        self.addMessage("Do you want to replace the file?\n", wx.ALIGN_LEFT, msg_vbox, fontSize=20, fontColor=self.FONT_WARNING_COLOR, fontWeight=wx.FONTWEIGHT_BOLD)
        vbox.Add(msg_vbox, 0, wx.ALIGN_CENTER_HORIZONTAL)
        
        btn_box = wx.BoxSizer(wx.HORIZONTAL)
        button_ok = wx.Button(self, wx.ID_YES, label='YES')
        button_ok.Bind(wx.EVT_BUTTON, self.on_button)
        btn_box.Add(button_ok, 0, wx.ALIGN_CENTER_HORIZONTAL)
        button_ok.SetForegroundColour(wx.Colour(255,0,0))
        
        button_ok = wx.Button(self, wx.ID_NO, label='NO')
        button_ok.Bind(wx.EVT_BUTTON, self.on_button)
        btn_box.Add(button_ok, 0, wx.ALIGN_CENTER_HORIZONTAL)
        button_ok.SetForegroundColour(wx.Colour(0,150,0))
        
        button_ok = wx.Button(self, EVT_SCIP_ID, label='SKIP')
        button_ok.Bind(wx.EVT_BUTTON, self.on_button)
        btn_box.Add(button_ok, 0, wx.ALIGN_CENTER_HORIZONTAL)
        
        button_ok = wx.Button(self, wx.ID_CANCEL, label='CANCEL')
        button_ok.Bind(wx.EVT_BUTTON, self.on_button)
        btn_box.Add(button_ok, 0, wx.ALIGN_CENTER_HORIZONTAL)
        
        vbox.Add(btn_box, 0, wx.ALIGN_CENTER_HORIZONTAL)
        
        self.SetBackgroundColour(wx.Colour(255,255,255, 0))
        pane.SetBackgroundColour(wx.Colour(255,255,255, 0))

        vbox.FitInside(self)
#        self.Fit()
        
        self.Bind(wx.EVT_PAINT, self.OnPaint)
    
    def addMessage(self, msg, flags, szr, fontSize=None, fontColor=None, fontWeight=None):
        label = wx.StaticText(self, label=msg)
        label.SetFont(self.getFont(fontSize=fontSize, fontWeight=fontWeight))
        label.SetForegroundColour(fontColor if fontColor else self.FONT_COLOR)
        szr.Add(label, 0, flags)
        
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
    entry_name = 'C++ Literature'
    entry_baseDir = '/home/hippo/Documents/home/hippo/workspace_Qt/Gosh/Gosh/scripts/replace_entry_dialog.py'
    dlg = ReplaceEntryDialog(entry_name=entry_name, entry_baseDir=entry_baseDir)
    dlg.Show()
    app.MainLoop()