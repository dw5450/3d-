//-----------------------------------------------------------------------------
// File: CGameObject.cpp
//-----------------------------------------------------------------------------

#include "stdafx.h"
#include "Object.h"
#include "Shader.h"

CGameObject::CGameObject()
{
	m_xmf4x4World = Matrix4x4::Identity();
}

CGameObject::~CGameObject()
{
	if (m_pMesh) m_pMesh->Release();
	if (m_pShader) m_pShader->Release();
}

void CGameObject::SetMesh(CMesh *pMesh)
{
	if (m_pMesh) m_pMesh->Release();
	m_pMesh = pMesh;
	if (m_pMesh) m_pMesh->AddRef();
}

void CGameObject::SetShader(CShader *pShader)
{
	if (m_pShader) m_pShader->Release();
	m_pShader = pShader;
	if (m_pShader) m_pShader->AddRef();
}

void CGameObject::Animate(float fTimeElapsed)
{
	m_fElapseTime += fTimeElapsed;
	if (m_bActive) {
		if (m_fRotationSpeed != 0.0f) Rotate(&m_xmf3RotationAxis, m_fRotationSpeed * fTimeElapsed);
		if (m_fMovingSpeed != 0.0f) Move(m_xmf3MovingDirection, m_fMovingSpeed * fTimeElapsed);

		if (m_pMesh)
		{
			m_pMesh->m_xmAABB.Transform(m_xmAABB, XMLoadFloat4x4(&m_xmf4x4World));
		}
	}
}

void CGameObject::Render(ID3D12GraphicsCommandList *pd3dCommandList, CCamera *pCamera)
{
	OnPrepareRender();

	if (m_pShader)
	{
		m_pShader->Render(pd3dCommandList, pCamera);
		m_pShader->UpdateShaderVariable(pd3dCommandList, &m_xmf4x4World);
		
	}

	if (m_pMesh) m_pMesh->Render(pd3dCommandList);
	//OnPostRender();
}

void CGameObject::ReleaseUploadBuffers()
{
	if (m_pMesh) m_pMesh->ReleaseUploadBuffers();
}

void CGameObject::SetPosition(float x, float y, float z)
{
	m_xmf4x4World._41 = x;
	m_xmf4x4World._42 = y;
	m_xmf4x4World._43 = z;
}

void CGameObject::SetPosition(XMFLOAT3 xmf3Position)
{
	SetPosition(xmf3Position.x, xmf3Position.y, xmf3Position.z);
}

XMFLOAT3 CGameObject::GetPosition()
{
	return(XMFLOAT3(m_xmf4x4World._41, m_xmf4x4World._42, m_xmf4x4World._43));
}

XMFLOAT3 CGameObject::GetLook()
{
	return(Vector3::Normalize(XMFLOAT3(m_xmf4x4World._31, m_xmf4x4World._32, m_xmf4x4World._33)));
}

XMFLOAT3 CGameObject::GetUp()
{
	return(Vector3::Normalize(XMFLOAT3(m_xmf4x4World._21, m_xmf4x4World._22, m_xmf4x4World._23)));
}

XMFLOAT3 CGameObject::GetRight()
{
	return(Vector3::Normalize(XMFLOAT3(m_xmf4x4World._11, m_xmf4x4World._12, m_xmf4x4World._13)));
}

void CGameObject::MoveStrafe(float fDistance)
{
	XMFLOAT3 xmf3Position = GetPosition();
	XMFLOAT3 xmf3Right = GetRight();
	xmf3Position = Vector3::Add(xmf3Position, xmf3Right, fDistance);
	CGameObject::SetPosition(xmf3Position);
}

void CGameObject::MoveUp(float fDistance)
{
	XMFLOAT3 xmf3Position = GetPosition();
	XMFLOAT3 xmf3Up = GetUp();
	xmf3Position = Vector3::Add(xmf3Position, xmf3Up, fDistance);
	CGameObject::SetPosition(xmf3Position);
}

void CGameObject::MoveForward(float fDistance)
{
	XMFLOAT3 xmf3Position = GetPosition();
	XMFLOAT3 xmf3Look = GetLook();
	xmf3Position = Vector3::Add(xmf3Position, xmf3Look, fDistance);
	CGameObject::SetPosition(xmf3Position);
}

