// Pixel Shader의 입력을 위한 구조체 선언
struct PS_INPUT
{
	float4 fPosition : SV_POSITION;
	float4 fColor : COLOR;
};

float4 main(PS_INPUT pInput) : SV_TARGET
{
	// 입력되는 Pixel의 색상을 그대로 출력-병합 단계(Render Target)로 출력
	return pInput.fColor;
}