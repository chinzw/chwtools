#ifndef _chwWindField
#define _chwWindField

#include <maya/MPxFieldNode.h>
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
#include <maya/MIOStream.h>
#include <maya/MPxFieldNode.h>
#include <maya/MFnTypedAttribute.h>
#include <maya/MFnVectorArrayData.h>
#include <maya/MFnPointArrayData.h>
#include <maya/MFnEnumAttribute.h>
#include <maya/MArrayDataBuilder.h>

#include "OpenSimplexNoise.h"
#include "tbb/tbb.h"
 
using namespace tbb;

inline float clamp(float x, float a, float b) { return x < a ? a : (x > b ? b : x); }
inline float linear(float x, float a, float b)
{
	x = clamp((x - a)/(b - a), a, b);
	return x;
}
inline float smooth(float x, float a, float b)
{
	x = clamp((x - a)/(b - a), a, b);
	return x*x*x*(x * (x * 6 - 15) + 10);
}
inline float squared(float x, float a, float b)
{
	x = clamp((x - a)/(b - a), a, b);
	return x*x;
}
inline float invsquared(float x, float a, float b)
{
	x = 1- clamp((x - a)/(b - a), a, b);
	return 1 - x*x;
}
inline float cubed(float x, float a, float b)
{
	x = clamp((x - a)/(b - a), a, b);
	return x*x*x;
}
inline float invcubed(float x, float a, float b)
{
	x = 1- clamp((x - a)/(b - a), a, b);
	return 1 - x*x*x;
}

class IntersectionResult
{
	public:
		bool hit;
		float dist;
		MPoint point;

		IntersectionResult():hit(false), dist(0), point(MPoint(0,0,0)) {}
		IntersectionResult(float dist, MPoint &p):dist(dist), point(p), hit(true) {}
		IntersectionResult(float dist, MVector &p):dist(dist), point(p), hit(true) {}
};

class fieldData
{
	public:
		// COMMON
		float			m_magnitude;
		float			m_current_time;
		float			m_attenuation;
		int				m_attenuation_mode;

		// NOISE
		int				m_noise_octaves;
		float			m_noise_persistence;
		float			m_noise_time_mult;
		float			m_noise_weight;
		MVector			m_noise_frequency;
		MVector			m_noise_phase;

		// GUST
		int				m_gust_octaves;
		float			m_gust_persistence;
		float			m_gust_weight;
		float			m_gust_time_mult;
		float2			m_gust_bounds;
		MVector			m_gust_frequency;
		MVector			m_gust_phase;

		// SWIRL
		int				m_swirl_octaves;
		float			m_swirl_persistence;
		float			m_swirl_weight;
		float			m_swirl_time_mult;
		MVector			m_swirl_frequency;
		MVector			m_swirl_phase;

		// OCCLUSION
		float			m_occ_self_radius;
		float			m_occ_mesh_radius;
		MPointArray 	m_occ_mesh_points;

		// DISPLAY
		bool 			m_display;
		bool 			m_display_normalized;
		float 			m_display_scale;

		// STORAGE
		MVectorArray 	m_velocities;
		MVectorArray 	m_points;
		MVector			m_direction;
		MVector			m_idirection;
		MVector			m_mdirection;
		MVector			m_position;
		bool			m_initialized;

		// ATTENUATION
		bool			m_use_max_distance;
		float			m_max_distance;

		float			m_max_mag;
};

class chwWindField : public MPxFieldNode
{
	public:
				chwWindField();
		virtual ~chwWindField();
		
		static void    	*creator();
		static MStatus  initialize();
		virtual MStatus compute( const MPlug& plug, MDataBlock& block );
		virtual void    draw (  M3dView  & view, const  MDagPath  & path,  M3dView::DisplayStyle  style, M3dView:: DisplayStatus );
		
		static const MTypeId  typeId;
		static const MString  typeName;
		
		// COMMON
		static MObject  m_attenuation_mode;
		static MObject  m_direction;
		static MObject  m_current_time;

		// NOISE
		static MObject  m_noise_time_mult;
		static MObject  m_noise_octaves;
		static MObject  m_noise_persistence;
		static MObject  m_noise_weight;
		static MObject  m_noise_frequency;
		static MObject  m_noise_phase;

		// GUST
		static MObject  m_gust_time_mult;
		static MObject  m_gust_octaves;
		static MObject  m_gust_persistence;
		static MObject  m_gust_weight;
		static MObject  m_gust_frequency;
		static MObject  m_gust_phase;
		static MObject  m_gust_bounds;

		// SWIRL
		static MObject  m_swirl_weight;
		static MObject  m_swirl_time_mult;
		static MObject  m_swirl_octaves;
		static MObject  m_swirl_persistence;
		static MObject  m_swirl_frequency;
		static MObject  m_swirl_phase;

