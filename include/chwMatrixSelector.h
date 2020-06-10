/***********************************************************************************
* Copyright (c) 2018 Cristian Hinz Welkens.
*
* This file is part of chwTools (https://github.com/chinzw/chwtools).
*
* Permission is hereby granted, free of charge, to any person obtaining a copy of
* this software and associated documentation files (the "Software"), to deal in
* the Software without restriction, including without limitation the rights to 
* use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
* of the Software, and to permit persons to whom the Software is furnished to do
* so, subject to the following conditions:
*
* The above copyright notice and this permission notice shall be included in all
* copies or substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,
* INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A
* PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
* HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION
* OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
* SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
***********************************************************************************/

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
