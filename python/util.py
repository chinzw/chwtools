#/*************************************************************************
#* Copyright (c) 2018 Cristian Hinz Welkens.
#*
#* This file is part of chwTools (https://github.com/chinzw/chwtools).
#*
#* chwTools is free software: you can redistribute it and/or modify
#* it under the terms of the GNU General Public License as published by
#* the Free Software Foundation, version 3 of the License.
#*
#* chwTools is distributed in the hope that it will be useful,
#* but WITHOUT ANY WARRANTY; without even the implied warranty of
#* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#* GNU General Public License for more details.
#*
#* You should have received a copy of the GNU General Public License
#* along with chwTools.  If not, see <https://www.gnu.org/licenses/>.
# *************************************************************************/

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
