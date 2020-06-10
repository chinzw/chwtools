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

#include <chwCollision.h>

const MTypeId	chwCollision::typeId( 0x89006 );
const MString	chwCollision::typeName( "chwCollision" );

MObject 		chwCollision::aCollisionStrength;
MObject 		chwCollision::aCollisionMesh;
MObject 		chwCollision::aCollisionMeshName;
MObject 		chwCollision::aCollisionMeshList;
MObject 		chwCollision::aCollisionBulgeStrength;
MObject 		chwCollision::aCollisionBulgeDistance;
MObject 		chwCollision::aCollisionBulgeRamp;

chwCollision::chwCollision()
{
	m_data = new chwCollisionData;
}
chwCollision::~chwCollision()
{
	delete m_data;
}
void* chwCollision::creator(){ return new chwCollision(); }

MStatus chwCollision::initialize()
{
	MFnTypedAttribute		tAttr;
	MFnNumericAttribute		nAttr;
	MFnCompoundAttribute	cAttr;
	MRampAttribute			rAttr;
	MFnStringData			stringdata;

	aCollisionMesh = tAttr.create( "collisionMesh", "cm", MFnData::kMesh );
	tAttr.setStorable(false);
	tAttr.setConnectable(true);
	addAttribute( aCollisionMesh );

	aCollisionMeshName = tAttr.create( "collisionMeshName", "cmn", MFnData::kString, stringdata.create() );
	tAttr.setStorable(true);
	tAttr.setConnectable(true);
	tAttr.setKeyable(false);
	addAttribute( aCollisionMeshName );

	aCollisionStrength = nAttr.create( "collisionStrength", "cs", MFnNumericData::kFloat, 1.f );
	nAttr.setStorable(true);
	nAttr.setConnectable(true);
	nAttr.setKeyable(true);
	nAttr.setMin(0.f);
	nAttr.setMax(1.f);
	addAttribute( aCollisionStrength );

	aCollisionBulgeStrength = nAttr.create( "collisionBulgeStrength", "cbs", MFnNumericData::kFloat, 0.f );
	nAttr.setStorable(true);
	nAttr.setConnectable(true);
	nAttr.setKeyable(true);
	nAttr.setMin(0);
	addAttribute( aCollisionBulgeStrength );

	aCollisionBulgeDistance = nAttr.create( "collisionBulgeDistance", "cbd", MFnNumericData::kFloat, 0.f );
	nAttr.setStorable(true);
	nAttr.setConnectable(true);
	nAttr.setKeyable(true);
	nAttr.setMin(0);
	addAttribute( aCollisionBulgeDistance );

	aCollisionMeshList = cAttr.create("collisionMeshList", "cml");
	cAttr.setArray(true);
	cAttr.setUsesArrayDataBuilder(true);
	cAttr.addChild(aCollisionMeshName);
	cAttr.addChild(aCollisionStrength);
	cAttr.addChild(aCollisionBulgeStrength);
	cAttr.addChild(aCollisionBulgeDistance);
	cAttr.addChild(aCollisionMesh);
	cAttr.setStorable(true);
	cAttr.setReadable(true);
	cAttr.setConnectable(true);
	addAttribute(aCollisionMeshList);

	aCollisionBulgeRamp = rAttr.createCurveRamp("collisionBulgeRamp", "cbr");
	addAttribute(aCollisionBulgeRamp);

	attributeAffects( chwCollision::aCollisionStrength, chwCollision::outputGeom );
	attributeAffects( chwCollision::aCollisionMesh, chwCollision::outputGeom );
	attributeAffects( chwCollision::aCollisionMeshList, chwCollision::outputGeom );
	attributeAffects( chwCollision::aCollisionBulgeStrength, chwCollision::outputGeom );
	attributeAffects( chwCollision::aCollisionBulgeDistance, chwCollision::outputGeom );
	attributeAffects( chwCollision::aCollisionBulgeRamp, chwCollision::outputGeom );

	MGlobal::executeCommand("makePaintable -attrType multiFloat -sm deformer chwCollision weights");

	return MStatus::kSuccess;
}

