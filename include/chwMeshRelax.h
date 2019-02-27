/*************************************************************************
* Copyright (c) 2018 Cristian Hinz Welkens.
*
* This file is part of chwTools (https://github.com/chinzw/chwtools).
*
* chwTools is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, version 3 of the License.
*
* chwTools is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with chwTools.  If not, see <https://www.gnu.org/licenses/>.
 *************************************************************************/

#ifndef CHWMESHRELAX_H
#define CHWMESHRELAX_H

#include <maya/MPxDeformerNode.h>
#include <maya/MItGeometry.h>
#include <maya/MTypeId.h>
#include <maya/MPlug.h>
#include <maya/MDataBlock.h>
#include <maya/MDataHandle.h>
#include <maya/MPoint.h>
#include <maya/MPointArray.h>
#include <maya/MFnMesh.h>
#include <maya/MVector.h>
#include <maya/MFloatVectorArray.h>
#include <maya/MGlobal.h>
#include <maya/MMatrix.h>
#include <maya/MFnNumericAttribute.h>
#include <maya/MIOStream.h>
#include <chwEdgeRelax.h>


class chwMeshRelax : public MPxDeformerNode
{
	public:
					chwMeshRelax();
			virtual ~chwMeshRelax();

			virtual MStatus 	compute(const MPlug& plug, MDataBlock& data);
			static  void* 		creator();
			static  MStatus 	initialize();

	public:
		static  MObject 		aIterations;
		static const MTypeId 	typeId;
		static const MString 	typeName;
	private:
		chwEdgeRelax m_relax;
};

#endif // CHWMESHRELAX_H
