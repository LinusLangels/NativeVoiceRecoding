#ifndef CUSTOM_FUNCTIONS_INCLUDED
#define CUSTOM_FUNCTIONS_INCLUDED

#define SMOOTH_NORMAL_Z 0.75
#define AmbienceOutput 0.5

#define BakedAOStrength 0.4
#define CCAOStrength 0.7

half _SmoothNormal;

//Used for DoF
uniform half _DepthFar;
uniform half _DOFApature;

inline fixed3 Normals(fixed4 packednormal)
{
	#if defined(SHADER_API_GLES) && defined(SHADER_API_MOBILE)
		return packednormal.xyz * 2 - 1;
	#else
		fixed3 normal;
		normal.xy = packednormal.wy * 2 - 1;
		normal.z = sqrt(1 - normal.x*normal.x - normal.y * normal.y) * _SmoothNormal;
		return normal;
	#endif
}

inline float ComputeDepth(float4 vert)
{
	return abs( (1 - clamp(-mul(UNITY_MATRIX_MV, vert).z / _DepthFar,0,2)) * _DOFApature);
}

float3 UnpackCreatorNormal(float4 bump)
{
	return bump.xyz * 2 - 1;
}

float3 UnpackNormalMap(float4 bump)
{
	#if defined(SHADER_API_GLES) && defined(SHADER_API_MOBILE)
		return bump.xyz * 2 - 1;
	#else
	    float3 remappedCoords = float3((2.0 * bump.wy) - float2(1.0, 1.0), 0.0);
		remappedCoords.z = sqrt(1.0 - dot(remappedCoords, remappedCoords)) * SMOOTH_NORMAL_Z;
		return remappedCoords;
	#endif
}

float3 softlightBlend(float3 base, float3 overlay, float blendFactor)
{
	float3 b_x2 = 2.0 * overlay;
	float3 a_b_x2 = base * b_x2;
	float3 c1 = a_b_x2 + base * base - base * a_b_x2;
	float3 c2 = sqrt(base) * (b_x2 - 1.0) + 2.0 * base - a_b_x2;
	float3 ret = (overlay > 0.5) ? c1 : c2;
	return lerp(base, ret, blendFactor);
}

half3 TangentSpaceLight(float3 normal, float4 tangent)
{
	float4x4 modelMatrixInverse = _World2Object;

	float3 binormal = cross(normal, tangent.xyz ) * tangent.w;
	float3x3 rotation = float3x3( tangent.xyz, binormal, normal );
	
	half3 objLight = mul((float3x3)modelMatrixInverse, normalize(_WorldSpaceLightPos0.xyz));
	return mul(rotation, objLight);
}
#endif