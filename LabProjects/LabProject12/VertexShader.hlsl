// Game Object�� ������ ���� ��� ���� ����
cbuffer cbGameObjectInfo : register(b0)
{
	matrix gmtxWorld : packoffset(c0);
};

// ī�޶��� ������ ���� ��� ���� ����
cbuffer cbCameraInfo : register(b1)
{
	matrix gmtxView : packoffset(c0);
	matrix gmtxProjection : packoffset(c4);
};

// Vertex Shader�� �Է��� ���� ����ü ����
struct VS_INPUT
{
	float3 fPosition : POSITION;
	float4 fColor : COLOR;
};

// Vertex Shader�� ���(Pixel Shader�� �Է�)�� ���� ����ü ����
struct VS_OUTPUT
{
	float4 fPosition : SV_POSITION;
	float4 fColor : COLOR;
};

// Vertex �����Ϳ� Instancing �����͸� ���� ����ü
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

// Vertex Shader ����
VS_OUTPUT Diffused(VS_INPUT vInput)
{
	VS_OUTPUT vOutput;

	// Vertex�� ��ȯ(����, ī�޶�, ����)
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