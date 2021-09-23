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

#include <chwMatrixSelector.h>

#define _USE_MATH_DEFINES

const MTypeId	chwMatrixSelector::typeId( 0x00139200, 4 );
const MString	chwMatrixSelector::typeName( "chwMatrixSelector" );

MObject 		chwMatrixSelector::aInMatrix;
MObject 		chwMatrixSelector::aInMatrixList;
MObject 		chwMatrixSelector::aOutRotate;
MObject 		chwMatrixSelector::aOutRotateX;
MObject 		chwMatrixSelector::aOutRotateY;
MObject 		chwMatrixSelector::aOutRotateZ;
MObject 		chwMatrixSelector::aOutTranslate;
MObject 		chwMatrixSelector::aOutTranslateX;
MObject 		chwMatrixSelector::aOutTranslateY;
MObject 		chwMatrixSelector::aOutTranslateZ;
MObject 		chwMatrixSelector::aInMatrixIndex;

chwMatrixSelector::chwMatrixSelector(){}
chwMatrixSelector::~chwMatrixSelector(){}
void* chwMatrixSelector::creator(){ return new chwMatrixSelector(); }

MStatus chwMatrixSelector::compute(const MPlug& plug, MDataBlock& data)
{
	MStatus status;

	if(plug == aOutRotate || plug == aOutTranslate || plug.parent() == aOutRotate || plug.parent() == aOutTranslate) {
		unsigned int matrixIndex = data.inputValue( aInMatrixIndex ).asShort();
		MArrayDataHandle arrayHandle = data.inputArrayValue(aInMatrixList, &status);
		if (status == MS::kSuccess) {
			unsigned int count = arrayHandle.elementCount();
			if (matrixIndex<count) {
				arrayHandle.jumpToArrayElement(matrixIndex);
				MDataHandle inputHandle = arrayHandle.inputValue(&status).child(aInMatrix);
				if (status == MS::kSuccess) {
					MTransformationMatrix matrix = inputHandle.asMatrix();

					// EXTRACT ROTATION
					MDataHandle rotateHnd = data.outputValue(aOutRotate);
					MEulerRotation erotation = matrix.eulerRotation();
					rotateHnd.set3Double(erotation.x, erotation.y, erotation.z);
					rotateHnd.setClean();

					// EXTRACT TRANSLATION
					MDataHandle translateHnd = data.outputValue(aOutTranslate);
					MVector pos = matrix.getTranslation(MSpace::kWorld);
					translateHnd.set3Double(double(pos.x),double(pos.y),double(pos.z));
					translateHnd.setClean();
				} else {
					MGlobal::displayError("Failed to get Input Handle!");
					return MS::kFailure;
				}
			} else {
				//MGlobal::displayError("Index out of bounds!");
				return MS::kFailure;
			}
		} else {
			MGlobal::displayError("Failed to get InputArray Handle!");
			return MS::kFailure;
		}
	} else {
		return MS::kUnknownParameter;
	}
	data.setClean(plug);
	return MS::kSuccess;
}


MStatus chwMatrixSelector::initialize()
{
	MFnNumericAttribute 	nAttr;
	MFnMatrixAttribute 	mAttr;
	MFnCompoundAttribute    cAttr;
	MFnUnitAttribute uAttr;

	aInMatrixIndex = nAttr.create( "matrixIndex", "mi", MFnNumericData::kInt );
	nAttr.setKeyable(true);
	nAttr.setStorable(true);
	nAttr.setWritable(true);
	nAttr.setConnectable(true);
	nAttr.setMin(0);
	addAttribute( aInMatrixIndex );

	aInMatrix = mAttr.create( "inMatrix", "im");
	mAttr.setStorable(false);
	mAttr.setConnectable(true);
	addAttribute( aInMatrix );

	aInMatrixList = cAttr.create("inMatrixList", "iml");
	cAttr.setArray(true);
	cAttr.addChild(aInMatrix);
	cAttr.setReadable(true);
	cAttr.setUsesArrayDataBuilder(true);
	cAttr.setConnectable(true);
	addAttribute(aInMatrixList);

	aOutRotateX = uAttr.create( "outRotateX", "orx", MFnUnitAttribute::kAngle, 0.0);
	uAttr.setWritable(true);
	uAttr.setStorable(true);
	uAttr.setConnectable(true);
	aOutRotateY = uAttr.create( "outRotateY", "ory", MFnUnitAttribute::kAngle, 0.0);
	uAttr.setWritable(true);
	uAttr.setStorable(true);
	uAttr.setConnectable(true);
	aOutRotateZ = uAttr.create( "outRotateZ", "orz", MFnUnitAttribute::kAngle, 0.0);
	uAttr.setWritable(true);
	uAttr.setStorable(true);
	uAttr.setConnectable(true);
	aOutRotate = nAttr.create( "outRotate", "or", aOutRotateX, aOutRotateY, aOutRotateZ);
	nAttr.setWritable(true);
	nAttr.setStorable(true);
	nAttr.setConnectable(true);
	addAttribute( aOutRotate );

	aOutTranslateX = nAttr.create( "outTranslateX", "otx", MFnNumericData::kDouble, 0.0);
	nAttr.setWritable(true);
	nAttr.setStorable(true);
	nAttr.setConnectable(true);
	aOutTranslateY = nAttr.create( "outTranslateY", "oty", MFnNumericData::kDouble, 0.0);
	nAttr.setWritable(true);
	nAttr.setStorable(true);
	nAttr.setConnectable(true);
	aOutTranslateZ = nAttr.create( "outTranslateZ", "otz", MFnNumericData::kDouble, 0.0);
	nAttr.setWritable(true);
	nAttr.setStorable(true);
	nAttr.setConnectable(true);
	aOutTranslate = nAttr.create( "outTranslate", "ot", aOutTranslateX, aOutTranslateY, aOutTranslateZ);
	addAttribute( aOutTranslate );

	attributeAffects( chwMatrixSelector::aInMatrixIndex,		chwMatrixSelector::aOutRotate );
	attributeAffects( chwMatrixSelector::aInMatrix,			chwMatrixSelector::aOutRotate );
	attributeAffects( chwMatrixSelector::aInMatrixList,		chwMatrixSelector::aOutRotate );

	attributeAffects( chwMatrixSelector::aInMatrixIndex,		chwMatrixSelector::aOutTranslate );
	attributeAffects( chwMatrixSelector::aInMatrix,			chwMatrixSelector::aOutTranslate );
	attributeAffects( chwMatrixSelector::aInMatrixList,		chwMatrixSelector::aOutTranslate );

	return MStatus::kSuccess;
}
