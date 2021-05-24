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

// Vertex Shader ����
VS_OUTPUT main(VS_INPUT vInput)
{
	VS_OUTPUT vOutput;

	vOutput.fPosition = float4(vInput.fPosition, 1.0f);		// Vertex�� ��ġ ���ʹ� ������ǥ��� ǥ���Ǿ� �����Ƿ� ��ȯ���� �ʰ� �״�� ���
	vOutput.fColor = vInput.fColor;							// �ԷµǴ� Pixel�� ����(Rasterizer �ܰ迡�� �����Ͽ� ���� ����)�� �״�� ���

	return vOutput;
}