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