MStatus chwCollision::compute(const MPlug& plug, MDataBlock& data)
{
	MStatus status;
	MObject thisNode = this->thisMObject();

	if (plug.attribute() == outputGeom)
	{
		float env = data.inputValue(envelope).asFloat();

		m_data->m_bulgeRampAttribute = MRampAttribute(thisNode, aCollisionBulgeRamp, &status);

		unsigned int index = plug.logicalIndex();
		MPlug inPlug(thisNode,input);
		inPlug.selectAncestorLogicalIndex(index,input);
		MDataHandle hInput = data.inputValue(inPlug, &status );
		CHECK_MSTATUS_AND_RETURN_IT(status);

		MDataHandle hGeom = hInput.child(inputGeom);
		MDataHandle hOutput = data.outputValue(plug);
		hOutput.copy(hGeom);
		MObject outMesh = hGeom.asMesh();
		MFnMesh meshFn(outMesh);

		status = meshFn.getPoints(m_data->m_points, MSpace::kWorld);
		CHECK_MSTATUS_AND_RETURN_IT(status);

		status = meshFn.getVertexNormals(false, m_data->m_normals, MSpace::kWorld);
		CHECK_MSTATUS_AND_RETURN_IT(status);

		// RESIZE ALL ARRAYS
		m_data->m_numpoints = m_data->m_points.length();
		m_data->m_weights.setLength(m_data->m_numpoints);
		m_data->m_collided.setLength(m_data->m_numpoints);

		if(env > 0)
		{
			for (unsigned int idx=0; idx<m_data->m_numpoints; idx++)
			{
				m_data->m_boxin.expand(m_data->m_points[idx]);
				m_data->m_weights[idx] = weightValue(data,index,idx) * env;
			}

			MArrayDataHandle arrayHandle = data.inputArrayValue(aCollisionMeshList, &status);
			unsigned int count = arrayHandle.elementCount();
			for (unsigned int midx=0; midx < count; midx++)
			{
				ThreadedCollide tc;
				tc.data = m_data;

				ThreadedBulge tb;
				tb.data = m_data;

				arrayHandle.jumpToArrayElement(midx);
				MDataHandle collisionMeshHnd = arrayHandle.inputValue(&status).child(aCollisionMesh);
				CHECK_MSTATUS_AND_RETURN_IT(status);

				MDataHandle collisionBulgeStrengthHnd = arrayHandle.inputValue(&status).child(aCollisionBulgeStrength);
				CHECK_MSTATUS_AND_RETURN_IT(status);

				MDataHandle collisionBulgeDistanceHnd = arrayHandle.inputValue(&status).child(aCollisionBulgeDistance);
				CHECK_MSTATUS_AND_RETURN_IT(status);

				MDataHandle collisionStrengthHnd = arrayHandle.inputValue(&status).child(aCollisionStrength);
				CHECK_MSTATUS_AND_RETURN_IT(status);

				m_data->m_collisionStrength = collisionStrengthHnd.asFloat();
				m_data->m_bulgeStrength = collisionBulgeStrengthHnd.asFloat();
				m_data->m_bulgeDistance = collisionBulgeDistanceHnd.asFloat();
				m_data->m_defmax = 0.f;

				if ((m_data->m_collisionStrength > 0.f) && !collisionMeshHnd.data().isNull())
				{
					MFloatPointArray pointsCol;
					MObject	colMesh = collisionMeshHnd.asMeshTransformed();
					tc.colFn = new MFnMesh(colMesh);

					status = tc.colFn->getPoints(pointsCol, MSpace::kWorld);
					CHECK_MSTATUS_AND_RETURN_IT(status);

					for (unsigned int idx=0; idx<pointsCol.length(); idx++)
					{
						m_data->m_boxcol.expand(pointsCol[idx]);
					}

					m_data->m_mmAccelParams = tc.colFn->autoUniformGridParams();
					m_data->m_intersector.create(colMesh);

					if (m_data->m_boxin.intersects(m_data->m_boxcol))
					{
						parallel_for( blocked_range<int>( 0, m_data->m_numpoints ), tc );
						parallel_for( blocked_range<int>( 0, m_data->m_numpoints ), tb );
					}
				}
			}
		}
		meshFn.setPoints(m_data->m_points, MSpace::kWorld);
	}
	data.setClean(plug);
	return MS::kSuccess;
}

void chwCollision::postConstructor()
{
	MStatus status;

	status = postConstructor_initialise_ramp_curve( aCollisionBulgeRamp, 0, 0.0f, 0.0f, 2 );
	CHECK_MSTATUS(status);
	status = postConstructor_initialise_ramp_curve( aCollisionBulgeRamp, 1, 0.3f, 0.8f, 2 );
	CHECK_MSTATUS(status);
	status = postConstructor_initialise_ramp_curve( aCollisionBulgeRamp, 2, 1.0f, 0.0f, 2 );
	CHECK_MSTATUS(status);
}

MStatus chwCollision::postConstructor_initialise_ramp_curve( MObject &rampObj, int index, float position, float value, int interpolation)
{
	MStatus status;

	MObject thisNode = this->thisMObject();

	MPlug rampPlug( thisNode, rampObj );
	MPlug elementPlug = rampPlug.elementByLogicalIndex( index, &status );

	MPlug positionPlug = elementPlug.child(0, &status);
	status = positionPlug.setFloat(position);

	MPlug valuePlug = elementPlug.child(1);
	status = valuePlug.setFloat(value);

	MPlug interpPlug = elementPlug.child(2);
	interpPlug.setInt(interpolation);

	return MS::kSuccess;
}
