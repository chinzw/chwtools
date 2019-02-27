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

#ifndef CHWVERTEXBIND_H
#define CHWVERTEXBIND_H

#include <maya/MStatus.h>
#include <maya/MObject.h>
#include <maya/MPoint.h>
#include <maya/MTypeId.h>
#include <maya/MString.h>
#include <maya/MPointArray.h>
#include <maya/MDataBlock.h>
#include <maya/MItGeometry.h>
#include <maya/MFnTypedAttribute.h>
#include <maya/MFnEnumAttribute.h>
#include <maya/MFnNumericAttribute.h>
#include <maya/MPxDeformerNode.h>
#include <maya/MGlobal.h>
#include <maya/MItMeshVertex.h>
#include <maya/MMatrix.h>
#include <maya/MArrayDataBuilder.h>

#include "float.h"

// THREADING
#include "tbb/tbb.h"
using namespace tbb;

class chwVertexBindData
{
	public:
		MPointArray		m_drvpoints;
		MPointArray		m_defpoints;
		MFloatArray		m_weights;
		MIntArray		m_mapping;
		MMatrix			m_localToWorldMatrix;
		float			m_cutoffDistance;
		float			m_searchDistance;
};

class chwVertexBind : public MPxDeformerNode
{
	public:
								chwVertexBind();
		virtual					~chwVertexBind();
		static  void*			creator();
		static  MStatus			initialize();

		virtual MStatus			deform(MDataBlock& data,
									MItGeometry& iter,
									const MMatrix& mat,
									unsigned int mIndex);

		virtual MStatus			initializeMapping( MDataBlock& data, MItGeometry& iter, const MMatrix &localToWorldMatrix);

		static int				getClosestPoint( MPoint &pt, MPointArray &points ) ;

		virtual MObject&		accessoryAttribute() const;

		// Attributes
		static  MObject 		aDriverMesh;
		static  MObject 		aCutoffDistance;
		static  MObject 		aSearchDistance;
		static  MObject 		aInitialize;
		static  MObject 		aVertexMap;

		// PLUGIN
		static  MTypeId 		typeId;
		static const MString 	typeName;
	private:
		chwVertexBindData *m_data;

		struct ThreadedInitialize
		{
			chwVertexBindData *data;
			void operator()( const blocked_range<int>& range ) const {
				float maxdistance;
				float curdistance;
				for ( int idx = range.begin(); idx!=range.end(); ++idx )
				{
					maxdistance = FLT_MAX;
					curdistance = FLT_MIN;

					data->m_mapping[idx] = -1;
					MPoint defpoint = data->m_defpoints[idx] * data->m_localToWorldMatrix;

					for ( int idy = 0; idy < data->m_drvpoints.length(); idy++ )
					{
						curdistance = defpoint.distanceTo( data->m_drvpoints[idy] );
						if (curdistance <= data->m_cutoffDistance)
						{
							data->m_mapping[idx] = idy;
							break;
						}
						if (curdistance < maxdistance)
						{
							maxdistance = curdistance;
							data->m_mapping[idx] = idy;
						}
						if(maxdistance > data->m_searchDistance)
						{
							data->m_mapping[idx] = -1;
						}
					}
				}
			}
		};
		struct ThreadedDeform
		{
			chwVertexBindData *data;
			void operator()( const blocked_range<int>& range ) const {
				for ( int idx = range.begin(); idx!=range.end(); ++idx )
				{
					if (data->m_weights[idx] != 0.f)
					{
						if (data->m_mapping[idx] >= 0)
						{
							data->m_defpoints[idx] *= data->m_localToWorldMatrix;
							data->m_defpoints[idx] += ((data->m_drvpoints[data->m_mapping[idx]] - data->m_defpoints[idx]) * data->m_weights[idx]);
							data->m_defpoints[idx] *= data->m_localToWorldMatrix.inverse();
						}
					}
				}
			}
		};

};

#endif // CHWVERTEXBIND_H
