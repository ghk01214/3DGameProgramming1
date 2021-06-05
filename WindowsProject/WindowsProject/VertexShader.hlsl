// Game Object의 정보를 위한 상수 버퍼 선언
cbuffer cbGameObjectInfo : register(b0)
{
	matrix gmtxWorld : packoffset(c0);
};

// 카메라의 정보를 위한 상수 버퍼 선언
cbuffer cbCameraInfo : register(b1)
{
	matrix gmtxView			 : packoffset(c0);
	matrix gmtxProjection	 : packoffset(c4);
};

// Vertex Shader의 입력을 위한 구조체 선언
struct INPUT
{
	float3 position	 : POSITION;
	float4 color	 : COLOR;
};

// Vertex Shader의 출력(Pixel Shader의 입력)을 위한 구조체 선언
struct OUTPUT
{
	float4 position	 : SV_POSITION;
	float4 color	 : COLOR;
};

// Vertex Shader 정의
OUTPUT Diffused(INPUT input)
{
	OUTPUT output;

	// Vertex를 변환(월드, 카메라, 투영)
	output.position = mul(mul(mul(float4(input.position, 1.0f), gmtxWorld), gmtxView), gmtxProjection);
	output.color = input.color;

	return output;
}