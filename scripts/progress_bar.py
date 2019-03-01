#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Created on Wed Feb 27 21:40:40 2019

@author: hippo
"""

import wx
import wx.lib.newevent as NE

ProgBarEvent, EVT_PROG_BAR = NE.NewCommandEvent()

EVT_VALUE_CHANGED_ID = wx.NewIdRef()
EVT_LABEL_CHANGED_ID = wx.NewIdRef()

class ProgressBar(wx.Control):
    def __init__(self, parent, id=wx.ID_ANY, label="", pos=wx.DefaultPosition,
                 size=wx.DefaultSize, style=wx.NO_BORDER, validator=wx.DefaultValidator,
                 name="CustomCheckBox"):
        wx.Control.__init__(self, parent, id, pos, size, style, validator, name)

        self._padding = 5

        self._value = 0
        self.BAR_COLOR        = wx.Colour(0,255,0)
        self.BAR_BACKGR_COLOR = wx.Colour(0,50,0)
        
        self.FONT_NAME = 'Samantana'
        self.FONT_WEIGHT = wx.FONTWEIGHT_BOLD
        self.FONT_SIZE = 10
        self.FONT_COLOR = wx.Colour(255,0,0)
        
        self.SetLabel(label)
        self.SetInitialSize(size)
        self.InheritAttributes()

        self.Bind(wx.EVT_PAINT, self.OnPaint)
        self.Bind(wx.EVT_ERASE_BACKGROUND, self.OnEraseBackground)
        
        self.Bind(EVT_PROG_BAR, self.SetValue,     id=EVT_VALUE_CHANGED_ID)
        self.Bind(EVT_PROG_BAR, self.SetInfoLabel, id=EVT_LABEL_CHANGED_ID)
        
    #--------   
    def SetValue(self, event):
        if event and event.data:
            self._value = max(min(event.data, 100), 0)
            self.Refresh()
    def GetValue(self):
        return self._value
    #--------
    def SetInfoLabel(self, event):
        if event and event.data:
            self.SetLabel(event.data)
            self.Refresh()
    def GetInfoLabel(self):
        return self.GetLabel()        
    #--------
    def OnPaint(self, event=None):
        dc = wx.BufferedPaintDC(self)
        self.Draw(dc)

    def Draw(self, dc):
        width, height = self.GetClientSize()

        if not width or not height:
            # Nothing to do, we still don't have dimensions!
            return
        backColour = wx.Colour((0,0,0))
        backBrush = wx.Brush(backColour, wx.SOLID)
        dc.SetBackground(backBrush)
        dc.Clear()

        # paint bar-background:
        dc.SetBrush(wx.Brush(self.BAR_BACKGR_COLOR, wx.SOLID))
        padding = self._padding
        dc.DrawRectangle(padding,padding, width-padding*2, height-padding*2)
        
        center = (width*0.5, height*0.5)
        viewport = (width - 2*padding, height - 2*padding)
        
        # paint progress-rect:
        value = self._value
        dc.SetBrush(wx.Brush(self.BAR_COLOR, wx.SOLID))  
        barSize = (viewport[0] * value/100, viewport[1])
        dc.DrawRectangle(padding,padding, barSize[0], barSize[1])
        dc.SetBrush(wx.Brush(wx.Colour(100,100,0), wx.SOLID))  
        
        # paint label:
        label = self.GetLabel()
        if label:
            dc.SetFont(self.getFont())
#            fm = dc.GetFontMetrics()
#            dc.SetPen(wx.Pen(self.FONT_COLOR))
            dc.SetTextForeground(self.FONT_COLOR)
            str_meas = dc.GetTextExtent(label)
            lbl_x = center[0] - str_meas[0] * 0.5
            lbl_y = center[1] - str_meas[1] * 0.5
            dc.DrawText(label, lbl_x, lbl_y)

    def OnEraseBackground(self, event):
        pass

    def getFont(self):
        return wx.Font(self.FONT_SIZE, wx.FONTFAMILY_DECORATIVE, style=wx.FONTSTYLE_NORMAL, weight=self.FONT_WEIGHT, faceName=self.FONT_NAME)
 