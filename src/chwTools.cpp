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
