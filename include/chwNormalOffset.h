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

#ifndef CHWNORMALOFFSET_H
#define CHWNORMALOFFSET_H

#include <maya/MPxDeformerNode.h>
#include <maya/MItGeometry.h>
#include <maya/MTypeId.h>
#include <maya/MPlug.h>
#include <maya/MDataBlock.h>
#include <maya/MDataHandle.h>
#include <maya/MPoint.h>
#include <maya/MPointArray.h>
#include <maya/MFnMesh.h>
#include <maya/MVector.h>
#include <maya/MFloatVectorArray.h>
#include <maya/MGlobal.h>
#include <maya/MMatrix.h>
#include <maya/MFnNumericAttribute.h>
#include <maya/MFloatArray.h>
#include "chwEdgeRelax.h"

// THREADING
#include "tbb/tbb.h"
using namespace tbb;

class chwNormalOffsetData
{
	public:
		unsigned int		m_numverts;
		unsigned int		m_smooth;
		float				m_env;
		float				m_strength;
		MPointArray			m_points;
		MFloatVectorArray	m_normals;
		MFloatArray			m_weights;
};

class chwNormalOffset : public MPxDeformerNode
{
	public:
								chwNormalOffset();
		virtual					~chwNormalOffset();

		virtual MStatus			compute(const MPlug& plug, MDataBlock& data);
		static  void*			creator();
		static  MStatus			initialize();

	public:
		static  MObject			aStrength;
		static  MObject			aNormalSmooth;

		static const MTypeId	typeId;
		static const MString	typeName;

	private:
			chwEdgeRelax m_relax;
			chwNormalOffsetData *m_data;
			struct ThreadedDeform
			{
				chwNormalOffsetData *data;
				void operator()( const blocked_range<int>& range ) const {
					for ( int idx = range.begin(); idx!=range.end(); ++idx )
					{
						data->m_points[idx] += data->m_normals[idx] * data->m_weights[idx];
					}
				}
			};
};

#endif // CHWNORMALOFFSET_H
