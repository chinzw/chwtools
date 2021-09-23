#include "chwWindField.h"

const MTypeId chwWindField::typeId( 0x00139200, 8 );
const MString chwWindField::typeName( "chwWindField" );

// COMMON
MObject  chwWindField::m_attenuation_mode;
MObject  chwWindField::m_direction;
MObject  chwWindField::m_current_time;

// NOISE
MObject  chwWindField::m_noise_time_mult;
MObject  chwWindField::m_noise_octaves;
MObject  chwWindField::m_noise_persistence;
MObject  chwWindField::m_noise_weight;
MObject  chwWindField::m_noise_frequency;
MObject  chwWindField::m_noise_phase;

// GUST
MObject  chwWindField::m_gust_time_mult;
MObject  chwWindField::m_gust_octaves;
MObject  chwWindField::m_gust_persistence;
MObject  chwWindField::m_gust_weight;
MObject  chwWindField::m_gust_frequency;
MObject  chwWindField::m_gust_phase;
MObject  chwWindField::m_gust_bounds;

// SWIRL
MObject  chwWindField::m_swirl_weight;
MObject  chwWindField::m_swirl_time_mult;
MObject  chwWindField::m_swirl_octaves;
MObject  chwWindField::m_swirl_persistence;
MObject  chwWindField::m_swirl_frequency;
MObject  chwWindField::m_swirl_phase;

// OCCLUSION
MObject  chwWindField::m_occ_self_radius;
MObject  chwWindField::m_occ_mesh_radius;
MObject  chwWindField::m_occ_mesh;

// DISPLAY
MObject  chwWindField::m_display;
MObject  chwWindField::m_display_scale;
MObject  chwWindField::m_display_normalized;

chwWindField::chwWindField()
{
	m_fieldData = new fieldData;
	m_fieldData->m_initialized = false;
}

chwWindField::~chwWindField()
{
	delete m_fieldData;
}

void *chwWindField::creator()
{
	return new chwWindField;
}