void CGameObject::Move(XMFLOAT3 & vDirection, float fSpeed)
{
	SetPosition(m_xmf4x4World._41 + vDirection.x * fSpeed, m_xmf4x4World._42 + vDirection.y * fSpeed, m_xmf4x4World._43 + vDirection.z * fSpeed);
}

void CGameObject::Rotate(float fPitch, float fYaw, float fRoll)
{
	XMMATRIX mtxRotate = XMMatrixRotationRollPitchYaw(XMConvertToRadians(fPitch), XMConvertToRadians(fYaw), XMConvertToRadians(fRoll));
	m_xmf4x4World = Matrix4x4::Multiply(mtxRotate, m_xmf4x4World);
}

void CGameObject::Rotate(XMFLOAT3 *pxmf3Axis, float fAngle)
{
	XMMATRIX mtxRotate = XMMatrixRotationAxis(XMLoadFloat3(pxmf3Axis), XMConvertToRadians(fAngle));
	m_xmf4x4World = Matrix4x4::Multiply(mtxRotate, m_xmf4x4World);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
inline float RandF(float fMin, float fMax)
{
	return(fMin + ((float)rand() / (float)RAND_MAX) * (fMax - fMin));
}

XMVECTOR RandomUnitVectorOnSphere()
{
	XMVECTOR xmvOne = XMVectorSet(1.0f, 1.0f, 1.0f, 1.0f);
	//XMVECTOR xmvZero = XMVectorZero();

	while (true)
	{
		XMVECTOR v = XMVectorSet(RandF(-1.0f, 1.0f), RandF(-1.0f, 1.0f), RandF(-1.0f, 1.0f), 0.0f);
		if (!XMVector3Greater(XMVector3LengthSq(v), xmvOne)) return(XMVector3Normalize(v));
	}
}



XMFLOAT3 CExplosiveObject::m_pxmf3SphereVectors[EXPLOSION_DEBRISES];
CMesh *CExplosiveObject::m_pExplosionMesh = NULL;

CExplosiveObject::CExplosiveObject()
{
}

CExplosiveObject::~CExplosiveObject()
{
}

void CExplosiveObject::PrepareExplosion(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList)
{
	for (int i = 0; i < EXPLOSION_DEBRISES; i++) XMStoreFloat3(&m_pxmf3SphereVectors[i], ::RandomUnitVectorOnSphere());

	m_pExplosionMesh = new CVariousColorsCubeMesh(pd3dDevice, pd3dCommandList, 0.5f, 0.5f, 0.5f);
}

void CExplosiveObject::Animate(float fElapsedTime)
{
	if (m_bActive) {
		if (m_bBlowingUp)
		{
			m_fElapsedTimes += fElapsedTime;
			if (m_fElapsedTimes <= m_fDuration)
			{
				XMFLOAT3 xmf3Position = GetPosition();
				for (int i = 0; i < EXPLOSION_DEBRISES; i += LOD)
				{
					m_pxmf4x4Transforms[i] = Matrix4x4::Identity();
					m_pxmf4x4Transforms[i]._41 = xmf3Position.x + m_pxmf3SphereVectors[i].x * m_fExplosionSpeed * m_fElapsedTimes;
					m_pxmf4x4Transforms[i]._42 = xmf3Position.y + m_pxmf3SphereVectors[i].y * m_fExplosionSpeed * m_fElapsedTimes;
					m_pxmf4x4Transforms[i]._43 = xmf3Position.z + m_pxmf3SphereVectors[i].z * m_fExplosionSpeed * m_fElapsedTimes;
					m_pxmf4x4Transforms[i] = Matrix4x4::Multiply(Matrix4x4::RotationAxis(m_pxmf3SphereVectors[i], m_fExplosionRotation * m_fElapsedTimes), m_pxmf4x4Transforms[i]);
				}
			}

			else m_bActive = false;
		}
		else
		{
			CGameObject::Animate(fElapsedTime);
		}
	}
}

void CExplosiveObject::Render(ID3D12GraphicsCommandList *pd3dCommandList, CCamera *pCamera)
{
	if (m_bActive) {
		if (m_bBlowingUp)
		{
			for (int i = 0; i < EXPLOSION_DEBRISES; i += LOD)
			{
				if (m_pExplosionMesh)
				{
					if (m_pShader)
					{
						m_pShader->Render(pd3dCommandList, pCamera);
						m_pShader->UpdateShaderVariable(pd3dCommandList, &m_pxmf4x4Transforms[i]);
					}

					if (m_pExplosionMesh) m_pExplosionMesh->Render(pd3dCommandList);
				}
			}
		}
		else
		{
			CGameObject::Render(pd3dCommandList, pCamera);
		}
	}
}

void CEnermy::SetByType(size_t Type)
{
	m_iType = Type;
	switch (m_iType)
	{
	case Red:
		SetMovingSpeed(50);
		break;

	case Blue:
		SetMovingSpeed(10);
		break;

	case Green:
		SetMovingSpeed(20);
		break;

	case Pink:
		SetMovingSpeed(30);
		break;

	default:
		break;
	}

}

void CEnermy::TraceObject(CGameObject * pObejct)
{
	XMVECTOR movingDirection = XMVector3Normalize(XMVectorSubtract(XMLoadFloat3(&pObejct->GetPosition()), XMLoadFloat3(&GetPosition())));
	XMStoreFloat3(&m_xmf3MovingDirection, movingDirection);
}

CWallsObject::CWallsObject()
{
}

CWallsObject::~CWallsObject()
{
}

void CWallsObject::Animate(float fElapsedTime)
{
	if (m_pMesh && m_bActive)
	{
		m_pMesh->m_xmAABB.Transform(m_xmAABB, XMLoadFloat4x4(&m_xmf4x4World));
	}
	//CGameObject::Animate(fElapsedTime);
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////
CBullet::CBullet()
{

}

CBullet::~CBullet()
{

}

void CBullet::Animate(float fElapsedTime)
{
	if (m_bActive) {
		if (m_fShootingRange < 0)
			m_bActive = false;

		CGameObject::Animate(fElapsedTime);
		m_fShootingRange -= fElapsedTime * m_fMovingSpeed;
	}
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void CBoss::Animate(float fElapseTime)
{
	if (m_bActive) {
		CExplosiveObject::Animate(fElapseTime);
		m_fBulletCooltime -= fElapseTime;

		if (m_iLife == 0)
			m_bBlowingUp = true;

	}
}

void CBoss::Render(ID3D12GraphicsCommandList * pd3dCommandList, CCamera * pCamera)
{
	if (m_bActive) {
		if (m_bBlowingUp)
		{
			for (int i = 0; i < 100 ; i++)
			{
				if (m_pExplosionMesh)
				{
					if (m_pShader)
					{
						m_pShader->Render(pd3dCommandList, pCamera);
						m_pShader->UpdateShaderVariable(pd3dCommandList, &m_pxmf4x4Transforms[i]);
					}

					if (m_pExplosionMesh) m_pExplosionMesh->Render(pd3dCommandList);
				}
			}
		}
		else
		{
			CGameObject::Render(pd3dCommandList, pCamera);
		}
	}
}

void CBoss::SetExplosionMesh(CMesh * agrMesh)
{
	m_pExplosionMesh = agrMesh;
}

void CBoss::SetBulletInfo(CMesh * agrMesh, CShader * pShader)
{
	m_pBulletMesh = agrMesh;
	m_pBulletShader = pShader;
}

bool CBoss::CanShot()
{
	if (m_fBulletCooltime < 0) {
		m_fBulletCooltime = m_fBulletInitCooltime;
		m_bShotBullet = false;
		return true;
	}

	return false;
}

CBullet * CBoss::ShotBullet()
{
	//CCubeMesh *pObjectCubeMesh = new CCubeMesh(1.0f, 1.0f, 1.0f);
	CBullet * pBullet = new CBullet;
	if(m_pBulletMesh) pBullet->SetMesh(m_pBulletMesh);
	if (m_pBulletShader) pBullet->SetShader(m_pBulletShader);
	pBullet->SetMovingSpeed(60);
	pBullet->SetRotationSpeed(600.0f);
	pBullet->SetPosition(Vector3::Add(GetPosition(), m_xmf3MovingDirection, 12.1f));
	pBullet->SetMovingDirection(m_xmf3MovingDirection);
	pBullet->SetRotationAxis(m_xmf3MovingDirection);
	return pBullet;

}