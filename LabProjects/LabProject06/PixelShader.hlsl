// Pixel Shader�� �Է��� ���� ����ü ����
struct PS_INPUT
{
	float4 fPosition : SV_POSITION;
	float4 fColor : COLOR;
};

float4 main(PS_INPUT pInput) : SV_TARGET
{
	// �ԷµǴ� Pixel�� ������ �״�� ���-���� �ܰ�(Render Target)�� ���
	return pInput.fColor;
}