MStatus chwWindField::initialize()
{
	MFnNumericAttribute nAttr;
	MFnTypedAttribute tAttr;
	MFnEnumAttribute eAttr;

	// COMMON
	m_attenuation_mode = eAttr.create( "attenuationMode", "attm", 0 );
	CHECK_MSTATUS(eAttr.addField("Linear",0));
	CHECK_MSTATUS(eAttr.addField("Smooth",1));
	CHECK_MSTATUS(eAttr.addField("Squared",2));
	CHECK_MSTATUS(eAttr.addField("Cubed",3));
	CHECK_MSTATUS(eAttr.addField("Inv Squared",4));
	CHECK_MSTATUS(eAttr.addField("Inv Cubed",5));
	CHECK_MSTATUS(eAttr.setWritable(true));
	CHECK_MSTATUS(eAttr.setStorable(true));
	CHECK_MSTATUS(eAttr.setKeyable (true));
	CHECK_MSTATUS(eAttr.setConnectable(true));
	CHECK_MSTATUS(eAttr.setDefault(0));
	addAttribute( m_attenuation_mode );
	
	m_direction = nAttr.createPoint( "direction", "dir");
	CHECK_MSTATUS(nAttr.setDefault( 0.0, 0.0, 0.0 ));
	CHECK_MSTATUS(nAttr.setWritable(true));
	CHECK_MSTATUS(nAttr.setStorable(true));
	CHECK_MSTATUS(nAttr.setKeyable (true));
	CHECK_MSTATUS(nAttr.setConnectable(true));
	addAttribute( m_direction );
	
	m_current_time = nAttr.create( "currentTime", "frame", MFnNumericData::kFloat, 1.f);
	CHECK_MSTATUS(nAttr.setWritable(true));
	CHECK_MSTATUS(nAttr.setStorable(true));
	CHECK_MSTATUS(nAttr.setKeyable (true));
	CHECK_MSTATUS(nAttr.setConnectable(true));
	CHECK_MSTATUS(nAttr.setHidden(false));
	addAttribute( m_current_time );

	// NOISE
	m_noise_weight = nAttr.create( "noiseWeight", "now", MFnNumericData::kFloat, 0.5);
	CHECK_MSTATUS(nAttr.setWritable(true));
	CHECK_MSTATUS(nAttr.setStorable(true));
	CHECK_MSTATUS(nAttr.setKeyable (true));
	CHECK_MSTATUS(nAttr.setConnectable(true));
	addAttribute( m_noise_weight );

	m_noise_time_mult = nAttr.create( "noiseTimeMult", "not", MFnNumericData::kFloat, 0.25);
	CHECK_MSTATUS(nAttr.setWritable(true));
	CHECK_MSTATUS(nAttr.setStorable(true));
	CHECK_MSTATUS(nAttr.setKeyable (true));
	CHECK_MSTATUS(nAttr.setConnectable(true));
	addAttribute( m_noise_time_mult );
	
	m_noise_octaves = nAttr.create( "noiseOctaves", "noo", MFnNumericData::kInt, 8);
	CHECK_MSTATUS(nAttr.setWritable(true));
	CHECK_MSTATUS(nAttr.setStorable(true));
	CHECK_MSTATUS(nAttr.setKeyable (true));
	CHECK_MSTATUS(nAttr.setConnectable(true));
	addAttribute( m_noise_octaves );
	
	m_noise_persistence = nAttr.create( "noisePersistence", "nop", MFnNumericData::kFloat, 0.25);
	CHECK_MSTATUS(nAttr.setWritable(true));
	CHECK_MSTATUS(nAttr.setStorable(true));
	CHECK_MSTATUS(nAttr.setKeyable (true));
	CHECK_MSTATUS(nAttr.setConnectable(true));
	addAttribute( m_noise_persistence );
	
	m_noise_frequency = nAttr.createPoint( "noiseFrequency", "nof");
	CHECK_MSTATUS(nAttr.setDefault( 0.1, 0.1, 0.1 ));
	CHECK_MSTATUS(nAttr.setWritable(true));
	CHECK_MSTATUS(nAttr.setStorable(true));
	CHECK_MSTATUS(nAttr.setKeyable (true));
	CHECK_MSTATUS(nAttr.setConnectable(true));
	addAttribute( m_noise_frequency );
	
	m_noise_phase = nAttr.createPoint( "noisePhase", "noph");
	CHECK_MSTATUS(nAttr.setDefault( 1.0, 1.0, 1.0 ));
	CHECK_MSTATUS(nAttr.setWritable(true));
	CHECK_MSTATUS(nAttr.setStorable(true));
	CHECK_MSTATUS(nAttr.setKeyable (true));
	CHECK_MSTATUS(nAttr.setConnectable(true));
	addAttribute( m_noise_phase );
	
	// GUST
	m_gust_weight = nAttr.create( "gustWeight", "guw", MFnNumericData::kFloat, 0.0);
	CHECK_MSTATUS(nAttr.setWritable(true));
	CHECK_MSTATUS(nAttr.setStorable(true));
	CHECK_MSTATUS(nAttr.setKeyable (true));
	CHECK_MSTATUS(nAttr.setConnectable(true));
	addAttribute( m_gust_weight );

	m_gust_time_mult = nAttr.create( "gustTimeMult", "gut", MFnNumericData::kFloat, 0.25);
	CHECK_MSTATUS(nAttr.setWritable(true));
	CHECK_MSTATUS(nAttr.setStorable(true));
	CHECK_MSTATUS(nAttr.setKeyable (true));
	CHECK_MSTATUS(nAttr.setConnectable(true));
	addAttribute( m_gust_time_mult );

	m_gust_octaves = nAttr.create( "gustOctaves", "guo", MFnNumericData::kInt, 1);
	CHECK_MSTATUS(nAttr.setWritable(true));
	CHECK_MSTATUS(nAttr.setStorable(true));
	CHECK_MSTATUS(nAttr.setKeyable (true));
	CHECK_MSTATUS(nAttr.setConnectable(true));
	addAttribute( m_gust_octaves );

	m_gust_persistence = nAttr.create( "gustPersistence", "gup", MFnNumericData::kFloat, 0.0);
	CHECK_MSTATUS(nAttr.setWritable(true));
	CHECK_MSTATUS(nAttr.setStorable(true));
	CHECK_MSTATUS(nAttr.setKeyable (true));
	CHECK_MSTATUS(nAttr.setConnectable(true));
	addAttribute( m_gust_persistence );

	m_gust_bounds = nAttr.create( "gustBounds", "gub", MFnNumericData::k2Float);
	CHECK_MSTATUS(nAttr.setMin(-1.0f, -1.0f) );
	CHECK_MSTATUS(nAttr.setMax(1.0f, 1.0f) );
	CHECK_MSTATUS(nAttr.setDefault(-1.0f, 1.0f) );
	CHECK_MSTATUS(nAttr.setWritable(true));
	CHECK_MSTATUS(nAttr.setStorable(true));
	CHECK_MSTATUS(nAttr.setKeyable (true));
	CHECK_MSTATUS(nAttr.setConnectable(true));
	addAttribute( m_gust_bounds );

	m_gust_frequency = nAttr.createPoint( "gustFrequency", "guf");
	CHECK_MSTATUS(nAttr.setDefault( 0.1, 0.1, 0.1 ));
	CHECK_MSTATUS(nAttr.setWritable(true));
	CHECK_MSTATUS(nAttr.setStorable(true));
	CHECK_MSTATUS(nAttr.setKeyable (true));
	CHECK_MSTATUS(nAttr.setConnectable(true));
	addAttribute( m_gust_frequency );

	m_gust_phase = nAttr.createPoint( "gustPhase", "guph");
	CHECK_MSTATUS(nAttr.setDefault( 1.0, 1.0, 1.0 ));
	CHECK_MSTATUS(nAttr.setWritable(true));
	CHECK_MSTATUS(nAttr.setStorable(true));
	CHECK_MSTATUS(nAttr.setKeyable (true));
	CHECK_MSTATUS(nAttr.setConnectable(true));
	addAttribute( m_gust_phase );

	// SWIRL
	m_swirl_weight = nAttr.create( "swirlWeight", "sww", MFnNumericData::kFloat, 0.1);
	CHECK_MSTATUS(nAttr.setWritable(true));
	CHECK_MSTATUS(nAttr.setStorable(true));
	CHECK_MSTATUS(nAttr.setKeyable (true));
	CHECK_MSTATUS(nAttr.setConnectable(true));
	addAttribute( m_swirl_weight );

	m_swirl_time_mult = nAttr.create( "swirlTimeMult", "swt", MFnNumericData::kFloat, 0.25);
	CHECK_MSTATUS(nAttr.setWritable(true));
	CHECK_MSTATUS(nAttr.setStorable(true));
	CHECK_MSTATUS(nAttr.setKeyable (true));
	CHECK_MSTATUS(nAttr.setConnectable(true));
	addAttribute( m_swirl_time_mult );

	m_swirl_octaves = nAttr.create( "swirlOctaves", "swo", MFnNumericData::kInt, 1);
	CHECK_MSTATUS(nAttr.setWritable(true));
	CHECK_MSTATUS(nAttr.setStorable(true));
	CHECK_MSTATUS(nAttr.setKeyable (true));
	CHECK_MSTATUS(nAttr.setConnectable(true));
	addAttribute( m_swirl_octaves );

	m_swirl_persistence = nAttr.create( "swirlPersistence", "swp", MFnNumericData::kFloat, 0.0);
	CHECK_MSTATUS(nAttr.setWritable(true));
	CHECK_MSTATUS(nAttr.setStorable(true));
	CHECK_MSTATUS(nAttr.setKeyable (true));
	CHECK_MSTATUS(nAttr.setConnectable(true));
	addAttribute( m_swirl_persistence );

	m_swirl_frequency = nAttr.createPoint( "swirlFrequency", "swf");
	CHECK_MSTATUS(nAttr.setDefault( 0.1, 0.1, 0.1 ));
	CHECK_MSTATUS(nAttr.setWritable(true));
	CHECK_MSTATUS(nAttr.setStorable(true));
	CHECK_MSTATUS(nAttr.setKeyable (true));
	CHECK_MSTATUS(nAttr.setConnectable(true));
	addAttribute( m_swirl_frequency );

	m_swirl_phase = nAttr.createPoint( "swirlPhase", "swph");
	CHECK_MSTATUS(nAttr.setDefault( 1.0, 1.0, 1.0 ));
	CHECK_MSTATUS(nAttr.setWritable(true));
	CHECK_MSTATUS(nAttr.setStorable(true));
	CHECK_MSTATUS(nAttr.setKeyable (true));
	CHECK_MSTATUS(nAttr.setConnectable(true));
	addAttribute( m_swirl_phase );

	// OCCLUSION
	m_occ_self_radius = nAttr.create( "occlusionSelfRadius", "osr", MFnNumericData::kFloat, 0.1);
	CHECK_MSTATUS(nAttr.setWritable(true));
	CHECK_MSTATUS(nAttr.setStorable(true));
	CHECK_MSTATUS(nAttr.setKeyable (true));
	CHECK_MSTATUS(nAttr.setConnectable(true));
	addAttribute( m_occ_self_radius );

	m_occ_mesh_radius = nAttr.create( "occlusionMeshRadius", "omr", MFnNumericData::kFloat, 0.1);
	CHECK_MSTATUS(nAttr.setWritable(true));
	CHECK_MSTATUS(nAttr.setStorable(true));
	CHECK_MSTATUS(nAttr.setKeyable (true));
	CHECK_MSTATUS(nAttr.setConnectable(true));
	addAttribute( m_occ_mesh_radius );

	m_occ_mesh = tAttr.create( "oclusionMesh", "om", MFnData::kMesh );
	CHECK_MSTATUS(tAttr.setStorable(false));
	CHECK_MSTATUS(tAttr.setConnectable(true));
	addAttribute( m_occ_mesh );

	// DISPLAY
	m_display = nAttr.create( "displayField", "disp", MFnNumericData::kBoolean, true);
	CHECK_MSTATUS(nAttr.setWritable(true));
	CHECK_MSTATUS(nAttr.setStorable(true));
	CHECK_MSTATUS(nAttr.setKeyable (true));
	CHECK_MSTATUS(nAttr.setConnectable(true));
	addAttribute( m_display );
	
	m_display_normalized = nAttr.create( "displayNormalized", "dispn", MFnNumericData::kBoolean, true);
	CHECK_MSTATUS(nAttr.setWritable(true));
	CHECK_MSTATUS(nAttr.setStorable(true));
	CHECK_MSTATUS(nAttr.setKeyable (true));
	CHECK_MSTATUS(nAttr.setConnectable(true));
	addAttribute( m_display_normalized );

	m_display_scale = nAttr.create( "displayScale", "disps", MFnNumericData::kFloat, 1.0);
	CHECK_MSTATUS(nAttr.setWritable(true));
	CHECK_MSTATUS(nAttr.setStorable(true));
	CHECK_MSTATUS(nAttr.setKeyable (true));
	CHECK_MSTATUS(nAttr.setConnectable(true));
	addAttribute( m_display_scale );

	attributeAffects(chwWindField::mMagnitude,	 			chwWindField::mOutputForce);
	attributeAffects(chwWindField::mAttenuation,			chwWindField::mOutputForce);
	attributeAffects(chwWindField::mMaxDistance,			chwWindField::mOutputForce);
	attributeAffects(chwWindField::mUseMaxDistance,			chwWindField::mOutputForce);
	attributeAffects(chwWindField::m_attenuation_mode,	 	chwWindField::mOutputForce);
	attributeAffects(chwWindField::m_direction,				chwWindField::mOutputForce);
	attributeAffects(chwWindField::m_current_time,			chwWindField::mOutputForce);
	attributeAffects(chwWindField::m_noise_time_mult, 		chwWindField::mOutputForce);
	attributeAffects(chwWindField::m_noise_octaves,			chwWindField::mOutputForce);
	attributeAffects(chwWindField::m_noise_persistence,		chwWindField::mOutputForce);
	attributeAffects(chwWindField::m_noise_weight,			chwWindField::mOutputForce);
	attributeAffects(chwWindField::m_noise_frequency,		chwWindField::mOutputForce);
	attributeAffects(chwWindField::m_noise_phase,			chwWindField::mOutputForce);
	attributeAffects(chwWindField::m_gust_time_mult,		chwWindField::mOutputForce);
	attributeAffects(chwWindField::m_gust_octaves,			chwWindField::mOutputForce);
	attributeAffects(chwWindField::m_gust_persistence,		chwWindField::mOutputForce);
	attributeAffects(chwWindField::m_gust_weight,			chwWindField::mOutputForce);
	attributeAffects(chwWindField::m_gust_frequency,		chwWindField::mOutputForce);
	attributeAffects(chwWindField::m_gust_phase,			chwWindField::mOutputForce);
	attributeAffects(chwWindField::m_gust_bounds,			chwWindField::mOutputForce);
	attributeAffects(chwWindField::m_swirl_weight,			chwWindField::mOutputForce);
	attributeAffects(chwWindField::m_swirl_time_mult,		chwWindField::mOutputForce);
	attributeAffects(chwWindField::m_swirl_octaves,			chwWindField::mOutputForce);
	attributeAffects(chwWindField::m_swirl_persistence,		chwWindField::mOutputForce);
	attributeAffects(chwWindField::m_swirl_frequency,		chwWindField::mOutputForce);
	attributeAffects(chwWindField::m_swirl_phase,			chwWindField::mOutputForce);
	attributeAffects(chwWindField::m_occ_self_radius,		chwWindField::mOutputForce);
	attributeAffects(chwWindField::m_occ_mesh_radius,		chwWindField::mOutputForce);
	attributeAffects(chwWindField::m_occ_mesh,				chwWindField::mOutputForce);
	attributeAffects(chwWindField::m_display,				chwWindField::mOutputForce);
	attributeAffects(chwWindField::m_display_scale,			chwWindField::mOutputForce);
	attributeAffects(chwWindField::m_display_normalized,	chwWindField::mOutputForce);

return( MS::kSuccess );
}


