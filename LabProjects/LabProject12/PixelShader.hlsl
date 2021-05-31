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
struct PS_INPUT
{
	float4 fPosition : SV_POSITION;
	float4 fColor : COLOR;
};

struct PS_INSTANCING_INPUT
{
	float4 fPosition : SV_POSITION;
	float4 fColor : COLOR;
};

float4 Diffused(PS_INPUT pInput) : SV_TARGET
{
	// �ԷµǴ� Pixel�� ������ �״�� ���-���� �ܰ�(Render Target)�� ���
	return pInput.fColor;
}

float4 Instancing(PS_INSTANCING_INPUT pInput) : SV_TARGET
{
	return pInput.fColor;
}