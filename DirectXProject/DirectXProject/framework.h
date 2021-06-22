// header.h: 표준 시스템 포함 파일
// 또는 프로젝트 특정 포함 파일이 들어 있는 포함 파일입니다.
//

#pragma once
#pragma comment(lib, "d3dcompiler.lib")
#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "dxguid.lib")

#pragma comment(lib, "winmm.lib")

#include "targetver.h"
#define WIN32_LEAN_AND_MEAN             // 거의 사용되지 않는 내용을 Windows 헤더에서 제외합니다.
#define _CRT_SECURE_NO_WARNINGS

// Windows 헤더 파일
#include <windows.h>
// C 런타임 헤더 파일입니다.
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>

#include <list>
#include <string>
#include <vector>
#include <iterator>
#include <algorithm>
#include <random>
#include <chrono>

#include <wrl.h>
#include <shellapi.h>
#include <timeapi.h>

#include <d3d12.h>
#include <dxgi1_4.h>
#include <d3dcompiler.h>
#include <dxgidebug.h>

#include <DirectXMath.h>
#include <DirectXPackedVector.h>
#include <DirectXColors.h>
#include <DirectXCollision.h>

#define FRAME_BUFFER_WIDTH	800
#define FRAME_BUFFER_HEIGHT	600

#define EPSILON				1.0e-10f

#define _WITH_SWAPCHAIN_FULLSCREEN_STATE

//Vertex의 색상을 무작위로(Random) 설정하기 위해 사용. 각 Vertex의 색상은 난수(Random Number)를 생성 후 지정
#define RANDOM_COLOR XMFLOAT4(rand() / (FLOAT)RAND_MAX, rand() / (FLOAT)RAND_MAX, rand() / (FLOAT)RAND_MAX, rand() / (FLOAT)RAND_MAX)

using namespace DirectX;
using namespace DirectX::PackedVector;
using Microsoft::WRL::ComPtr;

extern std::random_device rd;
extern std::default_random_engine dre;

extern ID3D12Resource* CreateBufferResource(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList,
	void* pData, UINT nBytes, D3D12_HEAP_TYPE d3dHeapType = D3D12_HEAP_TYPE_UPLOAD,
	D3D12_RESOURCE_STATES d3dResourceStates = D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER,
	ID3D12Resource** ppd3dUploadBuffer = nullptr);

inline BOOL IsZero(FLOAT fValue) { return (fabsf(fValue) < EPSILON); }
inline BOOL IsEqual(FLOAT fA, FLOAT fB) { return ::IsZero(fA - fB); }
inline FLOAT InverseSqrt(FLOAT fValue) { return 1.0f / sqrtf(fValue); }
inline void Swap(FLOAT* pfS, FLOAT* pfT)
{
	FLOAT fTemp{ *pfS };
	
	*pfS = *pfT;
	*pfT = fTemp;
}

// 3차원 Vector 연산
namespace Vector3
{
	inline XMFLOAT3 XMVectorToFloat3(XMVECTOR& xmvVector)
	{
		XMFLOAT3 xmf3Result;

		XMStoreFloat3(&xmf3Result, xmvVector);

		return xmf3Result;
	}

	inline XMFLOAT3 ScalarProduct(XMFLOAT3& xmf3Vector, FLOAT fScalar, BOOL bNormalize = TRUE)
	{
		XMFLOAT3 xmf3Result;

		if (bNormalize)
			XMStoreFloat3(&xmf3Result, XMVector3Normalize(XMLoadFloat3(&xmf3Vector)) * fScalar);
		else
			XMStoreFloat3(&xmf3Result, XMLoadFloat3(&xmf3Vector) * fScalar);

		return xmf3Result;
	}

	inline XMFLOAT3 Add(const XMFLOAT3& xmf3Vector1, const XMFLOAT3& xmf3Vector2)
	{
		XMFLOAT3 xmf3Result;

		XMStoreFloat3(&xmf3Result, XMLoadFloat3(&xmf3Vector1) + XMLoadFloat3(&xmf3Vector2));

		return xmf3Result;
	}

	inline XMFLOAT3 Add(XMFLOAT3& xmf3Vector1, XMFLOAT3& xmf3Vector2, FLOAT fScalar)
	{
		XMFLOAT3 xmf3Result;

		XMStoreFloat3(&xmf3Result, XMLoadFloat3(&xmf3Vector1) + (XMLoadFloat3(&xmf3Vector2) * fScalar));

		return xmf3Result;
	}

