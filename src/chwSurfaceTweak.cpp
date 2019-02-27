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

#include "chwSurfaceTweak.h"

const MTypeId chwSurfaceTweak::typeId( 0x89007 );
const MString chwSurfaceTweak::typeName( "chwSurfaceTweak" );

MObject    chwSurfaceTweak::aInVert;
MObject    chwSurfaceTweak::aFalloffMode;
MObject    chwSurfaceTweak::aFalloffCurve;
MObject    chwSurfaceTweak::aFalloffRadius;
MObject    chwSurfaceTweak::aRelativeMatrix;
MObject    chwSurfaceTweak::aDeformMatrix;

MObject    chwSurfaceTweak::aOutTranslate;
MObject    chwSurfaceTweak::aOutTranslateX;
MObject    chwSurfaceTweak::aOutTranslateY;
MObject    chwSurfaceTweak::aOutTranslateZ;

MObject    chwSurfaceTweak::aOutRotate;
MObject    chwSurfaceTweak::aOutRotateX;
MObject    chwSurfaceTweak::aOutRotateY;
MObject    chwSurfaceTweak::aOutRotateZ;


chwSurfaceTweak::chwSurfaceTweak()
{
	m_data = new chwSurfaceTweakData;
}

chwSurfaceTweak::~chwSurfaceTweak()
{
	delete m_data;
}

void* chwSurfaceTweak::creator(){ return new chwSurfaceTweak(); }


MStatus chwSurfaceTweak::initialize()
{
	MFnNumericAttribute		nAttr;
	MFnEnumAttribute		eAttr;
	MFnMatrixAttribute		mAttr;
	MRampAttribute			rAttr;
	MFnUnitAttribute		uAttr;

	aInVert = nAttr.create( "vertex", "v", MFnNumericData::kLong );
	nAttr.setKeyable(true);
	nAttr.setStorable(true);
	nAttr.setMin(0);
	nAttr.setDefault(0);
	nAttr.setReadable(true);
	nAttr.setWritable(true);
	addAttribute(aInVert);

	aFalloffMode = eAttr.create( "falloffMode", "fom" );
	eAttr.addField(	"Volume", 0);
	eAttr.setKeyable(true);
	eAttr.setStorable(true);
	eAttr.setReadable(true);
	eAttr.setWritable(true);
	eAttr.setDefault(0);
	addAttribute(aFalloffMode);

	aFalloffRadius = nAttr.create( "falloffRadius", "for", MFnNumericData::kFloat, 1.0 );
	nAttr.setWritable(true);
	nAttr.setStorable(true);
	nAttr.setKeyable (true);
	nAttr.setConnectable(true);
	nAttr.setMin(0.0);
	nAttr.setDefault(1.0);
	addAttribute(aFalloffRadius);

	aFalloffCurve = rAttr.createCurveRamp("falloffCurve", "foc");
	addAttribute(aFalloffCurve);

	aRelativeMatrix = mAttr.create( "relativeMatrix", "rm" );
	mAttr.setConnectable(true);
	addAttribute(aRelativeMatrix);

	aDeformMatrix = mAttr.create( "deformMatrix", "dm" );
	mAttr.setConnectable(true);
	addAttribute(aDeformMatrix);

	aOutTranslateX = nAttr.create( "outTranslateX", "ostx", MFnNumericData::kDouble, 0.0 );
	aOutTranslateY = nAttr.create( "outTranslateY", "osty", MFnNumericData::kDouble, 0.0 );
	aOutTranslateZ = nAttr.create( "outTranslateZ", "ostz", MFnNumericData::kDouble, 0.0 );
	aOutTranslate = nAttr.create( "outTranslate", "os", aOutTranslateX, aOutTranslateY, aOutTranslateZ );
	addAttribute(aOutTranslate);

	aOutRotateX = uAttr.create( "outRotateX", "osrx", MFnUnitAttribute::kAngle, 0.0 );
	aOutRotateY = uAttr.create( "outRotateY", "osry", MFnUnitAttribute::kAngle, 0.0 );
	aOutRotateZ = uAttr.create( "outRotateZ", "osrz", MFnUnitAttribute::kAngle, 0.0 );
	aOutRotate = nAttr.create( "outRotate", "osr", aOutRotateX, aOutRotateY, aOutRotateZ );
	addAttribute(aOutRotate);

	attributeAffects( chwSurfaceTweak::aInVert, chwSurfaceTweak::aOutTranslate );
	attributeAffects( chwSurfaceTweak::aInVert, chwSurfaceTweak::aOutRotate );

	attributeAffects( chwSurfaceTweak::inputGeom, chwSurfaceTweak::aOutTranslate );
	attributeAffects( chwSurfaceTweak::inputGeom, chwSurfaceTweak::aOutRotate );

	attributeAffects( chwSurfaceTweak::aRelativeMatrix, chwSurfaceTweak::outputGeom );
	attributeAffects( chwSurfaceTweak::aDeformMatrix, chwSurfaceTweak::outputGeom );
	attributeAffects( chwSurfaceTweak::aFalloffMode, chwSurfaceTweak::outputGeom );
	attributeAffects( chwSurfaceTweak::aFalloffCurve, chwSurfaceTweak::outputGeom );
	attributeAffects( chwSurfaceTweak::aFalloffRadius, chwSurfaceTweak::outputGeom );

	MGlobal::executeCommand( "makePaintable -attrType \"multiFloat\" -sm \"deformer\" \"stickySculpt\" \"weights\";" );

	return MStatus::kSuccess;
}


