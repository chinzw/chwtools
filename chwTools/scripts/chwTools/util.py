#/***********************************************************************************
#* Copyright (c) 2018 Cristian Hinz Welkens.
#*
#* This file is part of chwTools (https://github.com/chinzw/chwtools).
#*
#* Permission is hereby granted, free of charge, to any person obtaining a copy of
#* this software and associated documentation files (the "Software"), to deal in
#* the Software without restriction, including without limitation the rights to 
#* use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
#* of the Software, and to permit persons to whom the Software is furnished to do
#* so, subject to the following conditions:
#*
#* The above copyright notice and this permission notice shall be included in all
#* copies or substantial portions of the Software.
#*
#* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,
#* INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A
#* PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
#* HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION
#* OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
#* SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
#***********************************************************************************/

from maya import cmds

def loadPlugin():
	if not cmds.pluginInfo("chwTools.so", query=True, l=True):
		cmds.loadPlugin("chwTools.so")


def makeControlSphere(name='sphere', scale=1.0):
	circleA = cmds.circle(r=scale, nr=(1,0,0), name=name+'_CTRL')[0]
	cmds.delete(circleA, ch=True)
	circleAShape = cmds.listRelatives(circleA, s=True)[0]
	cmds.setAttr(circleAShape+'.overrideEnabled', 1)
	cmds.setAttr(circleAShape+'.overrideColor', 6)
	circleB = cmds.circle(r=scale, nr=(0,1,0))[0]
	cmds.delete(circleB, ch=True)
	circleBShape = cmds.listRelatives(circleB, s=True)[0]
	cmds.setAttr(circleBShape+'.overrideEnabled', 1)
	cmds.setAttr(circleBShape+'.overrideColor', 13)
	circleC = cmds.circle(r=scale, nr=(0,0,1))[0]
	cmds.delete(circleC, ch=True)
	circleCShape = cmds.listRelatives(circleC, s=True)[0]
	cmds.setAttr(circleCShape+'.overrideEnabled', 1)
	cmds.setAttr(circleCShape+'.overrideColor', 14)
	cmds.parent(circleBShape, circleCShape, circleA, add=True, shape=True)
	cmds.delete(circleB, circleC)
	return circleA


def getLetter(index):
	alphabet = ['A','B','C','D','E','F','G','H','I','J','K','L','M','N','O','P','Q','R','S','T','U','V','W','X','Y','Z']
	return alphabet[index]


def nextName(name):
	name = name + '_'
	index = 0
	while True:
		prefixedName = name + getLetter(index)
		if not cmds.objExists(prefixedName):
			return prefixedName
		index = index + 1
		if index > 25:
			index = 0
			prefixedName = name + getLetter(index)
			name = prefixedName
