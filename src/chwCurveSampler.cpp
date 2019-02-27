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

#include <chwCurveSampler.h>

#define _USE_MATH_DEFINES

const MTypeId	chwCurveSampler::typeId( 0x89005 );
const MString	chwCurveSampler::typeName( "chwCurveSampler" );

MObject 		chwCurveSampler::aInNumSamples;
MObject 		chwCurveSampler::aInCurve;
MObject 		chwCurveSampler::aInUpCurve;
MObject 		chwCurveSampler::aOutRotate;
MObject 		chwCurveSampler::aOutRotateX;
MObject 		chwCurveSampler::aOutRotateY;
MObject 		chwCurveSampler::aOutRotateZ;
MObject 		chwCurveSampler::aOutTranslate;
MObject 		chwCurveSampler::aOutTranslateX;
MObject 		chwCurveSampler::aOutTranslateY;
MObject 		chwCurveSampler::aOutTranslateZ;

chwCurveSampler::chwCurveSampler(){}
chwCurveSampler::~chwCurveSampler(){}
void* chwCurveSampler::creator(){ return new chwCurveSampler(); }

MStatus chwCurveSampler::compute(const MPlug& plug, MDataBlock& data)
{
	MStatus status;

	if(plug == aOutRotate || plug == aOutTranslate || plug.parent() == aOutRotate || plug.parent() == aOutTranslate) {
		// GET NUM SAMPLES
		unsigned int inNumSamples = data.inputValue(aInNumSamples).asInt();

		// SETUP ARRAY DATA HANDLE
		MArrayDataHandle outRotateArray = data.outputArrayValue(aOutRotate, &status);
		CHECK_MSTATUS_AND_RETURN_IT(status);

		MArrayDataHandle outTranslateArray = data.outputArrayValue(aOutTranslate, &status);
		CHECK_MSTATUS_AND_RETURN_IT(status);

		// SETUP ARRAY DATA BUILDER
		MArrayDataBuilder outRotateBuilder(aOutRotate, inNumSamples, &status);
		CHECK_MSTATUS_AND_RETURN_IT(status);

		MArrayDataBuilder outTranslateBuilder(aOutTranslate, inNumSamples, &status);
		CHECK_MSTATUS_AND_RETURN_IT(status);

		// GET INPUT CURVE
		MDataHandle inCurveHnd = data.inputValue(aInCurve, &status);
		CHECK_MSTATUS_AND_RETURN_IT(status);
		MObject inCurveObj ( inCurveHnd.asNurbsCurveTransformed() );
		MFnNurbsCurve inCurveFn ( inCurveObj, &status );
		CHECK_MSTATUS_AND_RETURN_IT(status);

		// GET INPUT UP CURVE
		MDataHandle inUpCurveHnd = data.inputValue(aInUpCurve, &status);
		CHECK_MSTATUS_AND_RETURN_IT(status);
		MObject inUpCurveObj ( inUpCurveHnd.asNurbsCurveTransformed() );
		MFnNurbsCurve inUpCurveFn ( inUpCurveObj, &status );
		CHECK_MSTATUS_AND_RETURN_IT(status);

		double min, max = 0.f;
		double param = 0.f;
		MVector		inCurveTangent(0.f,0.f,0.f);
		MPoint		inCurvePoint(0.f,0.f,0.f);
		MPoint		inUpCurvePoint(0.f,0.f,0.f);

		MVector		vx, vy, vz;

		MPoint outTranslate;
		double outRotate[3];

		inCurveFn.getKnotDomain(min, max);

		float step = max / (inNumSamples-1);

		for (unsigned int idx=0; idx < inNumSamples; idx++)
		{
			param = min + (idx * step);
			inCurveTangent = inCurveFn.tangent(param, MSpace::kWorld);
			inCurveFn.getPointAtParam(param, inCurvePoint, MSpace::kWorld);
			inUpCurveFn.getPointAtParam(param, inUpCurvePoint, MSpace::kWorld);


			MMatrix m;

			vx = inCurveTangent.normal();
			vy = (inUpCurvePoint - inCurvePoint).normal();
			vz = (vx ^ vy).normal();

			m[0][0] = vx[0];
			m[0][1] = vx[1];
			m[0][2] = vx[2];
			m[0][3] = 0.f;

			m[1][0] = vy[0];
			m[1][1] = vy[1];
			m[1][2] = vy[2];
			m[1][3] = 0.f;

			m[2][0] = vz[0];
			m[2][1] = vz[1];
			m[2][2] = vz[2];
			m[2][3] = 0.f;

			m[3][0] = inCurvePoint[0];
			m[3][1] = inCurvePoint[1];
			m[3][2] = inCurvePoint[2];
			m[3][3] = 1.f;

			MDataHandle outRotateHnd = outRotateBuilder.addElement(idx);
			MDataHandle outTranslateHnd = outTranslateBuilder.addElement(idx);

			MTransformationMatrix outMatrix(m);
			MTransformationMatrix::RotationOrder order;

			outMatrix.getRotation(outRotate, order, MSpace::kWorld);
			outTranslate = outMatrix.getTranslation(MSpace::kWorld);

			outRotateHnd.set3Double(outRotate[0], outRotate[1], outRotate[2]);
			outTranslateHnd.set3Double(outTranslate[0],outTranslate[1],outTranslate[2]);
		}

		// SET BUILDER INTO ARRAY
		status = outRotateArray.set(outRotateBuilder);
		CHECK_MSTATUS(status);

		status = outTranslateArray.set(outTranslateBuilder);
		CHECK_MSTATUS(status);

		// SET CLEAN THE WHOLE ARRAY
		status = outRotateArray.setAllClean();
		CHECK_MSTATUS(status);

		status = outTranslateArray.setAllClean();
		CHECK_MSTATUS(status);

	} else {
		return MS::kUnknownParameter;
	}
	return status;
}


