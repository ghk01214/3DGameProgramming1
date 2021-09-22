cbuffer cbFrameworkInfo : register(b0)
{
    float	 gfCurrentTime;
    float	 gfElapsedTime;
    float2	 gf2CursorPos;
};

cbuffer cbGameObjectInfo : register(b1)
{
	matrix gmtxWorld		 : packoffset(c0);
    float3 gf3ObjectColor	 : packoffset(c4);
};

// ī�޶��� ������ ���� ��� ���� ����
cbuffer cbCameraInfo : register(b2)
{
	matrix gmtxView			 : packoffset(c0);
	matrix gmtxProjection	 : packoffset(c4);
    float3 gf3CameraPosition : packoffset(c8);
};

// Vertex Shader�� �Է��� ���� ����ü ����
struct INPUT
{
	float3 position	 : POSITION;
	float4 color	 : COLOR;
};

struct LIGHTING_INPUT
{
    float3 position  : POSITION;
    float3 normal    : NORMAL;
    float2 uv		 : TEXTURECOORD;
};

// Vertex Shader�� ���(Pixel Shader�� �Է�)�� ���� ����ü ����
struct OUTPUT
{
	float4 position		 : SV_POSITION;
	float4 color		 : COLOR;
};

struct LIGHTING_OUTPUT
{
    float4 positionH	 : SV_POSITION;
    float3 positionW	 : POSITION;
    float3 normal		 : NORMAL0;
    float3 normalW		 : NORMAL1;
    float2 uv			 : TEXTURECOORD;
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

LIGHTING_OUTPUT PseudoLighting(LIGHTING_INPUT input)
{
    LIGHTING_OUTPUT output;

    output.positionW     = mul(float4(input.position, 1.0f), gmtxWorld).xyz;
    output.positionH     = mul(mul(float4(output.positionW, 1.0f), gmtxView), gmtxProjection);
    output.normalW       = mul(float4(input.normal, 0.0f), gmtxWorld).xyz;
    output.normal        = input.normal;
    output.uv            = input.uv;

    return output;
}