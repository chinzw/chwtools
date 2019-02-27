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

#ifndef CHWMATRIXSELECTOR_H
#define CHWMATRIXSELECTOR_H

#include <math.h>
#include <maya/MPxNode.h>
#include <maya/MItGeometry.h>
#include <maya/MTypeId.h>
#include <maya/MPlug.h>
#include <maya/MPlugArray.h>
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
#include <maya/MFnMatrixAttribute.h>
#include <maya/MFnEnumAttribute.h>
#include <maya/MFnCompoundAttribute.h>
#include <maya/MFnUnitAttribute.h>
#include <maya/MQuaternion.h>
#include <maya/MIOStream.h>
#include <maya/MEulerRotation.h>

class chwMatrixSelector : public MPxNode
{
	public:
				chwMatrixSelector();
		virtual ~chwMatrixSelector();

		virtual MStatus 	compute(const MPlug& plug, MDataBlock& data);
		static  void* 		creator();
		static  MStatus 	initialize();

	public:
		static	MObject			aInMatrix;
		static	MObject			aInMatrixList;
		static	MObject			aOutRotate;
		static	MObject			aOutRotateX;
		static	MObject			aOutRotateY;
		static	MObject			aOutRotateZ;
		static	MObject			aOutTranslate;
		static	MObject			aOutTranslateX;
		static	MObject			aOutTranslateY;
		static	MObject			aOutTranslateZ;
		static	MObject			aInMatrixIndex;


		static const MTypeId 	typeId;
		static const MString 	typeName;
};

#endif // CHWMATRIXSELECTOR_H
