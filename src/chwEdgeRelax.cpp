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

#include "chwEdgeRelax.h"

chwEdgeRelax::chwEdgeRelax()
{
	m_relaxData = new chwEdgeRelaxData;
	m_relaxData->m_cachedcount = 0;
}
chwEdgeRelax::~chwEdgeRelax() { }

void chwEdgeRelax::relax(MObject& mesh, unsigned int iterations)
{
	m_relaxData->m_iterations = iterations;

	buildTopology(mesh);

	doRelax();

	MFnMesh meshfn(mesh);
	meshfn.setPoints(m_relaxData->m_relaxpoints);
}

void chwEdgeRelax::relax(MObject& mesh, unsigned int iterations, MFloatArray &weights)
{
	m_relaxData->m_iterations = iterations;

	buildTopology(mesh);

	m_relaxData->m_weights = weights;

	doRelax();

	MFnMesh meshfn(mesh);
	meshfn.setPoints(m_relaxData->m_relaxpoints);
}

void chwEdgeRelax::relax(MObject& mesh, unsigned int iterations, MPointArray &output)
{
	m_relaxData->m_iterations = iterations;

	buildTopology(mesh);

	doRelax();

	output.setLength(m_relaxData->m_vertcount);
	output = m_relaxData->m_relaxpoints;
}

void chwEdgeRelax::relax(MObject& mesh, unsigned int iterations, MPointArray &output, MFloatArray &weights)
{
	m_relaxData->m_iterations = iterations;

	buildTopology(mesh);

	m_relaxData->m_weights = weights;

	doRelax();

	output.setLength(m_relaxData->m_vertcount);
	output = m_relaxData->m_relaxpoints;
}

void chwEdgeRelax::doRelax()
{
	for (unsigned int idx=0; idx < m_relaxData->m_iterations; ++idx)
	{
		ThreadedDeform td;
		td.data = m_relaxData;
		parallel_for( blocked_range<int>( 0, m_relaxData->m_vertcount ), td );
		m_relaxData->m_points = m_relaxData->m_relaxpoints;
	}
}

void chwEdgeRelax::buildTopology(MObject &mesh)
{
	MItMeshVertex 	iter(mesh);
	MFnMesh			meshfn(mesh);
	MIntArray 		conVerts;

	meshfn.getPoints(m_relaxData->m_points, MSpace::kObject);

	m_relaxData->m_vertcount = iter.count();
	m_relaxData->m_points.clear();
	m_relaxData->m_relaxpoints.clear();
	m_relaxData->m_weights.clear();
	m_relaxData->m_points.setLength(m_relaxData->m_vertcount);
	m_relaxData->m_relaxpoints.setLength(m_relaxData->m_vertcount);
	m_relaxData->m_weights.setLength(m_relaxData->m_vertcount);

	m_relaxData->m_weights = MFloatArray(m_relaxData->m_vertcount, 1.f);

	if (m_relaxData->m_vertcount  != m_relaxData->m_cachedcount)
	{
		m_relaxData->m_conids.clear();
		m_relaxData->m_conids.resize(m_relaxData->m_vertcount);
		for (unsigned int idx=0; !iter.isDone(); iter.next(),++idx)
		{
				iter.getConnectedVertices(conVerts);
				for (unsigned int idz=0;idz<conVerts.length();++idz)
				{
					m_relaxData->m_conids[idx].push_back(conVerts[idz]);
				}
		}
		iter.reset();
	}
	m_relaxData->m_cachedcount = m_relaxData->m_vertcount;
}
