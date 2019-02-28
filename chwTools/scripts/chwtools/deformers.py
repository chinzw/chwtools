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
# *************************************************************************

import re
import util
from maya import cmds

util.loadPlugin()

def chwMeshRelax():
	selection = cmds.ls(sl=True)
	if len(selection) == 1:
		cmds.deformer(selection, type='chwMeshRelax')
	else:
		print "chwMeshRelax: You need to select one mesh!\n"

def chwNormalOffset():
	selection = cmds.ls(sl=True)
	if len(selection) == 1:
		cmds.deformer(selection, type='chwNormalOffset')
	else:
		print "chwNormalOffset: You need to select one mesh!\n"

def chwVertexBind():
	selection = cmds.ls(sl=True)
	if len(selection) == 2:
		vs = cmds.deformer(selection[-1], type='chwVertexBind')
		cmds.connectAttr(selection[0]+'.worldMesh[0]', vs[0]+'.driverMesh')
		cmds.setAttr(vs[0]+'.initialize', 1)
	else:
		print "chwVertexBind: You need to select a driver and a target!\n"

def chwSurfaceTweak():
	selection = cmds.ls(sl=True)
	if len(selection) == 1:
		selection = selection[0]
		if selection.count('vtx'):
			splits = selection.split('.')
			mesh = splits[0]
			vtx = re.sub("[^0-9]", "", splits[1])
			name = util.nextName('chwSurfaceTweak_'+mesh)
			ss = cmds.deformer(mesh, type='chwSurfaceTweak')[0]
			cmds.setAttr(ss+'.vertex', int(vtx))
			pivot_ctrl = cmds.group(em=True, name=name)
			offset_ctrl = util.makeControlSphere(name+'_Offset', 0.25)
			sticky_ctrl = util.makeControlSphere(name+'_Deform')
			cmds.parent(sticky_ctrl, offset_ctrl)
			cmds.parent(offset_ctrl, pivot_ctrl)
			cmds.addAttr(sticky_ctrl, at='double', ln='envelope', k=1, dv=1, min=0, max=1)
			cmds.addAttr(sticky_ctrl, at='enum', ln='falloffMode', k=1, en='Volume')
			cmds.addAttr(sticky_ctrl, at='double', ln='falloffRadius', k=1, dv=1, min=0)
			cmds.connectAttr(sticky_ctrl+'.falloffMode', ss+'.falloffMode', f=True)
			cmds.connectAttr(sticky_ctrl+'.falloffRadius', ss+'.falloffRadius', f=True)
			cmds.connectAttr(sticky_ctrl+'.envelope', ss+'.envelope', f=True)
			cmds.connectAttr(sticky_ctrl+'.worldMatrix[0]', ss+'.deformMatrix')
			cmds.connectAttr(offset_ctrl+'.worldMatrix[0]', ss+'.relativeMatrix')
			cmds.connectAttr(ss+'.outRotate', pivot_ctrl+'.rotate')
			cmds.connectAttr(ss+'.outTranslate', pivot_ctrl+'.translate')
			cmds.select(sticky_ctrl, r=True)
			return [pivot_ctrl, offset_ctrl, sticky_ctrl, ss]
		else:
			print "chwSurfaceTweak: You need to select a mesh vertex!\n"
	else:
		print "chwSurfaceTweak: You need to select a mesh vertex!\n"


def chwCollision():
	selection = cmds.ls(sl=True)
	if len(selection) > 1:
		defmesh = selection[0]
		selection.pop(0)
		colDefs = [x for x in cmds.listHistory(defmesh) if cmds.nodeType(x) == 'chwCollision']
		if colDefs:
			nextIdx = 0
			coldef = colDefs[0]
			indices = cmds.getAttr(coldef+'.collisionMeshList', mi=True)
			if indices:
				nextIdx = indices[-1]+1
			for idx in xrange(0, len(selection)):
				midx = idx + nextIdx
				cmds.getAttr(coldef+'.collisionMeshList['+str(midx)+'].collisionMeshName')
				cmds.setAttr(coldef+'.collisionMeshList['+str(midx)+'].collisionMeshName', l=False)
				cmds.setAttr(coldef+'.collisionMeshList['+str(midx)+'].collisionMeshName', selection[idx], type='string')
				cmds.setAttr(coldef+'.collisionMeshList['+str(midx)+'].collisionMeshName', l=True)
				cmds.connectAttr(selection[idx]+'.worldMesh[0]', coldef+'.collisionMeshList['+str(midx)+'].collisionMesh', f=True)
		else:
			coldef = cmds.deformer(defmesh, type='chwCollision')[0]
			for idx in xrange(0, len(selection)):
				cmds.connectAttr(selection[idx]+'.worldMesh[0]', coldef+'.collisionMeshList['+str(idx)+'].collisionMesh', f=True)
				cmds.setAttr(coldef+'.collisionMeshList['+str(idx)+'].collisionMeshName', selection[idx], type='string')
				cmds.setAttr(coldef+'.collisionMeshList['+str(idx)+'].collisionMeshName', l=True)

