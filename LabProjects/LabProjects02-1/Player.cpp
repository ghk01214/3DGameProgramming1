#include "stdafx.h"
#include "Player.h"

void CPlayer::SetPosition(FLOAT x, FLOAT y, FLOAT z)
{
	// �÷��̾� ��ü�� ��ġ�� ī�޶��� ��ġ�� �����Ѵ�
	CGameObject::SetPosition(x, y, z);

	if (m_pCamera)
	{
		m_pCamera->SetPosition(x, y, z);
	}
}

void CPlayer::SetRotation(FLOAT x, FLOAT y, FLOAT z)
{
	// �÷��̾� ��ü�� ī�޶��� ȸ�� ������ �����Ѵ�
	CGameObject::SetRotation(x, y, z);

	if (m_pCamera)
	{
		m_pCamera->SetRotation(x, y, z);
	}
}

void CPlayer::Move(FLOAT x, FLOAT y, FLOAT z)
{
	// �÷��̾� ��ü�� ī�޶� �̵��Ѵ�
	if (m_pCamera)
	{
		m_pCamera->Move(x, y, z);
	}

	CGameObject::Move(x, y, z);
}

void CPlayer::Rotate(FLOAT fPitch, FLOAT fYaw, FLOAT fRoll)
{
	// �÷��̾� ��ü�� ī�޶� ȸ���Ѵ�
	if (m_pCamera)
	{
		m_pCamera->Rotate(fPitch, fYaw, fRoll);
	}

	CGameObject::Rotate(fPitch, fYaw, fRoll);
}