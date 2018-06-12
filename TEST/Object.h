//------------------------------------------------------- ----------------------
// File: Object.h
//-----------------------------------------------------------------------------

#pragma once

#include "Mesh.h"
#include "Camera.h"

class CShader;

class CGameObject
{
public:
	CGameObject();
	virtual ~CGameObject();

public:
	bool							m_bActive = true;
	float							m_fElapseTime = 0;
	XMFLOAT4X4						m_xmf4x4World;
	CMesh							*m_pMesh = NULL;
	BoundingBox						m_xmAABB;

	CShader							*m_pShader = NULL;

	XMFLOAT3					m_xmf3MovingDirection;
	float						m_fMovingSpeed;
	float						m_fMovingRange;

	XMFLOAT3					m_xmf3RotationAxis;
	float						m_fRotationSpeed;

	void SetMesh(CMesh *pMesh);
	void SetShader(CShader *pShader);

	XMFLOAT3 GetPosition();
	XMFLOAT3 GetLook();
	XMFLOAT3 GetUp();
	XMFLOAT3 GetRight();

	void SetPosition(float x, float y, float z);
	void SetPosition(XMFLOAT3 xmf3Position);

	void SetMovingDirection(XMFLOAT3& xmf3MovingDirection) { m_xmf3MovingDirection = Vector3::Normalize(xmf3MovingDirection); }
	void SetMovingSpeed(float fSpeed) { m_fMovingSpeed = fSpeed; }
	void SetMovingRange(float fRange) { m_fMovingRange = fRange; }

	void MoveStrafe(float fDistance = 1.0f);
	void MoveUp(float fDistance = 1.0f);
	void MoveForward(float fDistance = 1.0f);
	void Move(XMFLOAT3& vDirection, float fSpeed);

	void SetRotationAxis(XMFLOAT3& xmf3RotationAxis) { m_xmf3RotationAxis = Vector3::Normalize(xmf3RotationAxis); }
	void SetRotationSpeed(float fSpeed) { m_fRotationSpeed = fSpeed; }

	void Rotate(float fPitch = 10.0f, float fYaw = 10.0f, float fRoll = 10.0f);
	void Rotate(XMFLOAT3 *, float );

public:
	virtual void Animate(float fTimeElapsed);
	virtual void OnPrepareRender() { }
	virtual void OnPostRender() { }
	virtual void Render(ID3D12GraphicsCommandList *pd3dCommandList, CCamera *pCamera = NULL);

	virtual void ReleaseUploadBuffers();
};

/////////////////////////////////////////////////////////////////////////////////////////////////////////////
class CBullet : public CGameObject
{
public:
	CBullet();
	virtual ~CBullet();

public:
	//float	fElapseTime = 0.0f;								//경과 시간.
	float m_fShootingRange = 100.0f;
	float m_fMaxShootingRange = 100.0f;
	bool ChecksShootingRange();


	virtual void Animate(float fElapsedTime);
};

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


class CWallsObject : public CGameObject
{
public:
	CWallsObject();
	virtual ~CWallsObject();


public:

	virtual void Animate(float fElapsedTime);
};


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////


class CExplosiveObject : public  CGameObject
{
	enum {
		EXPLOSION_DEBRISES = 100
	};
public:
	CExplosiveObject();
	virtual ~CExplosiveObject();

	bool						m_bBlowingUp = false;

	XMFLOAT4X4					m_pxmf4x4Transforms[EXPLOSION_DEBRISES];

	float						m_fElapsedTimes = 0.0f;
	float						m_fDuration = 1.0f;
	float						m_fExplosionSpeed = 10.0f;
	float						m_fExplosionRotation = 720.0f;

	int LOD = 1;
	virtual void Animate(float fElapsedTime);
	virtual void Render(ID3D12GraphicsCommandList *pd3dCommandList, CCamera *pCamera);

public:
	static CMesh				*m_pExplosionMesh;
	static XMFLOAT3				m_pxmf3SphereVectors[EXPLOSION_DEBRISES];
	static void PrepareExplosion(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList);
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class CEnermy : public CExplosiveObject
{
public:
	CEnermy() {};
	virtual ~CEnermy() {};

public:
	void TraceObject(CGameObject *);
};


//////////////////////////////////////////////////////////////////////////////////////////////////////////////

class CBoss : public CEnermy
{
public:
	CBoss() {};
	virtual ~CBoss() {};
	virtual void Animate(float fElapseTime);

public:

	size_t m_iLife = 20;

	float m_fBulletCooltime = 0.5f;
	float m_fBulletInitCooltime = 0.5f;

	bool m_bShotBullet = false;
	bool CanShot();
	CBullet * ShotBullet();

};

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

