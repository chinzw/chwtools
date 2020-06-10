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

#include <maya/MFnPlugin.h>
#include <chwVertexBind.h>
#include <chwMeshRelax.h>
#include <chwNormalOffset.h>
#include <chwMatrixSelector.h>
#include <chwCurveSampler.h>
#include <chwCollision.h>
#include <chwSurfaceTweak.h>

MStatus initializePlugin( MObject obj )
{
	MFnPlugin plugin( obj, "Cristian Hinz Welkens", "1.0", "Any");

	CHECK_MSTATUS ( plugin.registerNode( chwVertexBind::typeName, chwVertexBind::typeId,
					chwVertexBind::creator, chwVertexBind::initialize, MPxNode::kDeformerNode ) );

	CHECK_MSTATUS ( plugin.registerNode( chwMeshRelax::typeName, chwMeshRelax::typeId,
					chwMeshRelax::creator, chwMeshRelax::initialize, MPxNode::kDeformerNode ) );

	CHECK_MSTATUS ( plugin.registerNode( chwNormalOffset::typeName, chwNormalOffset::typeId,
					chwNormalOffset::creator, chwNormalOffset::initialize, MPxNode::kDeformerNode ) );

	CHECK_MSTATUS ( plugin.registerNode( chwCollision::typeName, chwCollision::typeId,
					chwCollision::creator, chwCollision::initialize, MPxNode::kDeformerNode ) );

	CHECK_MSTATUS ( plugin.registerNode( chwSurfaceTweak::typeName, chwSurfaceTweak::typeId,
					chwSurfaceTweak::creator, chwSurfaceTweak::initialize, MPxNode::kDeformerNode ) );

	CHECK_MSTATUS ( plugin.registerNode( chwMatrixSelector::typeName, chwMatrixSelector::typeId,
					chwMatrixSelector::creator, chwMatrixSelector::initialize, MPxNode::kDependNode ) );

	CHECK_MSTATUS ( plugin.registerNode( chwCurveSampler::typeName, chwCurveSampler::typeId,
					chwCurveSampler::creator, chwCurveSampler::initialize, MPxNode::kDependNode ) );
	return MS::kSuccess;
}

MStatus uninitializePlugin( MObject obj )
{
	MFnPlugin plugin( obj );

	CHECK_MSTATUS ( plugin.deregisterNode( chwVertexBind::typeId ) );

	CHECK_MSTATUS ( plugin.deregisterNode( chwMeshRelax::typeId ) );

	CHECK_MSTATUS ( plugin.deregisterNode( chwNormalOffset::typeId ) );

	CHECK_MSTATUS ( plugin.deregisterNode( chwMatrixSelector::typeId ) );

	CHECK_MSTATUS ( plugin.deregisterNode( chwCurveSampler::typeId ) );

	CHECK_MSTATUS ( plugin.deregisterNode( chwCollision::typeId ) );

	CHECK_MSTATUS ( plugin.deregisterNode( chwSurfaceTweak::typeId ) );

	return MS::kSuccess;
}


// PLUGIN IDS:
// 0x89001 - chwVertexBind
// 0x89002 - chwMeshRelax
// 0x89003 - chwNormalOffset
// 0x89004 - chwMatrixSelector
// 0x89005 - chwCurveSampler
// 0x89006 - chwCollision
// 0x89007 - chwSurfaceTweak
