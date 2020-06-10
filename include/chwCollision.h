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

#ifndef CHWCOLLISION_H
#define CHWCOLLISION_H

#include <string>
#include <iostream>
#include <sstream>

#include <maya/MPxDeformerNode.h>
#include <maya/MItGeometry.h>
#include <maya/MTypeId.h>
#include <maya/MPlug.h>
#include <maya/MDataBlock.h>
#include <maya/MDataHandle.h>
#include <maya/MPoint.h>
#include <maya/MPointArray.h>
#include <maya/MFloatArray.h>
#include <maya/MFnMesh.h>
#include <maya/MVector.h>
#include <maya/MFloatVectorArray.h>
#include <maya/MFloatPointArray.h>
#include <maya/MGlobal.h>
#include <maya/MMatrix.h>
#include <maya/MFnNumericAttribute.h>
#include <maya/MFnTypedAttribute.h>
#include <maya/MFnCompoundAttribute.h>
#include <maya/MMeshIntersector.h>
#include <maya/MBoundingBox.h>
#include <maya/MDagPath.h>
#include <maya/MRampAttribute.h>
#include <maya/MFnStringData.h>

// THREADING
#include "tbb/tbb.h"
using namespace tbb;

typedef mutex mMutex;
static mMutex m_mutex;

class chwCollisionData
{
	public:
		MFloatPointArray			m_points;
		MFloatVectorArray			m_normals;
		MFloatArray					m_weights;
		MMeshIntersector			m_intersector;
		MMeshIsectAccelParams		m_mmAccelParams;
		MIntArray					m_collided;
		MRampAttribute				m_bulgeRampAttribute;

		unsigned int				m_numpoints;

		// USER SETTINGS
		float						m_collisionStrength;
		float						m_bulgeStrength;
		float						m_bulgeDistance;

		MBoundingBox				m_boxin;
		MBoundingBox				m_boxcol;

		float						m_defmax;
};

class chwCollision : public MPxDeformerNode
{
public:
					chwCollision();
		virtual		~chwCollision();

		virtual MStatus		compute(const MPlug& plug, MDataBlock& data);
		static  void*		creator();
		static  MStatus		initialize();

		virtual	void		postConstructor();

public:
		static  MObject		aCollisionStrength;
		static  MObject		aCollisionMesh;
		static  MObject		aCollisionMeshName;
		static  MObject		aCollisionMeshList;
		static  MObject		aCollisionBulgeStrength;
		static  MObject		aCollisionBulgeDistance;
		static  MObject		aCollisionBulgeRamp;

		static const MTypeId typeId;
		static const MString typeName;
private:
		chwCollisionData *m_data;
		MStatus postConstructor_initialise_ramp_curve(MObject &rampObj, int index, float position, float value, int interpolation);

		struct ThreadedCollide
		{
			chwCollisionData *data;
			MFnMesh *colFn;
			void operator()( const blocked_range<int>& range ) const {
				MFloatPointArray			m_hitPoints;
				MFloatArray					m_faHitRayParams;
				MIntArray					m_iaHitFaces;
				MIntArray					m_iaHitTriangles;
				MFloatArray					m_faHitBary1;
				MFloatArray					m_faHitBary2;
				MStatus						m_status;
				MPointOnMesh				m_pointOnMesh;
				for ( int idx = range.begin(); idx!=range.end(); ++idx )
				{
					data->m_collided[idx] = 0;
					if (data->m_boxcol.contains(data->m_points[idx]))
					{
						bool bAllIntersections = colFn->allIntersections(data->m_points[idx], data->m_normals[idx], NULL, NULL, false, MSpace::kWorld, 9999.f, false,
																				&data->m_mmAccelParams, false, m_hitPoints, &m_faHitRayParams, &m_iaHitFaces,
																				&m_iaHitTriangles, &m_faHitBary1, &m_faHitBary2, 0.000001f, &m_status);
						CHECK_MSTATUS(m_status);
						if (bAllIntersections)
						{
							unsigned int numHits = m_hitPoints.length();
							if ((numHits % 2) != 0)
							{
								data->m_intersector.getClosestPoint(data->m_points[idx], m_pointOnMesh);
								MFloatPoint colpoint = m_pointOnMesh.getPoint();
								MFloatVector offset = colpoint - data->m_points[idx];
								data->m_points[idx] += offset * data->m_weights[idx] * data->m_collisionStrength;
								float l = offset.length();
								tbb::mutex::scoped_lock lock;
								lock.acquire(m_mutex);
								if (l > data->m_defmax)
								{
									data->m_defmax = l;
								}
								lock.release();
								data->m_collided[idx] = 1;
							}
						}
					}
				}
			}
		};

		struct ThreadedBulge
		{
			chwCollisionData *data;
			void operator()( const blocked_range<int>& range ) const {
				MStatus						m_status;
				MPointOnMesh				m_pointOnMesh;
				for ( int idx = range.begin(); idx!=range.end(); ++idx )
				{
					if (!data->m_collided[idx])
					{
						float rampval;
						data->m_intersector.getClosestPoint(data->m_points[idx], m_pointOnMesh);
						MFloatPoint colpoint = m_pointOnMesh.getPoint();
						MFloatVector offset = colpoint - data->m_points[idx];
						float ramppos = offset.length() / data->m_bulgeDistance;
						data->m_bulgeRampAttribute.getValueAtPosition(ramppos, rampval, &m_status);
						CHECK_MSTATUS(m_status);
						float bulgestr = rampval * data->m_defmax * data->m_bulgeStrength;
						data->m_points[idx] += data->m_normals[idx] * data->m_weights[idx] * data->m_collisionStrength * bulgestr;
					}
				}
			}
		};
};

#endif // CHWCOLLISION_H
