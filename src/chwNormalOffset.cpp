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

#include <chwNormalOffset.h>

const MTypeId chwNormalOffset::typeId( 0x89003 );
const MString chwNormalOffset::typeName( "chwNormalOffset" );

MObject     chwNormalOffset::aStrength;
MObject     chwNormalOffset::aNormalSmooth;

chwNormalOffset::chwNormalOffset()
{
	m_data = new chwNormalOffsetData;
}

chwNormalOffset::~chwNormalOffset()
{
	delete m_data;
}

void* chwNormalOffset::creator()
{
	return new chwNormalOffset();
}


MStatus chwNormalOffset::initialize()
{
	MFnNumericAttribute  nAttr;

	aStrength = nAttr.create( "strength", "str", MFnNumericData::kFloat );
	nAttr.setKeyable(true);
	nAttr.setStorable(true);
	nAttr.setWritable(true);
	nAttr.setConnectable(true);
	addAttribute( aStrength );

	aNormalSmooth = nAttr.create( "normalSmooth", "ns", MFnNumericData::kInt );
	nAttr.setKeyable(true);
	nAttr.setStorable(true);
	nAttr.setWritable(true);
	nAttr.setConnectable(true);
	nAttr.setMin(0);
	addAttribute( aNormalSmooth );

	attributeAffects( chwNormalOffset::aStrength, chwNormalOffset::outputGeom );
	attributeAffects( chwNormalOffset::aNormalSmooth, chwNormalOffset::outputGeom );

	MGlobal::executeCommand("makePaintable -attrType multiFloat -sm deformer chwNormalOffset weights");

	return MStatus::kSuccess;
}

MStatus chwNormalOffset::compute(const MPlug& plug, MDataBlock& data)
{

	MStatus status;
	m_data->m_env = data.inputValue(envelope).asFloat();
	m_data->m_strength = data.inputValue(aStrength).asFloat();
	m_data->m_smooth = data.inputValue(aNormalSmooth).asInt();

	if (plug.attribute() == outputGeom)
	{
		unsigned int index = plug.logicalIndex();
		MObject thisNode = this->thisMObject();
		MPlug inPlug(thisNode,input);
		inPlug.selectAncestorLogicalIndex(index,input);
		MDataHandle hInput = data.inputValue(inPlug, &status );
		CHECK_MSTATUS_AND_RETURN_IT(status);

		MDataHandle hGeom = hInput.child(inputGeom);
		MMatrix m = hGeom.geometryTransformMatrix();
		MDataHandle hOutput = data.outputValue(plug);
		hOutput.copy(hGeom);
		MObject inMesh = hGeom.asMesh();
		MFnMesh meshFn(inMesh, &status);
		CHECK_MSTATUS_AND_RETURN_IT(status);

		meshFn.getPoints(m_data->m_points, MSpace::kObject);
		m_data->m_numverts = meshFn.numVertices();

		if(m_data->m_env != 0.00 && m_data->m_strength != 0.00)
		{
			m_data->m_weights.setLength(m_data->m_numverts);
			for (unsigned int idx=0; idx < m_data->m_numverts; idx++)
			{
				m_data->m_weights[idx] = weightValue(data,index,idx) * m_data->m_env * m_data->m_strength;
			}
			if (m_data->m_smooth > 0)
			{
				MObject relaxMesh(inMesh);
				m_relax.relax(relaxMesh, m_data->m_smooth);
				MFnMesh relaxFn(relaxMesh, &status);
				CHECK_MSTATUS_AND_RETURN_IT(status);

				relaxFn.getVertexNormals(false, m_data->m_normals, MSpace::kObject);
			}
			else
			{
				meshFn.getVertexNormals(false, m_data->m_normals, MSpace::kObject);
			}

			// THREADED DEFORMATION
			ThreadedDeform td;
			td.data = m_data;
			parallel_for( blocked_range<int>( 0, m_data->m_numverts ), td );
		}
		meshFn.setPoints(m_data->m_points, MSpace::kObject);
	}
	data.setClean(plug);
	return MS::kSuccess;
}
