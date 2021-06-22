// Game Object�� ������ ���� ��� ���� ����
cbuffer cbGameObjectInfo : register(b0)
{
	matrix gmtxWorld : packoffset(c0);
};

// ī�޶��� ������ ���� ��� ���� ����
cbuffer cbCameraInfo : register(b1)
{
	matrix gmtxView			 : packoffset(c0);
	matrix gmtxProjection	 : packoffset(c4);
};

// Vertex Shader�� �Է��� ���� ����ü ����
struct INPUT
{
	float3 position	 : POSITION;
	float4 color	 : COLOR;
};

// Vertex Shader�� ���(Pixel Shader�� �Է�)�� ���� ����ü ����
struct OUTPUT
{
	float4 position	 : SV_POSITION;
	float4 color	 : COLOR;
};

// Vertex Shader ����
OUTPUT Diffused(INPUT input)
{
	OUTPUT output;

	// Vertex�� ��ȯ(����, ī�޶�, ����)
	output.position = mul(mul(mul(float4(input.position, 1.0f), gmtxWorld), gmtxView), gmtxProjection);
	output.color = input.color;

	return output;
}