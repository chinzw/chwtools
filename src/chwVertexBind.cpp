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

#include <chwVertexBind.h>

MTypeId 		chwVertexBind::typeId ( 0x89001 );
const MString 	chwVertexBind::typeName( "chwVertexBind" );

MObject 		chwVertexBind::aDriverMesh;
MObject 		chwVertexBind::aSearchDistance;
MObject 		chwVertexBind::aCutoffDistance;
MObject 		chwVertexBind::aInitialize;
MObject 		chwVertexBind::aVertexMap;

chwVertexBind::chwVertexBind()
{
	m_data = new chwVertexBindData;
}
chwVertexBind::~chwVertexBind()
{
	delete m_data;
}
void* chwVertexBind::creator(){ return new chwVertexBind(); }

MStatus chwVertexBind::initialize()
{
	MFnTypedAttribute 		tAttr;
	MFnEnumAttribute 		eAttr;
	MFnNumericAttribute 	nAttr;

	aDriverMesh = tAttr.create( "driverMesh", "drv", MFnData::kMesh );
	tAttr.setStorable(false);
	tAttr.setConnectable(true);
	addAttribute( aDriverMesh );

	aSearchDistance = nAttr.create( "SearchDistance", "sd", MFnNumericData::kFloat, 0.01);
	nAttr.setStorable(true);
	nAttr.setReadable(true);
	nAttr.setWritable(true);
	nAttr.setKeyable(true);
	nAttr.setMin(0.0);
	addAttribute(aSearchDistance);

	aCutoffDistance = nAttr.create( "Cutoff", "cof", MFnNumericData::kFloat, 0.0);
	nAttr.setStorable(true);
	nAttr.setReadable(true);
	nAttr.setWritable(true);
	nAttr.setKeyable(true);
	nAttr.setMin(0.0);
	addAttribute(aCutoffDistance);

	aInitialize = eAttr.create( "initialize", "inl" );
	eAttr.addField(	"Off", 0);
	eAttr.addField(	"Re-Set Bind", 1);
	eAttr.addField(	"Bound", 2);
	eAttr.setKeyable(true);
	eAttr.setStorable(true);
	eAttr.setReadable(true);
	eAttr.setWritable(true);
	eAttr.setDefault(0);
	addAttribute( aInitialize );

	aVertexMap = nAttr.create( "vtxIndexMap", "vtximp", MFnNumericData::kLong, -9999 );
	nAttr.setKeyable(false);
	nAttr.setArray(true);
	nAttr.setStorable(true);
	nAttr.setReadable(true);
	nAttr.setWritable(true);
	nAttr.setUsesArrayDataBuilder(true);
	addAttribute( aVertexMap );

	attributeAffects(chwVertexBind::aDriverMesh, 	chwVertexBind::outputGeom);
	attributeAffects(chwVertexBind::aSearchDistance,	chwVertexBind::outputGeom);
	attributeAffects(chwVertexBind::aCutoffDistance,	chwVertexBind::outputGeom);
	attributeAffects(chwVertexBind::aInitialize, 	chwVertexBind::outputGeom);
	attributeAffects(chwVertexBind::aVertexMap,		chwVertexBind::outputGeom);

	MGlobal::executeCommand( "makePaintable -attrType \"multiFloat\" -sm \"deformer\" \"vertSnap\" \"weights\";" );

	return MStatus::kSuccess;
}

MStatus chwVertexBind::deform( MDataBlock& data, MItGeometry& iter, const MMatrix& localToWorldMatrix, unsigned int mIndex)
{
	MStatus status;
	short initialized_mapping = data.inputValue( aInitialize ).asShort();

	if( initialized_mapping == 1 )
	{
		status = initializeMapping(data, iter, localToWorldMatrix);
		initialized_mapping = data.inputValue( aInitialize ).asShort();
	}
	if( initialized_mapping == 2 )
	{
		float env = data.inputValue(envelope).asFloat();
		MArrayDataHandle vertMapArrayData  = data.inputArrayValue( aVertexMap );

		MDataHandle meshAttrHandle = data.inputValue( aDriverMesh, &status );
		MItGeometry drvIter( meshAttrHandle );

		drvIter.allPositions(m_data->m_drvpoints, MSpace::kWorld);
		iter.allPositions(m_data->m_defpoints);
		m_data->m_mapping.setLength(m_data->m_defpoints.length());
		m_data->m_weights.setLength(m_data->m_defpoints.length());
		m_data->m_localToWorldMatrix = localToWorldMatrix;

		for ( unsigned int idx = 0; idx < m_data->m_defpoints.length(); idx++ )
		{
			vertMapArrayData.jumpToElement(idx);
			m_data->m_mapping[idx] = vertMapArrayData.inputValue().asInt();
			m_data->m_weights[idx] = weightValue( data, mIndex, idx ) * env;
		}

		ThreadedDeform td;
		td.data = m_data;
		parallel_for( blocked_range<int>( 0, m_data->m_defpoints.length() ), td );

		iter.setAllPositions(m_data->m_defpoints);
	}
	return status;
}



MStatus chwVertexBind::initializeMapping( MDataBlock& data, MItGeometry& iter, const MMatrix& localToWorldMatrix)
{
	MStatus status;

	MDataHandle cutoffDistanceHnd = data.inputValue(aCutoffDistance, &status);
	m_data->m_cutoffDistance = cutoffDistanceHnd.asFloat();

	MDataHandle searchDistanceHnd = data.inputValue(aSearchDistance, &status);
	m_data->m_searchDistance = searchDistanceHnd.asFloat();

	MDataHandle meshAttrHandle = data.inputValue( aDriverMesh, &status );
	MItGeometry drvIter( meshAttrHandle );

	drvIter.allPositions(m_data->m_drvpoints, MSpace::kWorld);
	iter.allPositions(m_data->m_defpoints);
	m_data->m_mapping.setLength(m_data->m_defpoints.length());
	m_data->m_localToWorldMatrix = localToWorldMatrix;

	ThreadedInitialize ti;
	ti.data = m_data;
	parallel_for( blocked_range<int>( 0, m_data->m_defpoints.length() ), ti );

	MArrayDataHandle vertMapOutArrayHandle  =  data.outputArrayValue( aVertexMap, &status );
	CHECK_MSTATUS(status);
	MArrayDataBuilder vertMapOutArrayBuilder = vertMapOutArrayHandle.builder(&status);
	CHECK_MSTATUS(status);

	for ( unsigned int idx = 0; idx < m_data->m_defpoints.length(); idx++ )
	{
		MDataHandle initIndexData = vertMapOutArrayBuilder.addElement( idx, &status );
		CHECK_MSTATUS(status);
		initIndexData.setInt(m_data->m_mapping[idx]);
		initIndexData.setClean();
	}

	vertMapOutArrayHandle.set( vertMapOutArrayBuilder );

	MObject tObj  =  thisMObject();
	MPlug setInitMode( tObj, aInitialize );
	setInitMode.setValue( 2 );

	data.setClean(aVertexMap);
	return status;
}

MObject& chwVertexBind::accessoryAttribute() const
{
	return chwVertexBind::aDriverMesh ;
}
