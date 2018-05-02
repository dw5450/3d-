#pragma once

#include "GameObject.h"


class CPlayer : public CGameObject
{
public:
	CPlayer();
	virtual ~CPlayer();

	XMFLOAT3					m_xmf3Position;
	XMFLOAT3					m_xmf3Right;
	XMFLOAT3					m_xmf3Up;
	XMFLOAT3					m_xmf3Look;

	XMFLOAT3					m_xmf3CameraOffset;
	XMFLOAT3					m_xmf3Velocity;
	float						m_fFriction = 125.0f;

	float           			m_fPitch = 0.0f;
	float           			m_fYaw = 0.0f;
	float           			m_fRoll = 0.0f;

	CCamera						*m_pCamera = NULL;

	void SetPosition(float x, float y, float z);
	void Move(DWORD dwDirection, float fDistance);
	void Move(XMFLOAT3& xmf3Shift, bool bUpdateVelocity);
	void Move(float x, float y, float z);
	void Rotate(float fPitch = 0.0f, float fYaw = 0.0f, float fRoll = 0.0f);
	void SetCameraOffset(XMFLOAT3& xmf3CameraOffset);
	
	void Update(float fTimeElapsed=0.016f);

	virtual void Animate(float fElapsedTime);
	virtual void Render(HDC hDCFrameBuffer, CCamera *pCamera);


public:		//skill
	bool					m_bShotBullet = false;
	float					m_fBulletCooltime = BULLETCOOLTIME;
	float					m_fBulletInitCooltime = BULLETCOOLTIME;
	unsigned int			m_iBulletMaxNum = MAXBULLETNUM;
	unsigned int			m_iBulletNum = MAXBULLETNUM;
	bool CanShot();
	CBullet * ShotBullet();
	
	bool					m_bReload	= false;
	float					m_bReloadTime = RELOADTIME;
	float					m_bReloadInitTime = RELOADTIME;
	void ReloadBullet(float fElapseTime);


	float					m_fBombCooltime = BULLETCOOLTIME;
	float					m_fBombInitCooltime = BULLETCOOLTIME;
	int m_iBombNum = 10;
	bool m_bShotBomb = false;
	bool ShotBomb();

	unsigned int m_iLife = 5;
};