MStatus chwSurfaceTweak::compute(const MPlug& plug, MDataBlock& data)
{
	MStatus status;

	MObject thisNode = this->thisMObject();

	m_data->m_env = data.inputValue(envelope).asFloat();
	m_data->m_vert = data.inputValue( aInVert ).asInt();

	if (plug == aOutTranslate || plug == aOutRotate || plug.parent() == aOutTranslate || plug.parent() == aOutRotate )
	{
		MDataHandle outTranslateHnd = data.outputValue( aOutTranslate );
		MDataHandle outRotateHnd = data.outputValue( aOutRotate );

		MPlug inPlug(thisNode,input);
		inPlug.selectAncestorLogicalIndex(0,input);
		MDataHandle hInput = data.inputValue(inPlug, &status);
		CHECK_MSTATUS_AND_RETURN_IT(status);

		MDataHandle hGeom = hInput.child(inputGeom);
		MObject mesh = hGeom.asMesh();

		if (!mesh.isNull())
		{
			MFnMesh meshFn(mesh, &status);
			CHECK_MSTATUS_AND_RETURN_IT(status);

			m_data->m_numverts = meshFn.numVertices();
			status = meshFn.getPoints(m_data->m_points,  MSpace::kWorld);
			CHECK_MSTATUS_AND_RETURN_IT(status);

			status = meshFn.getVertexNormals(false, m_data->m_normals,  MSpace::kWorld);
			CHECK_MSTATUS_AND_RETURN_IT(status);

			int prev;
			MItMeshVertex iter(mesh);
			iter.setIndex(m_data->m_vert, prev);
			iter.getConnectedVertices(m_data->m_conids);

			if (m_data->m_conids.length() > 1)
			{
				m_data->m_aimvert = m_data->m_conids[0];
				m_data->m_upvert = m_data->m_conids[1];
			}
			else
			{
				return MStatus::kFailure;
			}

			if (m_data->m_vert < m_data->m_numverts)
			{
				MMatrix		m;
				MVector		vx, vy, vz;

				MPoint outTranslate;
				double outRotate[3];

				// VERTEX NORMAL
				vx = m_data->m_normals[m_data->m_vert];
				// TEMPORARY TANGENT
				vy = (m_data->m_points[m_data->m_upvert] - m_data->m_points[m_data->m_vert]).normal();
				// VERTEX BINORMAL
				vz = (vx ^ vy).normal();
				// VERTEX TANGENT
				vy = (vx ^ vz).normal();

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

				m[3][0] = m_data->m_points[m_data->m_vert][0];
				m[3][1] = m_data->m_points[m_data->m_vert][1];
				m[3][2] = m_data->m_points[m_data->m_vert][2];
				m[3][3] = 1.f;

				MTransformationMatrix outMatrix(m);
				MTransformationMatrix::RotationOrder order;

				outMatrix.getRotation(outRotate, order, MSpace::kWorld);
				outTranslate = outMatrix.getTranslation(MSpace::kWorld);

				outRotateHnd.set3Double(outRotate[0], outRotate[1], outRotate[2]);
				outTranslateHnd.set3Double(outTranslate[0],outTranslate[1],outTranslate[2]);
			}
			else
			{
				outTranslateHnd.set(0.0, 0.0, 0.0);
				outRotateHnd.set(0.0, 0.0, 0.0);
			}
		}
		outTranslateHnd.setClean();
		outRotateHnd.setClean();
	}
	else if (plug.attribute() == outputGeom)
	{
		unsigned int index = plug.logicalIndex();
		MPlug inPlug(thisNode,input);
		inPlug.selectAncestorLogicalIndex(index,input);
		MDataHandle hInput = data.inputValue(inPlug);

		MDataHandle hGeom = hInput.child(inputGeom);
		m_data->m_localToWorldMatrix = hGeom.geometryTransformMatrix();
		MDataHandle hOutput = data.outputValue(plug);
		hOutput.copy(hGeom);
		MObject mesh = hGeom.asMesh();

		if (!mesh.isNull())
		{
			if (m_data->m_env > 0.0)
			{
				m_data->m_falloffMode = data.inputValue( aFalloffMode ).asShort();
				m_data->m_falloffRadius = data.inputValue( aFalloffRadius ).asFloat();
				m_data->m_falloffCurve = MRampAttribute(thisNode, aFalloffCurve, &status);

				m_data->m_relativeMatrix = data.inputValue( aRelativeMatrix ).asMatrix();
				m_data->m_deformMatrix = data.inputValue( aDeformMatrix ).asMatrix();
				m_data->m_relativeInverseMatrix = m_data->m_relativeMatrix.inverse();
				m_data->m_localToWorldMatrixInverse = m_data->m_localToWorldMatrix.inverse();

				MTransformationMatrix defMat(m_data->m_relativeMatrix);
				m_data->m_relativeMatrixTranslate = defMat.translation(MSpace::kWorld);

				MFnMesh meshFn(mesh, &status);
				CHECK_MSTATUS_AND_RETURN_IT(status);

				m_data->m_numverts = meshFn.numVertices();
				status = meshFn.getPoints(m_data->m_points,  MSpace::kWorld);
				CHECK_MSTATUS_AND_RETURN_IT(status);

				m_data->m_weights.setLength(m_data->m_numverts);
				for (unsigned int idx=0; idx < m_data->m_numverts; idx++)
				{
					m_data->m_weights[idx]= weightValue(data,index,idx);
				}

				// THREADED DEFORMATION
				ThreadedDeform td;
				td.data = m_data;
				parallel_for( blocked_range<int>( 0, m_data->m_numverts ), td );

				status = meshFn.setPoints(m_data->m_points,  MSpace::kWorld);
			}
		}
	}
	data.setClean(plug);

	return status;
}


MStatus chwSurfaceTweak::postConstructor_initialise_ramp_curve( MObject &rampObj, int index, float position, float value, int interpolation)
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

void chwSurfaceTweak::postConstructor()
{
	MStatus status;

	status = postConstructor_initialise_ramp_curve( aFalloffCurve, 0, 0.0f, 1.0f, 2 );
	CHECK_MSTATUS(status);
	status = postConstructor_initialise_ramp_curve( aFalloffCurve, 1, 1.0f, 0.0f, 2 );
	CHECK_MSTATUS(status);
}
