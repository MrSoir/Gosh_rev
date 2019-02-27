#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Created on Wed Feb 27 21:40:40 2019

@author: hippo
"""

import wx
import wx.lib.newevent as NE

BDButtonEvent, EVT_BDButton = NE.NewCommandEvent()

EVT_BDCLICKED_ID = wx.NewIdRef()


class Button(wx.Control):
    def __init__(self, parent, id=wx.ID_ANY, label="", pos=wx.DefaultPosition,
                 size=wx.DefaultSize, style=wx.NO_BORDER, validator=wx.DefaultValidator,
                 name="CustomCheckBox"):
        wx.Control.__init__(self, parent, id, pos, size, style, validator, name)
        
        self._mouIn = False
        self._pressed = False
        self.MOUSE_IN_COLOR = wx.Colour(255,0,0)
        self.PRESSED_COLOR = wx.Colour(255,100,100)
        self.COLOR = wx.Colour(150,0,0)

        self._padding = 1

        self._value = 45
        self.BAR_COLOR        = wx.Colour(0,255,0)
        self.BAR_BACKGR_COLOR = wx.Colour(0,  0,100)
        
        self.FONT_NAME = 'Samantana'
        self.FONT_WEIGHT = wx.FONTWEIGHT_BOLD
        self.FONT_SIZE = 20
        self.FONT_COLOR = wx.Colour(255,255,255)
        
        self.SetLabel(label)
        self.SetInitialSize(size)
        self.InheritAttributes()

        self.Bind(wx.EVT_PAINT, self.OnPaint)
        self.Bind(wx.EVT_ERASE_BACKGROUND, self.OnEraseBackground)
        
        self.Bind(wx.EVT_LEFT_DOWN, self.OnMousePressed)
        self.Bind(wx.EVT_LEFT_UP,   self.OnMouseReleased)
        if wx.Platform == '__WXMSW__':
            # MSW Sometimes does strange things...
            self.Bind(wx.EVT_LEFT_DCLICK,  self.OnMousePressed)
            
        self.Bind(wx.EVT_KEY_UP, self.OnKeyUp)
        self.Bind(wx.EVT_ENTER_WINDOW, self.OnMOuseEnter)
        self.Bind(wx.EVT_LEAVE_WINDOW, self.OnMOuseLeave)
        
    def SetOnClick(self, callback):
        self.onClickFunc = callback
    def _callClickCallback(self):
        wx.PostEvent(self.GetParent(), BDButtonEvent(EVT_BDCLICKED_ID))
#        try:
#            self.onClickFunc()
#        except:
#            pass
        
    def OnMOuseEnter(self, event):
        self._mouIn = True
        self.Refresh()
    def OnMOuseLeave(self, event):
        self._mouIn = False
        self.Refresh()

    def OnMousePressed(self, event):
        self._pressed = True
        self.Refresh()
        self._callClickCallback()
        
    def OnMouseReleased(self, event):
        self._pressed = False
        self.Refresh()
        
    def OnKeyUp(self, event):
        if event.GetKeyCode() == wx.WXK_ENTER or \
           event.GetKeyCode() == wx.WXK_CANCEL:
            self._callClickCallback()
        
    def OnPaint(self, event):
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

        # button color:
        color = self.COLOR
        if self._pressed:
            color = self.PRESSED_COLOR
        elif self._mouIn:
            color = self.MOUSE_IN_COLOR
        dc.SetBrush(wx.Brush(color, wx.SOLID))
        padding = self._padding
        dc.DrawRectangle(padding,padding, width-padding*2, height-padding*2)
        
        center = (width*0.5, height*0.5)
        viewport = (width - 2*padding, height - 2*padding)
        
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
 