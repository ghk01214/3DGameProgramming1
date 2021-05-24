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

// Vertex Shader 정의
VS_OUTPUT main(VS_INPUT vInput)
{
	VS_OUTPUT vOutput;

	vOutput.fPosition = float4(vInput.fPosition, 1.0f);		// Vertex의 위치 벡터는 투영좌표계로 표현되어 있으므로 변환하지 않고 그대로 출력
	vOutput.fColor = vInput.fColor;							// 입력되는 Pixel의 색상(Rasterizer 단계에서 보간하여 얻은 색상)을 그대로 출력

	return vOutput;
}