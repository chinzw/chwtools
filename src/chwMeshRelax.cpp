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

#include <chwMeshRelax.h>

const MTypeId	chwMeshRelax::typeId( 0x89002 );
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
