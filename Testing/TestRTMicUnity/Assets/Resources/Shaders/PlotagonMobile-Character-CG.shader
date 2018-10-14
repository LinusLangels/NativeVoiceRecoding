Shader "Plotagon/PlotagonMobile-Character-CG" {
	Properties {
		_MainTex ("Base (RGB)", 2D) = "white" {}
		_Bump ("Normal Map", 2D) = "white" {}
		_DiffuseColor ("Diffuse Color", Color) = (1,1,1,1)
		_ShadowColor ("Shadow Color", Color) = (0,0,0,0)
		_LightRamp ("Light Ramp", 2D) = "white" {}
	}
	
	SubShader {
		Tags { "RenderType"="Opaque" }
		

		Pass
		{
			Tags { "LightMode" = "ForwardBase" }
			Zwrite On
			
			CGPROGRAM
			#pragma vertex vert
			#pragma fragment frag
			#pragma multi_compile_fwdbase
			#pragma fragmentoption ARB_precision_hint_fastest
			#include "UnityCG.cginc"
			#pragma only_renderers d3d9 opengl gles
			
			sampler2D _MainTex, _LightRamp, _Bump;
			float4 _MainTex_ST, _DiffuseColor, _ShadowColor;
			
			uniform fixed4 _LightColor0;
			
			struct a2v
			{
				float4 vertex		: POSITION;
				float4 uv			: TEXCOORD0;
				float3 normal		: NORMAL;
				float4 tangent		: TANGENT;
			};
			
			struct v2f
			{
				float4 pos				: SV_POSITION;
				float2 uv1				: TEXCOORD0;
				float3 normal			: TEXCOORD1;
				float3 tangentLightDir	: TEXCOORD2;
			};
			
			float3 UnpackNormalMap(float4 bump)
			{
				float3 remappedCoords = float3(2.0 * bump.w - 1.0, 2.0 * bump.y - 1.0, 0.0);
            	remappedCoords.z = sqrt(1.0 - dot(remappedCoords, remappedCoords));
            	return remappedCoords;
			}
			
			v2f vert(a2v v)
			{
				v2f output;
				
				float4x4 modelMatrix = _Object2World;
				float4x4 modelMatrixInverse = _World2Object;
				
				output.pos = mul(UNITY_MATRIX_MVP, v.vertex);
				output.uv1 = _MainTex_ST.xy * v.uv + _MainTex_ST.zw;
				output.normal = normalize(mul(float4(v.normal, 0), modelMatrixInverse).xyz);
				
				float3 binormal = cross( v.normal, v.tangent.xyz ) * v.tangent.w;
				float3x3 rotation = float3x3( v.tangent.xyz, binormal, v.normal );
				
				half3 objLight = mul((float3x3)modelMatrixInverse, normalize(_WorldSpaceLightPos0.xyz));
				output.tangentLightDir = mul(rotation, objLight);
				
				return output;
			}
			
			fixed4 frag(v2f IN) : COLOR
			{
				fixed3 col = tex2D(_MainTex, IN.uv1).rgb;
				fixed3 normal = UnpackNormalMap(tex2D(_Bump, IN.uv1));
				normal.z *= 0.75;

				fixed NdotL = saturate(dot(normal, IN.tangentLightDir) * 0.5 + 0.5);
				fixed3 ramp = tex2D(_LightRamp, fixed2(NdotL, 0));
				fixed3 diff = lerp(_ShadowColor, _DiffuseColor, ramp) * col * _LightColor0.rgb; 
				fixed3 AmbientCol = ShadeSH9(float4(IN.normal, 1)) * col.rgb; //lightprobes and ambient data from unity
				
				return fixed4(diff + AmbientCol, 0);
			}
			
			ENDCG
		}
	} 
	
	FallBack "Diffuse"
}