MStatus chwCurveSampler::initialize()
{
	MFnNumericAttribute 	nAttr;
	MFnUnitAttribute		uAttr;
	MFnTypedAttribute		tAttr;

	aInNumSamples = nAttr.create( "numSamples", "ns", MFnNumericData::kInt );
	nAttr.setKeyable(true);
	nAttr.setStorable(true);
	nAttr.setWritable(true);
	nAttr.setConnectable(true);
	nAttr.setMin(0);
	addAttribute( aInNumSamples );

	aInCurve = tAttr.create( "inCurve", "ic", MFnData::kNurbsCurve);
	tAttr.setStorable(false);
	tAttr.setConnectable(true);
	addAttribute( aInCurve );

	aInUpCurve = tAttr.create("inUpCurve", "uic", MFnData::kNurbsCurve);
	tAttr.setStorable(true);
	tAttr.setConnectable(true);
	addAttribute(aInUpCurve);

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
	nAttr.setArray(true);
	nAttr.setWritable(true);
	nAttr.setStorable(true);
	nAttr.setConnectable(true);
	nAttr.setUsesArrayDataBuilder(true);
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
	nAttr.setArray(true);
	nAttr.setWritable(true);
	nAttr.setStorable(true);
	nAttr.setConnectable(true);
	nAttr.setUsesArrayDataBuilder(true);
	addAttribute( aOutTranslate );

	attributeAffects( chwCurveSampler::aInCurve,			chwCurveSampler::aOutRotate );
	attributeAffects( chwCurveSampler::aInUpCurve,			chwCurveSampler::aOutRotate );
	attributeAffects( chwCurveSampler::aInNumSamples,		chwCurveSampler::aOutRotate );

	attributeAffects( chwCurveSampler::aInCurve,			chwCurveSampler::aOutTranslate );
	attributeAffects( chwCurveSampler::aInUpCurve,			chwCurveSampler::aOutTranslate );
	attributeAffects( chwCurveSampler::aInNumSamples,		chwCurveSampler::aOutTranslate );

	return MStatus::kSuccess;
}
