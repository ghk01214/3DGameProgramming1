#include "stdafx.h"
#include "Scene.h"

CScene::CScene()
{
}

CScene::~CScene()
{
}

void CScene::OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
{
}

void CScene::OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
{
	switch (nMessageID)
	{
	case WM_KEYDOWN:
	{
		switch (wParam)
		{
		case '1':
		case '2':
		case '3':
		case '4':
		case '5':
		case '6':
		case '7':
		case '8':
		case '9':
		{
			CEnemyObject* pEnemyObject = (CEnemyObject*)m_ppObjects[INT(wParam - '1')];
			pEnemyObject->m_bBlowingUp = TRUE;
		
			break;
		}
		case 'A':
		{
			for (INT i = 0; i < m_nObjects; ++i)
			{
				CEnemyObject* pEnemyObject = (CEnemyObject*)m_ppObjects[i];

				pEnemyObject->m_bBlowingUp = TRUE;
			}

			break;
		}
		default:
			break;
		}
	
		break;
	}
	default:
		break;
	}
}

void CScene::BuildObjects()
{
	CEnemyObject::PrepareExplosion();

	FLOAT fHalfWidth = 45.0f, fHalfHeight = 45.0f, fHalfDepth = 30.0f;
	CWallMesh* pWallCubeMesh = new CWallMesh(fHalfWidth * 2.0f, fHalfHeight * 2.0f, fHalfDepth * 2.0f, 30);

	m_pWallsObject = new CWallsObject();
	m_pWallsObject->SetPosition(0.0f, 0.0f, 0.0f);
	m_pWallsObject->SetMesh(pWallCubeMesh);
	m_pWallsObject->SetColor(RGB(0, 0, 0));
	m_pWallsObject->m_pxmf4WallPlanes[0] = XMFLOAT4(+1.0f, 0.0f, 0.0f, fHalfWidth);
	m_pWallsObject->m_pxmf4WallPlanes[1] = XMFLOAT4(-1.0f, 0.0f, 0.0f, fHalfWidth);
	m_pWallsObject->m_pxmf4WallPlanes[2] = XMFLOAT4(0.0f, +1.0f, 0.0f, fHalfHeight);
	m_pWallsObject->m_pxmf4WallPlanes[3] = XMFLOAT4(0.0f, -1.0f, 0.0f, fHalfHeight);
	m_pWallsObject->m_pxmf4WallPlanes[4] = XMFLOAT4(0.0f, 0.0f, +1.0f, fHalfDepth);
	m_pWallsObject->m_pxmf4WallPlanes[5] = XMFLOAT4(0.0f, 0.0f, -1.0f, fHalfDepth);
	m_pWallsObject->m_xmOOBBPlayerMoveCheck = BoundingOrientedBox(XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(fHalfWidth, fHalfHeight, fHalfDepth * 0.05f), XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f));

	CEnemyMesh* pEnemyCarMesh = new CEnemyMesh(4.0f, 4.0f, 4.0f);

	m_nObjects = 10;
	m_ppObjects = new CGameObject * [m_nObjects];

	for (int i = 0; i < 10; ++i)
	{
		m_ppObjects[i] = new CEnemyObject();
		m_ppObjects[i]->SetMesh(pEnemyCarMesh);
		m_ppObjects[i]->SetRotationSpeed(0.0f);
		m_ppObjects[i]->SetMovingDirection(XMFLOAT3(0.0f, 0.0f, -1.0f));
		m_ppObjects[i]->SetRotationAxis(XMFLOAT3(0.0f, 0.0f, 0.0f));
	}

	m_ppObjects[0]->SetColor(RGB(255, 0, 0));
	m_ppObjects[0]->SetPosition(-13.5f, 0.0f, -14.0f);
	m_ppObjects[0]->SetMovingSpeed(10.5f);

	m_ppObjects[1]->SetColor(RGB(0, 0, 255));
	m_ppObjects[1]->SetPosition(+13.5f, 0.0f, -14.0f);
	m_ppObjects[1]->SetMovingSpeed(8.8f);

	m_ppObjects[2]->SetColor(RGB(0, 255, 0));
	m_ppObjects[2]->SetPosition(0.0f, 0.0f, 20.0f);
	m_ppObjects[2]->SetMovingSpeed(5.2f);

	m_ppObjects[3]->SetColor(RGB(0, 255, 255));
	m_ppObjects[3]->SetPosition(0.0f, 0.0f, 0.0f);
	m_ppObjects[3]->SetMovingSpeed(20.4f);

	m_ppObjects[4]->SetColor(RGB(128, 0, 255));
	m_ppObjects[4]->SetPosition(10.0f, 0.0f, 0.0f);
	m_ppObjects[4]->SetMovingSpeed(6.4f);

	m_ppObjects[5]->SetColor(RGB(255, 0, 255));
	m_ppObjects[5]->SetPosition(-10.0f, 0.0f, -10.0f);
	m_ppObjects[5]->SetMovingSpeed(8.9f);

	m_ppObjects[6]->SetColor(RGB(255, 0, 255));
	m_ppObjects[6]->SetPosition(-10.0f, 0.0f, -10.0f);
	m_ppObjects[6]->SetMovingSpeed(9.7f);

	m_ppObjects[7]->SetColor(RGB(255, 0, 128));
	m_ppObjects[7]->SetPosition(-10.0f, 0.0f, -20.0f);
	m_ppObjects[7]->SetMovingSpeed(15.6f);

	m_ppObjects[8]->SetColor(RGB(128, 0, 255));
	m_ppObjects[8]->SetPosition(-15.0f, 0.0f, -30.0f);
	m_ppObjects[8]->SetMovingSpeed(15.0f);

	m_ppObjects[9]->SetColor(RGB(255, 64, 64));
	m_ppObjects[9]->SetPosition(+15.0f, 0.0f, 0.0f);
	m_ppObjects[9]->SetMovingSpeed(15.0f);
}

