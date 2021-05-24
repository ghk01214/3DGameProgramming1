float4 main(float4 fInput : SV_POSITION) : SV_TARGET
{
	// Primitive의 모든 픽셀의 색상을 노란색을 반환
	return float4(1.0f, 1.0f, 0.0f, 1.0f);		// Yellow
}