cbuffer cbGameObjectInfo : register(b0)
{
	matrix		gmtxWorld : packoffset(c0);
};

cbuffer cbCameraInfo : register(b1)
{
	matrix		gmtxView : packoffset(c0);
	matrix		gmtxProjection : packoffset(c4);
};

struct VS_INPUT
{
	float3 position : POSITION;
	float3 normal : NORMAL;
	float2 uv : TEXTURECOORD;
};

struct VS_OUTPUT
{
	float4 positionH : SV_POSITION;
	float3 positionW : POSITION;
	float3 normalW : NORMAL;
	float2 uv : TEXTURECOORD;
};

VS_OUTPUT VSLighting(VS_INPUT input)
{
	VS_OUTPUT output;

	output.positionW = mul(float4(input.position, 1.0f), gmtxWorld).xyz;
	output.positionH = mul(mul(float4(output.positionW, 1.0f), gmtxView), gmtxProjection);
	output.normalW = mul(float4(input.normal, 0.0f), gmtxWorld).xyz;
	output.uv = input.uv;

	return(output);
}

static float3 gDirectionalLight = float3(0.0f, -1.0f, 1.0f);
static float4 gLightColor = float4(1.0f, 0.5f, 0.5f, 1.0f);

float4 PSLighting(VS_OUTPUT input) : SV_TARGET
{
	float4 color = saturate(float4(0.3f + abs(input.uv.g), 0.2f + abs(input.uv.r), 0.2f, 1.0f) + gLightColor * dot(normalize(input.normalW), normalize(-gDirectionalLight)));
//	float4 color = saturate(float4(0.3f, 0.2f, 0.1f, 1.0f) + gLightColor * dot(normalize(input.normalW), normalize(-gDirectionalLight)));
//	float4 color = saturate(float4(0.3f, 0.2f, 0.1f, 1.0f) + gLightColor * abs(dot(normalize(input.normalW), normalize(-gDirectionalLight))));
	return(color);
}