void CScene::ReleaseObjects()
{
	if (CEnemyObject::m_pExplosionMesh)
		CEnemyObject::m_pExplosionMesh->Release();

	for (INT i = 0; i < m_nObjects; ++i)
	{
		if (m_ppObjects[i])
			delete m_ppObjects[i];
	}

	if (m_ppObjects)
		delete[] m_ppObjects;

	if (m_pWallsObject)
		delete m_pWallsObject;
}

CGameObject* CScene::PickObjectPointedByCursor(INT xClient, INT yClient, CCamera* pCamera)
{
	XMFLOAT3 xmf3PickPosition;
	xmf3PickPosition.x = (((2.0f * xClient) / pCamera->m_d3dViewport.Width) - 1) / pCamera->m_xmf4x4Projection._11;
	xmf3PickPosition.y = -(((2.0f * yClient) / pCamera->m_d3dViewport.Height) - 1) / pCamera->m_xmf4x4Projection._22;
	xmf3PickPosition.z = 1.0f;

	XMVECTOR xmvPickPosition = XMLoadFloat3(&xmf3PickPosition);
	XMMATRIX xmmtxView = XMLoadFloat4x4(&pCamera->m_xmf4x4View);

	INT nIntersected = 0;
	FLOAT fNearestHitDistance = FLT_MAX;
	CGameObject* pNearestObject = nullptr;

	for (INT i = 0; i < m_nObjects; ++i)
	{
		FLOAT fHitDistance = FLT_MAX;

		nIntersected = m_ppObjects[i]->PickObjectByRayIntersection(xmvPickPosition, xmmtxView, &fHitDistance);

		if ((nIntersected > 0) && (fHitDistance < fNearestHitDistance))
		{
			fNearestHitDistance = fHitDistance;
			pNearestObject = m_ppObjects[i];
		}
	}

	return pNearestObject;
}

void CScene::CheckObjectByObjectCollisions()
{
	for (INT i = 0; i < m_nObjects; ++i)
	{
		m_ppObjects[i]->m_pObjectCollided = nullptr;
	}

	for (INT i = 0; i < m_nObjects; ++i)
	{
		for (INT j = (i + 1); j < m_nObjects; ++j)
		{
			if (m_ppObjects[i]->m_xmOOBB.Intersects(m_ppObjects[j]->m_xmOOBB))
			{
				m_ppObjects[i]->m_pObjectCollided = m_ppObjects[j];
				m_ppObjects[j]->m_pObjectCollided = m_ppObjects[i];
			}
		}
	}

	for (INT i = 0; i < m_nObjects; ++i)
	{
		if (m_ppObjects[i]->m_pObjectCollided)
		{
			XMFLOAT3 xmf3MovingDirection = m_ppObjects[i]->m_xmf3MovingDirection;
			FLOAT fMovingSpeed = m_ppObjects[i]->m_fMovingSpeed;

			m_ppObjects[i]->m_xmf3MovingDirection = m_ppObjects[i]->m_pObjectCollided->m_xmf3MovingDirection;
			m_ppObjects[i]->m_fMovingSpeed = m_ppObjects[i]->m_pObjectCollided->m_fMovingSpeed;
			m_ppObjects[i]->m_pObjectCollided->m_xmf3MovingDirection = xmf3MovingDirection;
			m_ppObjects[i]->m_pObjectCollided->m_fMovingSpeed = fMovingSpeed;
			m_ppObjects[i]->m_pObjectCollided->m_pObjectCollided = nullptr;
			m_ppObjects[i]->m_pObjectCollided = nullptr;
		}
	}
}