	inline XMFLOAT3 Subtract(XMFLOAT3& xmf3Vector1, XMFLOAT3& xmf3Vector2)
	{
		XMFLOAT3 xmf3Result;

		XMStoreFloat3(&xmf3Result, XMLoadFloat3(&xmf3Vector1) - XMLoadFloat3(&xmf3Vector2));

		return xmf3Result;
	}

	inline FLOAT DotProduct(XMFLOAT3& xmf3Vector1, XMFLOAT3& xmf3Vector2)
	{
		XMFLOAT3 xmf3Result;

		XMStoreFloat3(&xmf3Result, XMVector3Dot(XMLoadFloat3(&xmf3Vector1), XMLoadFloat3(&xmf3Vector2)));

		return(xmf3Result.x);
	}

	inline XMFLOAT3 CrossProduct(XMFLOAT3& xmf3Vector1, XMFLOAT3& xmf3Vector2, BOOL bNormalize = TRUE)
	{
		XMFLOAT3 xmf3Result;

		if (bNormalize)
			XMStoreFloat3(&xmf3Result, XMVector3Normalize(XMVector3Cross(XMLoadFloat3(&xmf3Vector1), XMLoadFloat3(&xmf3Vector2))));
		else
			XMStoreFloat3(&xmf3Result, XMVector3Cross(XMLoadFloat3(&xmf3Vector1), XMLoadFloat3(&xmf3Vector2)));

		return xmf3Result;
	}

	inline XMFLOAT3 Normalize(XMFLOAT3& xmf3Vector)
	{
		XMFLOAT3 m_xmf3Normal;

		XMStoreFloat3(&m_xmf3Normal, XMVector3Normalize(XMLoadFloat3(&xmf3Vector)));

		return m_xmf3Normal;
	}

	inline FLOAT Length(XMFLOAT3& xmf3Vector)
	{
		XMFLOAT3 xmf3Result;

		XMStoreFloat3(&xmf3Result, XMVector3Length(XMLoadFloat3(&xmf3Vector)));

		return xmf3Result.x;
	}

	inline FLOAT Angle(XMVECTOR& xmvVector1, XMVECTOR& xmvVector2)
	{
		XMVECTOR xmvAngle{ XMVector3AngleBetweenNormals(xmvVector1, xmvVector2) };

		return XMConvertToDegrees(acosf(XMVectorGetX(xmvAngle)));
	}

	inline FLOAT Angle(XMFLOAT3& xmf3Vector1, XMFLOAT3& xmf3Vector2)
	{
		return Angle(XMLoadFloat3(&xmf3Vector1), XMLoadFloat3(&xmf3Vector2));
	}

	inline XMFLOAT3 TransformNormal(XMFLOAT3& xmf3Vector, XMMATRIX& xmmtxTransform)
	{
		XMFLOAT3 xmf3Result;

		XMStoreFloat3(&xmf3Result, XMVector3TransformNormal(XMLoadFloat3(&xmf3Vector), xmmtxTransform));

		return xmf3Result;
	}

	inline XMFLOAT3 TransformCoord(XMFLOAT3& xmf3Vector, XMMATRIX& xmmtxTransform)
	{
		XMFLOAT3 xmf3Result;

		XMStoreFloat3(&xmf3Result, XMVector3TransformCoord(XMLoadFloat3(&xmf3Vector), xmmtxTransform));

		return xmf3Result;
	}

	inline XMFLOAT3 TransformCoord(XMFLOAT3& xmf3Vector, XMFLOAT4X4& xmmtx4x4Matrix)
	{
		return TransformCoord(xmf3Vector, XMLoadFloat4x4(&xmmtx4x4Matrix));
	}

	inline BOOL IsZero(XMFLOAT3& xmf3Vector)
	{
		if (::IsZero(xmf3Vector.x) && ::IsZero(xmf3Vector.y) && ::IsZero(xmf3Vector.z))
			return TRUE;

		return FALSE;
	}
}

// 4차원 Vector 연산
namespace Vector4
{
	inline XMFLOAT4 Add(XMFLOAT4& xmf4Vector1, XMFLOAT4& xmf4Vector2)
	{
		XMFLOAT4 xmf4Result;

		XMStoreFloat4(&xmf4Result, XMLoadFloat4(&xmf4Vector1) + XMLoadFloat4(&xmf4Vector2));

		return xmf4Result;
	}