MStatus chwWindField::compute(const MPlug& plug, MDataBlock& block)
{
	MStatus status;
	
	// COMMON
	m_fieldData->m_magnitude = block.inputValue( mMagnitude , &status ).asDouble();
	m_fieldData->m_direction = block.inputValue( m_direction , &status ).asFloatVector();
	m_fieldData->m_current_time = block.inputValue( m_current_time , &status ).asFloat();
	m_fieldData->m_mdirection = m_fieldData->m_direction*(1-m_fieldData->m_noise_weight);
	m_fieldData->m_idirection = -1 * m_fieldData->m_direction.normal();

	m_fieldData->m_position = getWorldPosition( block );

	// NOISE
	m_fieldData->m_noise_weight = block.inputValue( m_noise_weight , &status ).asFloat();
	m_fieldData->m_noise_time_mult = block.inputValue( m_noise_time_mult , &status ).asFloat();
	m_fieldData->m_noise_octaves = block.inputValue( m_noise_octaves , &status ).asInt();
	m_fieldData->m_noise_persistence = block.inputValue( m_noise_persistence , &status ).asFloat();
	m_fieldData->m_noise_frequency = block.inputValue( m_noise_frequency , &status ).asFloatVector();
	m_fieldData->m_noise_phase = block.inputValue( m_noise_phase , &status ).asFloatVector();

	// GUST
	m_fieldData->m_gust_weight = block.inputValue(m_gust_weight, &status).asFloat();
	m_fieldData->m_gust_time_mult = block.inputValue(m_gust_time_mult, &status).asFloat();
	m_fieldData->m_gust_octaves = block.inputValue( m_gust_octaves , &status ).asInt();
	m_fieldData->m_gust_persistence = block.inputValue(m_gust_persistence, &status).asFloat();
	float2 &bounds = block.inputValue( m_gust_bounds , &status ).asFloat2();
	m_fieldData->m_gust_bounds[0] = bounds[0];
	m_fieldData->m_gust_bounds[1] = bounds[1];
	m_fieldData->m_gust_frequency = block.inputValue( m_gust_frequency , &status ).asFloatVector();
	m_fieldData->m_gust_phase = block.inputValue( m_gust_phase , &status ).asFloatVector();

	// SWIRL
	m_fieldData->m_swirl_weight = block.inputValue( m_swirl_weight , &status ).asFloat();
	m_fieldData->m_swirl_time_mult = block.inputValue( m_swirl_time_mult , &status ).asFloat();
	m_fieldData->m_swirl_octaves = block.inputValue( m_swirl_octaves , &status ).asInt();
	m_fieldData->m_swirl_persistence = block.inputValue( m_swirl_persistence , &status ).asFloat();
	m_fieldData->m_swirl_frequency = block.inputValue( m_swirl_frequency , &status ).asFloatVector();
	m_fieldData->m_swirl_phase = block.inputValue( m_swirl_phase , &status ).asFloatVector();

	// OCCLUSION
	m_fieldData->m_occ_self_radius = block.inputValue( m_occ_self_radius , &status ).asFloat();
	m_fieldData->m_occ_self_radius *= m_fieldData->m_occ_self_radius;

	m_fieldData->m_occ_mesh_radius = block.inputValue( m_occ_mesh_radius , &status ).asFloat();
	m_fieldData->m_occ_mesh_radius *= m_fieldData->m_occ_mesh_radius;
	MObject meshMobj = block.inputValue(m_occ_mesh, &status).asMesh();
	MFnMesh inputMesh(meshMobj);
	inputMesh.getPoints(m_fieldData->m_occ_mesh_points, MSpace::kWorld);

	//DISPLAY
	m_fieldData->m_display = block.inputValue( m_display , &status ).asBool();
	m_fieldData->m_display_normalized = block.inputValue( m_display_normalized , &status ).asBool();
	m_fieldData->m_display_scale = block.inputValue( m_display_scale , &status ).asFloat();

	//ATTENUATION
	m_fieldData->m_attenuation = block.inputValue(mAttenuation, &status).asDouble();
	m_fieldData->m_attenuation_mode = block.inputValue(m_attenuation_mode, &status).asShort();
	m_fieldData->m_use_max_distance = block.inputValue(mUseMaxDistance, &status).asBool();
	m_fieldData->m_max_distance = block.inputValue(mMaxDistance, &status).asDouble();

	m_fieldData->m_max_mag = 0.f;
	m_fieldData->m_initialized = true;

	if( !(plug == mOutputForce) )
		return( MS::kUnknownParameter );

	int multiIndex = plug.logicalIndex( &status );
	CHECK_MSTATUS(status);
	
	MArrayDataHandle hInputArray = block.outputArrayValue( mInputData, &status );
	CHECK_MSTATUS(status);
	
	status = hInputArray.jumpToElement( multiIndex );
	CHECK_MSTATUS(status);
	
	MDataHandle hCompond = hInputArray.inputValue( &status );
	CHECK_MSTATUS(status);
	
	MDataHandle hPosition = hCompond.child( mInputPositions );
	MObject dPosition = hPosition.data();
	MFnVectorArrayData fnPosition( dPosition );
	m_fieldData->m_points = fnPosition.array( &status );
	CHECK_MSTATUS(status);
	
	MDataHandle hVelocity = hCompond.child( mInputVelocities );
	MObject dVelocity = hVelocity.data();
	MFnVectorArrayData fnVelocity( dVelocity );
	m_fieldData->m_velocities = fnVelocity.array( &status );
	CHECK_MSTATUS(status);

	// TBB parallel_for
	ThreadedForce tf;
	tf.data = m_fieldData;
	parallel_for( blocked_range<int>( 0, m_fieldData->m_points.length() ), tf );

	// get output data handle
	//
	MArrayDataHandle hOutArray = block.outputArrayValue( mOutputForce, &status);
	CHECK_MSTATUS(status);
	MArrayDataBuilder bOutArray = hOutArray.builder( &status );
	CHECK_MSTATUS(status);
	
	// get output force array from block.
	//
	MDataHandle hOut = bOutArray.addElement(multiIndex, &status);
	CHECK_MSTATUS(status);
	
	MFnVectorArrayData fnOutputForce;
	MObject dOutputForce = fnOutputForce.create( m_fieldData->m_velocities, &status );
	CHECK_MSTATUS(status);
	
	// update data block with new output force data.
	//
	hOut.set( dOutputForce );
	block.setClean( plug );

	return( MS::kSuccess );
}

