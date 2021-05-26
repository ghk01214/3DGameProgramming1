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

// Vertex Shader ����
VS_OUTPUT Diffused(VS_INPUT vInput)
{
	VS_OUTPUT vOutput;

	// Vertex�� ��ȯ(����, ī�޶�, ����)
	vOutput.fPosition = mul(mul(mul(float4(vInput.fPosition, 1.0f), gmtxWorld), gmtxView), gmtxProjection);
	vOutput.fColor = vInput.fColor;

	return vOutput;
}