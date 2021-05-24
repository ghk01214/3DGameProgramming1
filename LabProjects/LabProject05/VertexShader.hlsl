// Vertex Shader ����
float4 main(uint nVertexID : SV_VertexID) : SV_POSITION
{
	float4 fOutput;

	// Primitive(�ﰢ��)�� �����ϴ� ������ �ε���(SV_VertexID)�� ���� ���� ��ȯ
	// ������ ��ġ ��ǥ�� ��ȯ�� �� ��ǥ(SV_POSITION) = ������ǥ�� ��ǥ
	if (nVertexID == 0)
		fOutput = float4(0.0, 0.5, 0.5, 1.0);
	else if (nVertexID == 1)
		fOutput = float4(0.5, -0.5, 0.5, 1.0);
	else if (nVertexID == 2)
		fOutput = float4(-0.5, -0.5, 0.5, 1.0);

	return fOutput;
}