	inline XMFLOAT4 Multiply(XMFLOAT4& xmf4Vector1, XMFLOAT4& xmf4Vector2)
	{
		XMFLOAT4 xmf4Result;

		XMStoreFloat4(&xmf4Result, XMLoadFloat4(&xmf4Vector1) * XMLoadFloat4(&xmf4Vector2));

		return xmf4Result;
	}

	inline XMFLOAT4 Multiply(FLOAT fScalar, XMFLOAT4& xmf4Vector)
	{
		XMFLOAT4 xmf4Result;

		XMStoreFloat4(&xmf4Result, fScalar * XMLoadFloat4(&xmf4Vector));

		return xmf4Result;
	}
}

// 행렬 연산
namespace Matrix4x4
{
	inline XMFLOAT4X4 Identity()
	{
		XMFLOAT4X4 xmmtx4x4Result;

		XMStoreFloat4x4(&xmmtx4x4Result, XMMatrixIdentity());

		return xmmtx4x4Result;
	}

	inline XMFLOAT4X4 Multiply(XMFLOAT4X4& xmmtx4x4Matrix1, XMFLOAT4X4& xmmtx4x4Matrix2)
	{
		XMFLOAT4X4 xmmtx4x4Result;

		XMStoreFloat4x4(&xmmtx4x4Result, XMLoadFloat4x4(&xmmtx4x4Matrix1) * XMLoadFloat4x4(&xmmtx4x4Matrix2));

		return xmmtx4x4Result;
	}

	inline XMFLOAT4X4 Multiply(XMFLOAT4X4& xmmtx4x4Matrix1, XMMATRIX& xmmtxMatrix2)
	{
		XMFLOAT4X4 xmmtx4x4Result;

		XMStoreFloat4x4(&xmmtx4x4Result, XMLoadFloat4x4(&xmmtx4x4Matrix1) * xmmtxMatrix2);

		return xmmtx4x4Result;
	}

	inline XMFLOAT4X4 Multiply(XMMATRIX& xmmtxMatrix1, XMFLOAT4X4& xmmtx4x4Matrix2)
	{
		XMFLOAT4X4 xmmtx4x4Result;

		XMStoreFloat4x4(&xmmtx4x4Result, xmmtxMatrix1 * XMLoadFloat4x4(&xmmtx4x4Matrix2));

		return xmmtx4x4Result;
	}

	inline XMFLOAT4X4 Inverse(XMFLOAT4X4& xmmtx4x4Matrix)
	{
		XMFLOAT4X4 xmmtx4x4Result;

		XMStoreFloat4x4(&xmmtx4x4Result, XMMatrixInverse(nullptr, XMLoadFloat4x4(&xmmtx4x4Matrix)));

		return xmmtx4x4Result;
	}

	inline XMFLOAT4X4 Transpose(XMFLOAT4X4& xmmtx4x4Matrix)
	{
		XMFLOAT4X4 xmmtx4x4Result;

		XMStoreFloat4x4(&xmmtx4x4Result, XMMatrixTranspose(XMLoadFloat4x4(&xmmtx4x4Matrix)));

		return xmmtx4x4Result;
	}

	inline XMFLOAT4X4 PerspectiveFovLH(FLOAT FovAngleY, FLOAT AspectRatio, FLOAT NearZ, FLOAT FarZ)
	{
		XMFLOAT4X4 xmmtx4x4Result;

		XMStoreFloat4x4(&xmmtx4x4Result, XMMatrixPerspectiveFovLH(FovAngleY, AspectRatio, NearZ, FarZ));

		return xmmtx4x4Result;
	}

	inline XMFLOAT4X4 LookAtLH(XMFLOAT3& xmf3EyePosition, XMFLOAT3& xmf3LookAtPosition, XMFLOAT3& xmf3UpDirection)
	{
		XMFLOAT4X4 xmmtx4x4Result;

		XMStoreFloat4x4(&xmmtx4x4Result, XMMatrixLookAtLH(XMLoadFloat3(&xmf3EyePosition), XMLoadFloat3(&xmf3LookAtPosition), XMLoadFloat3(&xmf3UpDirection)));

		return xmmtx4x4Result;
	}
}