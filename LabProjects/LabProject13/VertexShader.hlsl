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

// Instancing �����͸� ���� ����ü
struct INSTANCEDGAMEOBJECTINFO
{
	matrix m_mtxGameObject;
	float4 m_cColor;
};

StructuredBuffer<INSTANCEDGAMEOBJECTINFO> gGameObjectInfos : register(t0);

struct VS_INSTANCING_INPUT
{
	float3 fPosition : POSITION;
	float4 fColor : COLOR;
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

VS_INSTANCING_OUTPUT Instancing(VS_INSTANCING_INPUT vInput, uint nInstanceID : SV_InstanceID)
{
	VS_INSTANCING_OUTPUT vOutput;

	vOutput.fPosition = mul(mul(mul(float4(vInput.fPosition, 1.0f), gGameObjectInfos[nInstanceID].m_mtxGameObject), gmtxView), gmtxProjection);
	vOutput.fColor = vInput.fColor + gGameObjectInfos[nInstanceID].m_cColor;

	return vOutput;
}