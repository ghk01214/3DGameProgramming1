// Vertex Shader 정의, Render Target 사각형 전체를 삼각형 2개로 표현
float4 main(uint nVertexID : SV_VertexID) : SV_POSITION
{
	float4 fOutput;

	// 정점의 위치 좌표는 변환이 된 좌표(SV_POSITION) = 투영좌표계 좌표
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