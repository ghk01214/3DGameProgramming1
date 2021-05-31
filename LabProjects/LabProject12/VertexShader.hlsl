// Game Object의 정보를 위한 상수 버퍼 선언
cbuffer cbGameObjectInfo : register(b0)
{
	matrix gmtxWorld : packoffset(c0);
};

// 카메라의 정보를 위한 상수 버퍼 선언
cbuffer cbCameraInfo : register(b1)
{
	matrix gmtxView : packoffset(c0);
	matrix gmtxProjection : packoffset(c4);
};

// Vertex Shader의 입력을 위한 구조체 선언
struct VS_INPUT
{
	float3 fPosition : POSITION;
	float4 fColor : COLOR;
};

// Vertex Shader의 출력(Pixel Shader의 입력)을 위한 구조체 선언
struct VS_OUTPUT
{
	float4 fPosition : SV_POSITION;
	float4 fColor : COLOR;
};

// Vertex 데이터와 Instancing 데이터를 위한 구조체
struct VS_INSTANCING_INPUT
{
	float3		 fPosition : POSITION;
	float4		 fColor : COLOR;
	float4x4	 mtxTransform : WORLDMATRIX;
	float4		 instanceColor : INSTANCECOLOR;
};

struct VS_INSTANCING_OUTPUT
{
	float4 fPosition : SV_POSITION;
	float4 fColor : COLOR;
};

// Vertex Shader 정의
VS_OUTPUT Diffused(VS_INPUT vInput)
{
	VS_OUTPUT vOutput;

	// Vertex를 변환(월드, 카메라, 투영)
	vOutput.fPosition = mul(mul(mul(float4(vInput.fPosition, 1.0f), gmtxWorld), gmtxView), gmtxProjection);
	vOutput.fColor = vInput.fColor;

	return vOutput;
}

VS_INSTANCING_OUTPUT Instancing(VS_INSTANCING_INPUT vInput)
{
	VS_INSTANCING_OUTPUT vOutput;

	vOutput.fPosition = mul(mul(mul(float4(vInput.fPosition, 1.0f), vInput.mtxTransform), gmtxView), gmtxProjection);
	vOutput.fColor = vInput.fColor + vInput.instanceColor;

	return vOutput;
}