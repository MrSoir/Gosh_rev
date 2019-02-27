#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Created on Sat Dec 29 21:05:29 2018

@author: hippo
"""


from dataclasses import dataclass, field
from threading import Thread
import threading
import time

from path_func import *
import shutil

#class A:
#    def __init__(self, x):
#        self.x = x
#        self._cancelled = False
#        
#    def setX(self, x):
#        self.x = x
#        
#    def __str__(self):
#        return 'A(x=%s, _cancelled=%s)' % (self.x, self._cancelled)
#        
#    
#class B(A):
#    def __init__(self, x):
#        super().__init__(x)
#        
#    def cancel(self):
#        self._cancelled = True
#    
#    def cancelled(self):
#        return self._cancelled
#    
#    def __str__(self):
#        return 'B(x=%s, _cancelled=%s)' % (self.x, self._cancelled)
#a = B(x=5)
#print(a)
#a.f = a.setX
#try:
#    a.f(30)
#except:
#    pass
#print(a)
#
#a.cancel()
#print(a)


p = '/home/hippo/Documents/Effective_DevOps.pdf'
base_dir, unique_entry_name = generateUniquePath(p)
absUniquePath = os.path.join(base_dir, unique_entry_name)
print(p)
print(absUniquePath)
print(p == absUniquePath)