void chwWindField::draw( M3dView& view, const MDagPath& path, M3dView::DisplayStyle style, M3dView:: DisplayStatus )
{
	MObject thisNode = this->thisMObject();
	MPlug outputForcePlug(thisNode,mOutputForce);
	unsigned int numElements = outputForcePlug.numElements();
	for (unsigned int idx=0; idx < numElements; idx++)
	{	
		outputForcePlug.selectAncestorLogicalIndex(idx);
		MDataHandle outputForceHnd = outputForcePlug.asMDataHandle();
		outputForceHnd.setClean();
	}

	if (m_fieldData->m_display)
	{
		if (m_fieldData->m_initialized)
		{
			MVector force(0.f, 0.f, 0.f);
			MVector mult(0.f, 0.f, 0.f);
			_OPENMAYA_DEPRECATION_PUSH_AND_DISABLE_WARNING
			view.beginGL();
			_OPENMAYA_POP_WARNING
			glPushMatrix();
			for (unsigned int i=0;i<m_fieldData->m_points.length();i++)
			{
				float magovermax = m_fieldData->m_velocities[i].length() / m_fieldData->m_max_mag;
				if (m_fieldData->m_display_normalized)
					mult = m_fieldData->m_points[i] + (m_fieldData->m_velocities[i].normal() * m_fieldData->m_display_scale);
				else
					mult = m_fieldData->m_points[i] + (m_fieldData->m_velocities[i] * m_fieldData->m_display_scale);

				glColor3f(magovermax, 1.f-magovermax, 0);
				glBegin(GL_LINE_STRIP);
				glVertex3f( m_fieldData->m_points[i].x, m_fieldData->m_points[i].y, m_fieldData->m_points[i].z );
				glVertex3f( mult.x, mult.y, mult.z );
				glEnd();
			}
			glPopMatrix();
			_OPENMAYA_DEPRECATION_PUSH_AND_DISABLE_WARNING
			view.endGL();
			_OPENMAYA_POP_WARNING
		}
	}
}

