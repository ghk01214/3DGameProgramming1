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

// Pixel Shader의 입력을 위한 구조체 선언
struct INPUT
{
	float4 position	 : SV_POSITION;
	float4 color	 : COLOR;
};

float4 Diffused(INPUT input) : SV_TARGET
{
	// 입력되는 Pixel의 색상을 그대로 출력-병합 단계(Render Target)로 출력
	return input.color;
}