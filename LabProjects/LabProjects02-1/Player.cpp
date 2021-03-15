#include "stdafx.h"
#include "Player.h"

void CPlayer::SetPosition(FLOAT x, FLOAT y, FLOAT z)
{
	// 플레이어 객체의 위치와 카메라의 위치를 설정한다
	CGameObject::SetPosition(x, y, z);

	if (m_pCamera)
	{
		m_pCamera->SetPosition(x, y, z);
	}
}

void CPlayer::SetRotation(FLOAT x, FLOAT y, FLOAT z)
{
	// 플레이어 객체와 카메라의 회전 각도를 설정한다
	CGameObject::SetRotation(x, y, z);

	if (m_pCamera)
	{
		m_pCamera->SetRotation(x, y, z);
	}
}

void CPlayer::Move(FLOAT x, FLOAT y, FLOAT z)
{
	// 플레이어 객체와 카메라를 이동한다
	if (m_pCamera)
	{
		m_pCamera->Move(x, y, z);
	}

	CGameObject::Move(x, y, z);
}

void CPlayer::Rotate(FLOAT fPitch, FLOAT fYaw, FLOAT fRoll)
{
	// 플레이어 객체와 카메라를 회전한다
	if (m_pCamera)
	{
		m_pCamera->Rotate(fPitch, fYaw, fRoll);
	}

	CGameObject::Rotate(fPitch, fYaw, fRoll);
}