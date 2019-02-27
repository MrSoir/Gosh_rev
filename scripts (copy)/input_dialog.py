#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Created on Wed Feb 20 21:53:53 2019

@author: hippo
"""

import wx
from wx.lib import sized_controls

class InputDialog(sized_controls.SizedDialog):

    def __init__(self,
                 message="select a name:",
                 init_val="", 
                 validatorFunc=lambda txt: True,
                 warningFunc=lambda txt: "invalid input - make a different choice!",
                 *args, **kwargs):
        super(InputDialog, self).__init__(*args, **kwargs)
        pane = self.GetContentsPane()
        
        self.validatorFunc = validatorFunc
        self.warningFunc = warningFunc
        
        width = 500
#        height = 160
        
        rowHeight = 30

        #----------------------
        
        pane_textInput = self.genPane(pane)

        static_line = wx.StaticLine(pane_textInput, style=wx.LI_HORIZONTAL)
        static_line.SetSizerProps(border=(('all', 0)), expand=True)
        
        self.textCtrlLabel = wx.StaticText(pane_textInput, label=message)

        
        self.text_ctrl = wx.TextCtrl (pane_textInput, size=(width,rowHeight), value = init_val, style = wx.TE_LEFT ) 
        self.text_ctrl.Bind(wx.EVT_TEXT, self.onTextChanged)
        
        #----------------------
        
        self.warningLabel = wx.StaticText(pane, label="")#, style=wx.TE_CENTRE)
        self.warningLabel.SetForegroundColour((255,0,0))

        #----------------------
        
        pane_btns = self.genPane(pane)

        self.okBtn = wx.Button(pane_btns, wx.ID_OK, label='Ok')
        self.okBtn.Bind(wx.EVT_BUTTON, self.on_button)
        
        self.validateUserInput()

        self.cancelBtn = wx.Button(pane_btns, wx.ID_CANCEL, label='Cancel')
        self.cancelBtn.Bind(wx.EVT_BUTTON, self.on_button)

        self.Fit()
#        self.Size = (width,height)
        
    def validateUserInput(self):
        user_input = self.getText()
        isValid = self.validatorFunc(user_input)
        self.okBtn.Enable() if isValid else self.okBtn.Disable()
        if isValid:
            self.warningLabel.SetLabel("")
            self.warningLabel.Disable()
            self.warningLabel.Hide()
        else:
            self.warningLabel.SetLabel( self.warningFunc(user_input) )
            self.warningLabel.Enable()
            self.warningLabel.Show()
        
    def onTextChanged(self, e):
        self.validateUserInput()
        
    def getText(self):
        return self.text_ctrl.GetValue().strip()
        
    def genPane(self, pane):
        p = sized_controls.SizedPanel(pane)
        p.SetSizerType('horizontal')
        p.SetSizerProps(align='center')#, expande=True)
        return p

    def on_button(self, event):
        if self.IsModal():
            self.EndModal(event.EventObject.Id)
        else:
            self.Close()