void CScene::CheckObjectByWallCollisions()
{
	for (INT i = 0; i < m_nObjects; ++i)
	{
		ContainmentType containType = m_pWallsObject->m_xmOOBB.Contains(m_ppObjects[i]->m_xmOOBB);

		switch (containType)
		{
		case DISJOINT:
		{
			INT nPlaneIndex = -1;

			for (INT j = 0; j < 6; ++j)
			{
				PlaneIntersectionType intersectType = m_ppObjects[i]->m_xmOOBB.Intersects(XMLoadFloat4(&m_pWallsObject->m_pxmf4WallPlanes[j]));

				if (intersectType == BACK)
				{
					nPlaneIndex = j;

					break;
				}
			}

			if (nPlaneIndex != -1)
			{
				XMVECTOR xmvNormal = XMVectorSet(m_pWallsObject->m_pxmf4WallPlanes[nPlaneIndex].x, m_pWallsObject->m_pxmf4WallPlanes[nPlaneIndex].y, m_pWallsObject->m_pxmf4WallPlanes[nPlaneIndex].z, 0.0f);
				XMVECTOR xmvReflect = XMVector3Reflect(XMLoadFloat3(&m_ppObjects[i]->m_xmf3MovingDirection), xmvNormal);
				
				XMStoreFloat3(&m_ppObjects[i]->m_xmf3MovingDirection, xmvReflect);
			}

			break;
		}
		case INTERSECTS:
		{
			INT nPlaneIndex = -1;
			
			for (INT j = 0; j < 6; ++j)
			{
				PlaneIntersectionType intersectType = m_ppObjects[i]->m_xmOOBB.Intersects(XMLoadFloat4(&m_pWallsObject->m_pxmf4WallPlanes[j]));
				
				if (intersectType == INTERSECTING)
				{
					nPlaneIndex = j;
					
					break;
				}
			}

			if (nPlaneIndex != -1)
			{
				XMVECTOR xmvNormal = XMVectorSet(m_pWallsObject->m_pxmf4WallPlanes[nPlaneIndex].x, m_pWallsObject->m_pxmf4WallPlanes[nPlaneIndex].y, m_pWallsObject->m_pxmf4WallPlanes[nPlaneIndex].z, 0.0f);
				XMVECTOR xmvReflect = XMVector3Reflect(XMLoadFloat3(&m_ppObjects[i]->m_xmf3MovingDirection), xmvNormal);
				
				XMStoreFloat3(&m_ppObjects[i]->m_xmf3MovingDirection, xmvReflect);
			}

			break;
		}
		case CONTAINS:
			break;
		}
	}
}

void CScene::CheckPlayerByWallCollision()
{
	BoundingOrientedBox xmOOBBPlayerMoveCheck;
	
	m_pWallsObject->m_xmOOBBPlayerMoveCheck.Transform(xmOOBBPlayerMoveCheck, XMLoadFloat4x4(&m_pWallsObject->m_xmf4x4World));
	
	XMStoreFloat4(&xmOOBBPlayerMoveCheck.Orientation, XMQuaternionNormalize(XMLoadFloat4(&xmOOBBPlayerMoveCheck.Orientation)));

	if (!xmOOBBPlayerMoveCheck.Intersects(m_pPlayer->m_xmOOBB))
		m_pWallsObject->SetPosition(m_pPlayer->m_xmf3Position);
}

void CScene::CheckPlayerByObjectCollision()
{
	BoundingOrientedBox xmOOBBPlayerMoveCheck;

	
}

void CScene::Animate(FLOAT fElapsedTime)
{
	m_pWallsObject->Animate(fElapsedTime);

	for (INT i = 0; i < m_nObjects; ++i)
	{
		m_ppObjects[i]->Animate(fElapsedTime);
	}

	CheckPlayerByObjectCollision();
	CheckPlayerByWallCollision();
	CheckObjectByWallCollisions();
	CheckObjectByObjectCollisions();
}

void CScene::Render(HDC hDCFrameBuffer, CCamera* pCamera)
{
	m_pWallsObject->Render(hDCFrameBuffer, pCamera);

	for (INT i = 0; i < m_nObjects; ++i)
	{
		m_ppObjects[i]->Render(hDCFrameBuffer, pCamera);
	}
}