		// OCCLUSION
		static MObject  m_occ_self_radius;
		static MObject  m_occ_mesh_radius;
		static MObject  m_occ_mesh;

		// DISPLAY
		static MObject  m_display;
		static MObject  m_display_scale;
		static MObject  m_display_normalized;

	private:
		fieldData* m_fieldData;
		static MVector simplexNoise(const int octaves, const float persistence, const float time, const float timemult, const MVector freq, const MVector phase, const MVector sample);
		static float simplexNoiseFloat(const int octaves, const float persistence, const float time, const float timemult, MVector freq, MVector phase, MVector sample, float loBound, float hiBound);


		template<typename T, typename C, typename D>
		static IntersectionResult intersect(const T &origin, const C &dir, const D &center, const float radius2)
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
		
		MVector getWorldPosition(MDataBlock &block);

		struct ThreadedForce
		{
			fieldData *data;
			void operator()( const blocked_range<int>& range ) const {
				double rf = static_cast <double> (rand()) / static_cast <double> (RAND_MAX);
				MVector rv(rf, rf, rf);
				MVector noise(0,0,0);
				float oneoverdist = 0.f;
				float step = 0.f;
				float mag = data->m_magnitude;

				rv.normalize();
				for( int i=range.begin(); i!=range.end(); ++i )
				{
					IntersectionResult result;
					if (data->m_occ_mesh_radius > 0.f)
					{
						for (unsigned int z=0; z<data->m_occ_mesh_points.length(); z++)
						{
							result = intersect(data->m_points[i], data->m_idirection, data->m_occ_mesh_points[z], data->m_occ_mesh_radius);
							if (result.hit)
								break;
						}
					}
					if (data->m_occ_self_radius > 0.f && !result.hit)
					{
						for (unsigned int z=0; z<data->m_points.length(); z++)
						{
							if (i!=z)
								result = intersect(data->m_points[i], data->m_idirection, data->m_points[z], data->m_occ_self_radius);
							if (result.hit)
								break;
						}
					}
					if (result.hit)
						data->m_velocities[i] = MVector(0,0,0);
					else
					{
						if (data->m_noise_weight > 0.f)
						{
							noise = data->m_noise_weight * simplexNoise(data->m_noise_octaves, data->m_noise_persistence, data->m_current_time, data->m_noise_time_mult, data->m_noise_frequency, data->m_noise_phase, data->m_points[i]);
						}

						if (data->m_gust_weight > 0.f)
						{
							mag = (data->m_magnitude * (1 - data->m_gust_weight)) + (data->m_magnitude * data->m_gust_weight * simplexNoiseFloat(data->m_gust_octaves, data->m_gust_persistence, data->m_current_time, data->m_gust_time_mult, data->m_gust_frequency, data->m_gust_phase, data->m_points[i], data->m_gust_bounds[0], data->m_gust_bounds[1]));
						}

						data->m_velocities[i] = (data->m_mdirection + noise) * mag;

						if (data->m_swirl_weight > 0.f)
						{
							data->m_velocities[i] = (data->m_velocities[i] * (1-data->m_swirl_weight)) + (( data->m_velocities[i] ^ simplexNoise(data->m_swirl_octaves, data->m_swirl_persistence, data->m_current_time, data->m_swirl_time_mult, data->m_swirl_frequency, data->m_swirl_phase, data->m_points[i])) * data->m_swirl_weight);
						}

						float dist = (data->m_points[i] - data->m_position).length();

						if (data->m_attenuation > 0.f && data->m_use_max_distance)
						{
							if (dist <= data->m_max_distance)
							{
								oneoverdist = 1.f - (dist/data->m_max_distance);
								switch (data->m_attenuation_mode)
								{
									case 0:
										step = linear(oneoverdist, 0.f, data->m_attenuation);
										break;
									case 1:
										step = smooth(oneoverdist, 0.f, data->m_attenuation);
										break;
									case 2:
										step = squared(oneoverdist, 0.f, data->m_attenuation);
										break;
									case 3:
										step = cubed(oneoverdist, 0.f, data->m_attenuation);
										break;
									case 4:
										step = invsquared(oneoverdist, 0.f, data->m_attenuation);
										break;
									case 5:
										step = invcubed(oneoverdist, 0.f, data->m_attenuation);
										break;
									default:
										step = 1.f;
										break;
								}
								data->m_velocities[i] *= step;
							}
							else
							{
								data->m_velocities[i] *= 0.f;
							}
						}
						//if (data->m_max_distance > 0.f && data->m_use_max_distance)
						//{
						//	data->m_velocities[i] *= falloffCurve(dist/data->m_max_distance);
						//}
					}
					float velmag = data->m_velocities[i].length();
					if (velmag > data->m_max_mag)
					{
						data->m_max_mag = velmag;
					}

				}
			}
		};
};
#endif
