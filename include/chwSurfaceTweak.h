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

#ifndef CHWSURFACETWEAK_H
#define CHWSURFACETWEAK_H

#include <string.h>
#include <math.h>
#include <maya/MPxNode.h>
#include <maya/MArrayDataBuilder.h>
#include <maya/MPxDeformerNode.h>
#include <maya/MMeshIntersector.h>
#include <maya/MFnSingleIndexedComponent.h>
#include <maya/MItGeometry.h>
#include <maya/MItMeshVertex.h>
#include <maya/MItMeshPolygon.h>
#include <maya/MFnNumericAttribute.h>
#include <maya/MFnEnumAttribute.h>
#include <maya/MFnTypedAttribute.h>
#include <maya/MTypeId.h>
#include <maya/MDataBlock.h>
#include <maya/MDataHandle.h>
#include <maya/MDagPath.h>
#include <maya/MGlobal.h>
#include <maya/MArrayDataHandle.h>
#include <maya/MPointArray.h>
#include <maya/MPoint.h>
#include <maya/MVector.h>
#include <maya/MMatrix.h>
#include <maya/MObject.h>
#include <maya/MRampAttribute.h>
#include <maya/MFnUnitAttribute.h>
#include <maya/MFnMesh.h>
#include <maya/MItMeshVertex.h>
#include <maya/MFnMatrixAttribute.h>
#include <maya/MAngle.h>

// THREADING
#include "tbb/tbb.h"
using namespace tbb;

class chwSurfaceTweakData
{
	public:
		unsigned int			m_vert;
		unsigned int			m_upvert;
		unsigned int			m_aimvert;

		unsigned int			m_numverts;

		float					m_env;
		short					m_falloffMode;
		float					m_falloffRadius;
		MRampAttribute			m_falloffCurve;

		MMatrix					m_deformMatrix;
		MMatrix					m_relativeMatrix;
		MMatrix					m_relativeInverseMatrix;
		MMatrix					m_localToWorldMatrix;
		MMatrix					m_localToWorldMatrixInverse;

		MVector					m_relativeMatrixTranslate;

		MPointArray				m_points;
		MFloatVectorArray		m_normals;
		MFloatArray				m_weights;

		MIntArray				m_conids;
};

class chwSurfaceTweak : public MPxDeformerNode
{
	public:
							chwSurfaceTweak();
		virtual				~chwSurfaceTweak();
		static  void*		creator();
		static  MStatus		initialize();

		virtual MStatus		compute( const MPlug& plug, MDataBlock& data );
		virtual	void		postConstructor();

		static  MObject  aInVert;

		static  MObject  aFalloffMode;
		static  MObject  aFalloffCurve;
		static  MObject  aFalloffRadius;
		static  MObject  aRelativeMatrix;
		static  MObject  aDeformMatrix;

		static  MObject  aOutTranslate;
		static  MObject  aOutTranslateX;
		static  MObject  aOutTranslateY;
		static  MObject  aOutTranslateZ;

		static  MObject  aOutRotate;
		static  MObject  aOutRotateX;
		static  MObject  aOutRotateY;
		static  MObject  aOutRotateZ;

		static const MTypeId typeId;
		static const MString typeName;

		private:
			chwSurfaceTweakData	*m_data;

			MStatus		postConstructor_initialise_ramp_curve(MObject &rampObj, int index, float position, float value, int interpolation);

			struct ThreadedDeform
			{
				chwSurfaceTweakData *data;
				void operator()( const blocked_range<int>& range ) const {
					MStatus status;
					MPoint point;
					float dist,ramppos, rampval;
					float w;
					for ( int idx = range.begin(); idx!=range.end(); ++idx )
					{
						w = data->m_weights[idx] * data->m_env;
						if ((w > 0.f) && (data->m_falloffRadius > 0.f))
						{
							dist = (data->m_relativeMatrixTranslate - data->m_points[idx]).length();
							ramppos = dist / data->m_falloffRadius;
							data->m_falloffCurve.getValueAtPosition(ramppos, rampval, &status);

							point = data->m_points[idx] * data->m_relativeInverseMatrix * data->m_deformMatrix;
							data->m_points[idx] += ((point - data->m_points[idx]) * w * rampval);
						}
					}
				}
			};
};

#endif // CHWSURFACETWEAK_H
