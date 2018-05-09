#pragma once

#include "Mesh.h"
#include "Camera.h"

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
class CGameObject
{
public:
	CGameObject(CMesh *pMesh);
	CGameObject();
	virtual ~CGameObject();

public:
	bool						m_bActive = true;								//실행하는가?
	
	CMesh						*m_pMesh = NULL;
    XMFLOAT4X4					m_xmf4x4World;    

	BoundingBox					m_xmAABB;
	
	CGameObject					*m_pObjectCollided = NULL;

	DWORD						m_dwColor;

	XMFLOAT3					m_xmf3MovingDirection;
	float						m_fMovingSpeed;
	float						m_fMovingRange;

	XMFLOAT3					m_xmf3RotationAxis;
	float						m_fRotationSpeed;

public:
	void SetMesh(CMesh *pMesh) { m_pMesh = pMesh; if (pMesh) pMesh->AddRef(); }
	void SetColor(DWORD dwColor) { m_dwColor = dwColor; }
	void SetPosition(float x, float y, float z);
	void SetPosition(XMFLOAT3& xmf3Position);

	void SetMovingDirection(XMFLOAT3& xmf3MovingDirection) { m_xmf3MovingDirection = Vector3::Normalize(xmf3MovingDirection); }
	void SetMovingSpeed(float fSpeed) { m_fMovingSpeed = fSpeed; }
	void SetMovingRange(float fRange) { m_fMovingRange = fRange; }

	void SetRotationAxis(XMFLOAT3& xmf3RotationAxis) { m_xmf3RotationAxis = Vector3::Normalize(xmf3RotationAxis); }
	void SetRotationSpeed(float fSpeed) { m_fRotationSpeed = fSpeed; }

	void MoveStrafe(float fDistance=1.0f);
	void MoveUp(float fDistance=1.0f);
	void MoveForward(float fDistance=1.0f);
	void Move(XMFLOAT3& vDirection, float fSpeed);

	void Rotate(float fPitch=10.0f, float fYaw=10.0f, float fRoll=10.0f);
	void Rotate(XMFLOAT3& xmf3Axis, float fAngle);

	XMFLOAT3 GetPosition();
	XMFLOAT3 GetLook();
	XMFLOAT3 GetUp();
	XMFLOAT3 GetRight();

	virtual void Animate(float fElapsedTime);
	virtual void Render(HDC hDCFrameBuffer, CCamera *pCamera);
};



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


class CExplosiveObject : public CGameObject
{
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
	virtual void Render(HDC hDCFrameBuffer, CCamera *pCamera);

public:
	static CMesh				*m_pExplosionMesh;
	static XMFLOAT3				m_pxmf3SphereVectors[EXPLOSION_DEBRISES];

	static void PrepareExplosion();
};


class CEnermy : public CExplosiveObject
{
public:
	CEnermy() {};
	virtual ~CEnermy() {};

public:

	void TraceObject(CGameObject * );



};

class CBoss : public CEnermy
{
public:
	CBoss() {};
	virtual ~CBoss() {};
	virtual void Animate(float fElapseTime);

public:

	size_t m_iLife = 10;
	
	float m_fBulletCooltime = 0.5f;
	float m_fBulletInitCooltime = 0.5f;

	bool m_bShotBullet = false;
	bool CanShot();
	CBullet * ShotBullet();

};



class CBonusObject : public CExplosiveObject
{
public:
	CBonusObject() {};
	virtual ~CBonusObject() {};

};


class CWallsObject : public CGameObject
{
public:
	CWallsObject();
	virtual ~CWallsObject();


public:
	XMFLOAT4		m_pxmf4WallPlanes[6];

	virtual void Animate(float fElapsedTime);
};
