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

// Pixel Shader�� �Է��� ���� ����ü ����
struct INPUT
{
	float4 position	 : SV_POSITION;
	float4 color	 : COLOR;
};

float4 Diffused(INPUT input) : SV_TARGET
{
	// �ԷµǴ� Pixel�� ������ �״�� ���-���� �ܰ�(Render Target)�� ���
	return input.color;
}