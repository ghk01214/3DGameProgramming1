// Vertex Shader ����, Render Target �簢�� ��ü�� �ﰢ�� 2���� ǥ��
float4 main(uint nVertexID : SV_VertexID) : SV_POSITION
{
	float4 fOutput;

	// ������ ��ġ ��ǥ�� ��ȯ�� �� ��ǥ(SV_POSITION) = ������ǥ�� ��ǥ
	if (nVertexID == 0)
		fOutput = float4(-1.0f, +1.0f, 0.0f, 1.0f);
	else if (nVertexID == 1)
		fOutput = float4(+1.0f, +1.0f, 0.0f, 1.0f);
	else if (nVertexID == 2)
		fOutput = float4(+1.0f, -1.0f, 0.0f, 1.0f);
	else if (nVertexID == 3)
		fOutput = float4(-1.0f, +1.0f, 0.0f, 1.0f);
	else if (nVertexID == 4)
		fOutput = float4(+1.0f, -1.0f, 0.0f, 1.0f);
	else if (nVertexID == 5)
		fOutput = float4(-1.0f, -1.0f, 0.0f, 1.0f);

	return fOutput;
}