MVector chwWindField::simplexNoise(const int octaves, const float persistence, const float time, const float timemult, MVector freq, MVector phase, MVector sample)
{
	MVector total(0,0,0);
	MVector nfreq = freq;
	float amp = 1;

	// We have to keep track of the largest possible amplitude,
	// because each octave adds more, and we need a value in [-1, 1].
	float max = 0;
	float timed = time*timemult;
	
	OpenSimplexNoise::Noise noise;

	for( int i=0; i < octaves; i++ ) {
		total[0] += noise.eval( (sample[0]+phase[0]+1) * nfreq[0], (sample[1]+phase[1]) * nfreq[1], (sample[2]+phase[2]) * nfreq[2], timed ) * amp;
		total[1] += noise.eval( (sample[0]+phase[0]) * nfreq[0], (sample[1]+phase[1]+1) * nfreq[1], (sample[2]+phase[2]) * nfreq[2], timed ) * amp;
		total[2] += noise.eval( (sample[0]+phase[0]) * nfreq[0], (sample[1]+phase[1]) * nfreq[1], (sample[2]+phase[2]+1) * nfreq[2], timed ) * amp;

		nfreq[0] *= 2;
		nfreq[1] *= 2;
		nfreq[2] *= 2;

		max += amp;
		amp *= persistence;
	}

	return MVector(total[0]/max,total[1]/max,total[2]/max);
}

