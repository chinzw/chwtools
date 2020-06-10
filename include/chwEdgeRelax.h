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

#ifndef CHWEDGERELAX_H
#define CHWEDGERELAX_H

#include <string.h>
#include <math.h>
#include <float.h>
#include <vector>
#include <iostream>
#include <algorithm>

#include <maya/MFnMesh.h>
#include <maya/MVector.h>
#include <maya/MIntArray.h>
#include <maya/MFloatArray.h>
#include <maya/MPointArray.h>
#include <maya/MItMeshVertex.h>

// THREADING
#include "tbb/tbb.h"
using namespace tbb;

class chwEdgeRelaxData
{
	public:
		unsigned int					m_vertcount;
		unsigned int					m_cachedcount;
		unsigned int					m_iterations;
		MPointArray						m_points;
		MPointArray						m_relaxpoints;
		std::vector<std::vector<int>>	m_conids;
		MFloatArray						m_weights;
};

class chwEdgeRelax
{
	public:
				chwEdgeRelax();
		virtual	~chwEdgeRelax();
		void relax(MObject&, unsigned int);
		void relax(MObject&, unsigned int, MFloatArray &weights);
		void relax(MObject&, unsigned int, MPointArray&);
		void relax(MObject&, unsigned int, MPointArray&, MFloatArray &weights);
	private:
		chwEdgeRelaxData *m_relaxData;
		void	buildTopology(MObject &mesh);
		void	doRelax();
		struct ThreadedDeform
		{
			chwEdgeRelaxData *data;
			void operator()( const blocked_range<int>& range ) const {
				unsigned int idz;
				unsigned int numcons;
				for ( int idx = range.begin(); idx!=range.end(); ++idx )
				{
					numcons = data->m_conids[idx].size();
					data->m_relaxpoints[idx] = MPoint();
					for (unsigned int idy=0; idy < numcons; idy++)
					{
						idz = data->m_conids[idx][idy];
						data->m_relaxpoints[idx] += data->m_points[idz];
					}
					data->m_relaxpoints[idx] = data->m_points[idx] + ((data->m_relaxpoints[idx] / float(numcons)) - data->m_points[idx]) * data->m_weights[idx];
				}
			}
		};
};

#endif // CHWEDGERELAX_H
