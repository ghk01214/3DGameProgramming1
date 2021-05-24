// Vertex Shader 정의
float4 main(uint nVertexID : SV_VertexID) : SV_POSITION
{
	float4 fOutput;

	// Primitive(삼각형)을 구성하는 정점의 인덱스(SV_VertexID)에 따라 정점 반환
	// 정점의 위치 좌표는 변환이 된 좌표(SV_POSITION) = 투영좌표계 좌표
	if (nVertexID == 0)
		fOutput = float4(0.0, 0.5, 0.5, 1.0);
	else if (nVertexID == 1)
		fOutput = float4(0.5, -0.5, 0.5, 1.0);
	else if (nVertexID == 2)
		fOutput = float4(-0.5, -0.5, 0.5, 1.0);

	return fOutput;
}