float chwWindField::simplexNoiseFloat(const int octaves, const float persistence, const float time, const float timemult, MVector freq, MVector phase, MVector sample, float loBound, float hiBound)
{
	float total = 0.f;
	MVector nfreq = freq;
	float amp = 1;
	float result = 0.f;

	// We have to keep track of the largest possible amplitude,
	// because each octave adds more, and we need a value in [-1, 1].
	float max = 0;
	float timed = time*timemult;

	OpenSimplexNoise::Noise noise;

	for( int i=0; i < octaves; i++ ) {
		total += noise.eval( (sample[0]+phase[0]+1) * nfreq[0], (sample[1]+phase[1]) * nfreq[1], (sample[2]+phase[2]) * nfreq[2], timed ) * amp;

		nfreq[0] *= 2;
		nfreq[1] *= 2;
		nfreq[2] *= 2;

		max += amp;
		amp *= persistence;
	}
	result = (total/max) * (hiBound - loBound) / 2 + (hiBound + loBound) / 2;
	result = clamp(result, 0.f, 1.0f);
	return result;
}

IntersectionResult chwWindField::intersect(MVector &origin, MVector &dir, MPoint &center, float &radius2)
{
	MVector vcenter = center;
	return chwWindField::intersect(origin, dir, vcenter, radius2);
}

IntersectionResult chwWindField::intersect(MVector &origin, MVector &dir, MVector &center, float &radius2)
{
	MVector dst = center - origin;
	float B = dst * dir;
	float C = B*B - (dst * dst) + radius2;
	if(C < 0) return IntersectionResult();
	float D = sqrtf(C), E = B+D;
	if(E < 0) return IntersectionResult();
	float F = B - D;
	float dist = (F > 0) ? F : E;
	dist -= 0.001f;
	MVector p = origin + (dir * dist);
	return IntersectionResult(dist, p);
}

MVector chwWindField::getWorldPosition( MDataBlock &block )
{
	MVector pos(0,0,0);
	MStatus status;
	MObject thisNode = thisMObject();
	MFnDependencyNode fnThisNode( thisNode );

	MObject translateAttr = fnThisNode.attribute( "translate" );
	MPlug translatePlug( thisNode, translateAttr );
	pos = block.inputValue( translatePlug, &status ).asVector();
	CHECK_MSTATUS(status);
	return pos;
}
