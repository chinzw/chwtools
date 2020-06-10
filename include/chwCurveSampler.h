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

#ifndef CHWCURVESAMPLER_H
#define CHWCURVESAMPLER_H

#include <math.h>
#include <maya/MPxNode.h>
#include <maya/MTypeId.h>
#include <maya/MPlug.h>
#include <maya/MPlugArray.h>
#include <maya/MDataBlock.h>
#include <maya/MDataHandle.h>
#include <maya/MGlobal.h>
#include <maya/MMatrix.h>
#include <maya/MFnNumericAttribute.h>
#include <maya/MFnUnitAttribute.h>
#include <maya/MFnTypedAttribute.h>
#include <maya/MQuaternion.h>
#include <maya/MIOStream.h>
#include <maya/MEulerRotation.h>
#include <maya/MFnNurbsCurve.h>
#include <maya/MArrayDataBuilder.h>
#include <maya/MArrayDataHandle.h>
#include <maya/MVector.h>

class chwCurveSampler : public MPxNode
{
	public:
				chwCurveSampler();
		virtual ~chwCurveSampler();

		virtual MStatus 	compute(const MPlug& plug, MDataBlock& data);
		static  void* 		creator();
		static  MStatus 	initialize();

	public:
		static	MObject			aInCurve;
		static	MObject			aInUpCurve;
		static	MObject			aInNumSamples;
		static	MObject			aOutRotate;
		static	MObject			aOutRotateX;
		static	MObject			aOutRotateY;
		static	MObject			aOutRotateZ;
		static	MObject			aOutTranslate;
		static	MObject			aOutTranslateX;
		static	MObject			aOutTranslateY;
		static	MObject			aOutTranslateZ;

		static const MTypeId 	typeId;
		static const MString 	typeName;
};

#endif // CHWCURVESAMPLER_H
