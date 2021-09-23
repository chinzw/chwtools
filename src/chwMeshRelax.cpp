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

#include <chwMeshRelax.h>

const MTypeId	chwMeshRelax::typeId( 0x00139200, 2 );
const MString	chwMeshRelax::typeName( "chwMeshRelax" );

MObject 		chwMeshRelax::aIterations;

chwMeshRelax::chwMeshRelax(){}
chwMeshRelax::~chwMeshRelax(){}
void* chwMeshRelax::creator(){ return new chwMeshRelax(); }

MStatus chwMeshRelax::initialize()
{
	MFnNumericAttribute  nAttr;

	aIterations = nAttr.create( "iterations", "it", MFnNumericData::kInt );
	nAttr.setKeyable(true);
	nAttr.setStorable(true);
	nAttr.setWritable(true);
	nAttr.setConnectable(true);
	nAttr.setMin(0);
	addAttribute( aIterations );

	attributeAffects( chwMeshRelax::aIterations, chwMeshRelax::outputGeom );

	MGlobal::executeCommand("makePaintable -attrType multiFloat -sm deformer chwMeshRelax weights");

	return MStatus::kSuccess;
}

MStatus chwMeshRelax::compute(const MPlug& plug, MDataBlock& data)
{
	MStatus stat;
	MPointArray pts;
	MPointArray rpts;
	float env = data.inputValue(envelope).asFloat();
	unsigned int iterations = data.inputValue(aIterations).asInt();
	unsigned int vertCount;

	if (plug.attribute() == outputGeom)
	{
		unsigned int index = plug.logicalIndex();
		MObject thisNode = this->thisMObject();
		MPlug inPlug(thisNode,input);
		inPlug.selectAncestorLogicalIndex(index,input);
		MDataHandle hInput = data.inputValue(inPlug, &stat );
		CHECK_MSTATUS_AND_RETURN_IT(stat)

		MDataHandle hGeom = hInput.child(inputGeom);
		MMatrix m = hGeom.geometryTransformMatrix();
		MDataHandle hOutput = data.outputValue(plug);
		hOutput.copy(hGeom);
		MObject outMesh = hGeom.asMesh();
		MFnMesh meshFn(outMesh);
		vertCount = meshFn.numVertices();
		meshFn.getPoints(pts, MSpace::kObject);
		CHECK_MSTATUS_AND_RETURN_IT(stat)

		MFloatArray w(vertCount);
		if(env != 0.00 && iterations != 0)
		{
			for (unsigned int idx=0; idx < vertCount; idx++)
			{
				w[idx] = weightValue(data,index,idx) * env;
			}
			MObject relaxMesh(outMesh);
			m_relax.relax(relaxMesh, iterations, rpts, w);
			meshFn.setPoints(rpts, MSpace::kObject);
		}
	}
	data.setClean(plug);
	return MS::kSuccess;
}
