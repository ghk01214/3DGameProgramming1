#include "stdafx.h"
#include "Scene.h"

std::random_device rd;
std::default_random_engine dre(rd());
std::uniform_int_distribution<> randomPositionX(-4, 4);
std::uniform_real_distribution<> randomPositionZ(50.f, 240.f);
std::uniform_real_distribution<> randomSpeed(20.0f, 30.0f);

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
}

void CScene::BuildObjects()
{
	CEnemyObject::PrepareExplosion();

	FLOAT fHalfWidth = 45.0f, fHalfHeight = 45.0f, fHalfDepth = 240.0f;
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

	CEnemyMesh* pEnemyCarMesh = new CEnemyMesh(6.0f, 6.0f, 6.0f);
	DWORD dwStartColor;

	m_nObjects = 18;
	m_ppObjects = new CGameObject * [m_nObjects];

	INT j = -4;

	for (int i = 0; i < m_nObjects; ++i)
	{
		if (abs(randomPositionX(dre)) % 3 == 0)
			dwStartColor = RGB(0, 0, 255);
		else
			dwStartColor = RGB(0, 0, 0);

		m_ppObjects[i] = new CEnemyObject();
		m_ppObjects[i]->SetMesh(pEnemyCarMesh);
		m_ppObjects[i]->SetRotationSpeed(0.0f);
		m_ppObjects[i]->SetMovingDirection(XMFLOAT3(0.0f, 0.0f, -1.0f));
		m_ppObjects[i]->SetRotationAxis(XMFLOAT3(0.0f, 0.0f, 0.0f));
		m_ppObjects[i]->SetColor(dwStartColor);
		m_ppObjects[i]->SetMovingSpeed(randomSpeed(dre));

		m_ppObjects[i]->SetPosition(j++ * 10.0f, 0.0f, (FLOAT)randomPositionZ(dre));

		if (j == 5)
			j = -4;
	}
}

// 월드에서 사라진 오브젝트 리스폰
void CScene::RespawnEnemyObjects()
{
	CEnemyMesh* pEnemyCarMesh = new CEnemyMesh(6.0f, 6.0f, 6.0f);
	DWORD randomColor;

	if ((randomPositionX(dre) > 0) && (randomPositionX(dre) % 3 == 0))
		randomColor = RGB(0, 0, 255);
	else
		randomColor = RGB(0, 0, 0);

	++m_nObjects;

	m_ppObjects[m_nObjects - 1] = new CEnemyObject();
	m_ppObjects[m_nObjects - 1]->SetMesh(pEnemyCarMesh);
	m_ppObjects[m_nObjects - 1]->SetRotationSpeed(0.0f);
	m_ppObjects[m_nObjects - 1]->SetMovingDirection(XMFLOAT3(0.0f, 0.0f, -1.0f));
	m_ppObjects[m_nObjects - 1]->SetRotationAxis(XMFLOAT3(0.0f, 0.0f, 0.0f));
	m_ppObjects[m_nObjects - 1]->SetColor(randomColor);
	m_ppObjects[m_nObjects - 1]->SetPosition(randomPositionX(dre) * 10.0f, 0.0f, m_pPlayer->m_xmf3Position.z + (FLOAT)randomPositionZ(dre));
	m_ppObjects[m_nObjects - 1]->SetMovingSpeed(randomSpeed(dre));
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

// 특정 오브젝트 소멸
void CScene::ReleaseEnemyObjects(INT iObjectNum)
{
	if (m_ppObjects[iObjectNum])
	{
		for (int i = iObjectNum; i < m_nObjects - 1; ++i)
		{
			m_ppObjects[i] = m_ppObjects[i + 1];
		}

		--m_nObjects;
	}
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

			// 플레이어의 뒷 벽의 뒤에 있으면 해당 오브젝트를 삭제하고 새로운 위치에 오브젝트 리스폰
			if (nPlaneIndex != -1)
			{
				ReleaseEnemyObjects(i);
				RespawnEnemyObjects();
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

			// 플레이어의 뒷 벽이랑 교차하고 있으면 해당 오브젝트를 삭제하고 새로운 위치에 오브젝트 리스폰
			if (nPlaneIndex != -1)
			{
				ReleaseEnemyObjects(i);
				RespawnEnemyObjects();
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
	{
		FLOAT xPos;

		if (m_pPlayer->m_xmf3Position.x > 45.0f)
			xPos = 40.0f;
		else if (m_pPlayer->m_xmf3Position.x < -45.0f)
			xPos = -40.0f;
		else if (m_pPlayer->m_xmf3Position.z > 240.0f * 0.05f)
			xPos = m_pPlayer->m_xmf3Position.x;

		m_pPlayer->m_xmf3Position.x = 0.0f;
		m_pWallsObject->SetPosition(m_pPlayer->m_xmf3Position);

		m_pPlayer->SetPosition(xPos, m_pPlayer->m_xmf3Position.y, m_pPlayer->m_xmf3Position.z);
		m_pPlayer->m_xmf3Position.x = xPos;
	}
}

void CScene::CheckPlayerByObjectCollision()
{
	m_pPlayer->m_pObjectCollided = nullptr;
	
	for (INT i = 0; i < m_nObjects; ++i)
	{
		if (m_pPlayer->m_xmOOBB.Intersects(m_ppObjects[i]->m_xmOOBB))
		{
			m_ppObjects[i]->m_pObjectCollided = m_pPlayer;
			m_pPlayer->m_pObjectCollided = m_ppObjects[i];
			
			break;
		}
	}

	if (m_pPlayer->m_pObjectCollided)
	{
		static std::chrono::high_resolution_clock::time_point startTime;

		if (m_pPlayer->m_bFeverMode)
		{
			CEnemyObject* pEnemyObject = (CEnemyObject*)m_pPlayer->m_pObjectCollided;

			pEnemyObject->m_bBlowingUp = TRUE;

			if (std::chrono::duration_cast<std::chrono::seconds>(std::chrono::high_resolution_clock::now() - startTime).count() > 10)
			{
				m_pPlayer->m_bFeverMode = FALSE;
				m_pPlayer->m_iFeverStack = 0;
				m_pPlayer->m_dwColor = RGB(0, 0, 0);
			}
		}
		else
		{
			if (m_pPlayer->m_pObjectCollided->m_dwColor == RGB(0, 0, 255))
			{
				++m_pPlayer->m_iFeverStack;

				CEnemyObject* pEnemyObject = (CEnemyObject*)m_pPlayer->m_pObjectCollided;

				pEnemyObject->m_bBlowingUp = TRUE;

				if (m_pPlayer->m_iFeverStack == 200)
				{
					startTime = std::chrono::high_resolution_clock::now();

					m_pPlayer->m_bFeverMode = TRUE;
					m_pPlayer->m_dwColor = RGB(255, 0, 0);
				}
			}
			else
				m_bGameOver = TRUE;
		}
	}
}

void CScene::CheckObjectOutOfCamera()
{
	for (INT i = 0; i < m_nObjects; ++i)
	{
		if (m_ppObjects[i]->m_xmf4x4World._43 < m_pPlayer->m_xmf3Position.z - 30.0f)
		{
			ReleaseEnemyObjects(i);
			RespawnEnemyObjects();
		}
	}
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
	CheckObjectOutOfCamera();
}

void CScene::Render(HDC hDCFrameBuffer, CCamera* pCamera)
{
	m_pWallsObject->Render(hDCFrameBuffer, pCamera);

	for (INT i = 0; i < m_nObjects; ++i)
	{
		m_ppObjects[i]->Render(hDCFrameBuffer, pCamera);
	}
}

