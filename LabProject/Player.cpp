#include "stdafx.h"
#include "Player.h"

CPlayer::CPlayer()
{
	m_pCamera = new CCamera();
	m_pCamera->GenerateProjectionMatrix(1.01f, 1000.0f, 45.0f);
	m_pCamera->SetViewport(0, 0, CLIENT_WIDTH, CLIENT_HEIGHT);


	m_xmf3Position = XMFLOAT3(0.0f, 0.0f, 0.0f);
	CGameObject::SetPosition(m_xmf3Position);
	m_xmf3Right = XMFLOAT3(1.0f, 0.0f, 0.0f);
	m_xmf3Up = XMFLOAT3(0.0f, 1.0f, 0.0f);
	m_xmf3Look = XMFLOAT3(0.0f, 0.0f, 1.0f);

	m_xmf3CameraOffset = XMFLOAT3(0.0f, 0.0f, 0.0f);
	m_xmf3Velocity = XMFLOAT3(0.0f, 0.0f, 0.0f);


}

CPlayer::~CPlayer()
{
	if (m_pCamera) delete m_pCamera;
}

void CPlayer::SetPosition(float x, float y, float z)
{
	m_xmf3Position = XMFLOAT3(x, y, z);

	CGameObject::SetPosition(x, y, z);
}

void CPlayer::SetCameraOffset(XMFLOAT3& xmf3CameraOffset)
{
	m_xmf3CameraOffset = xmf3CameraOffset;
	m_pCamera->SetLookAt(Vector3::Add(m_xmf3Position, m_xmf3CameraOffset), m_xmf3Position, m_xmf3Up);
	m_pCamera->GenerateViewMatrix();
}


void CPlayer::Move(DWORD dwDirection, float elapse_time)
{
	UINT orderCnt = 0;
	float fDistance = m_fMovingSpeed * elapse_time;
	if (dwDirection)
	{
		if (dwDirection & DIR_FORWARD) ++orderCnt;
		if (dwDirection & DIR_BACKWARD)  ++orderCnt;
		if (dwDirection & DIR_RIGHT)  ++orderCnt;
		if (dwDirection & DIR_LEFT)  ++orderCnt;


		XMFLOAT3 xmf3Shift = XMFLOAT3(0, 0, 0);
		fDistance = fDistance / orderCnt;
		if (dwDirection & DIR_FORWARD) xmf3Shift = Vector3::Add(xmf3Shift, m_xmf3Look, fDistance);
		if (dwDirection & DIR_BACKWARD) xmf3Shift = Vector3::Add(xmf3Shift, m_xmf3Look, -fDistance);
		if (dwDirection & DIR_RIGHT) xmf3Shift = Vector3::Add(xmf3Shift, m_xmf3Right, fDistance);
		if (dwDirection & DIR_LEFT) xmf3Shift = Vector3::Add(xmf3Shift, m_xmf3Right, -fDistance);
		if (dwDirection & DIR_UP) xmf3Shift = Vector3::Add(xmf3Shift, m_xmf3Up, fDistance);
		if (dwDirection & DIR_DOWN) xmf3Shift = Vector3::Add(xmf3Shift, m_xmf3Up, -fDistance);

		Move(xmf3Shift, true);

	}
}

void CPlayer::Move(XMFLOAT3& xmf3Shift, bool bUpdateVelocity)
{

	if (bUpdateVelocity)
	{
		m_xmf3Velocity = Vector3::Add(m_xmf3Velocity, xmf3Shift);
	}
	else 
	{
		m_xmf3Position = Vector3::Add(xmf3Shift, m_xmf3Position);
		CGameObject::SetPosition(m_xmf3Position);
		m_xmf3MovingDirection = Vector3::Normalize(xmf3Shift);
		m_pCamera->Move(xmf3Shift);
	}
}

void CPlayer::Move(float x, float y, float z)
{
	Move(XMFLOAT3(x, y, z), false);
}

void CPlayer::Rotate(float fPitch, float fYaw, float fRoll)
{
	m_pCamera->Rotate(fPitch, fYaw, fRoll);
	if (fPitch != 0.0f)
	{
		XMMATRIX mtxRotate = XMMatrixRotationAxis(XMLoadFloat3(&m_xmf3Right), XMConvertToRadians(fPitch));
		m_xmf3Look = Vector3::TransformNormal(m_xmf3Look, mtxRotate);
		m_xmf3Up = Vector3::TransformNormal(m_xmf3Up, mtxRotate);
	}
	if (fYaw != 0.0f)
	{
		XMMATRIX mtxRotate = XMMatrixRotationAxis(XMLoadFloat3(&m_xmf3Up), XMConvertToRadians(fYaw));
		m_xmf3Look = Vector3::TransformNormal(m_xmf3Look, mtxRotate);
		m_xmf3Right = Vector3::TransformNormal(m_xmf3Right, mtxRotate);
	}
	if (fRoll != 0.0f)
	{
		XMMATRIX mtxRotate = XMMatrixRotationAxis(XMLoadFloat3(&m_xmf3Look), XMConvertToRadians(fRoll));
		m_xmf3Up = Vector3::TransformNormal(m_xmf3Up, mtxRotate);
		m_xmf3Right = Vector3::TransformNormal(m_xmf3Right, mtxRotate);
	}

	m_xmf3Look = Vector3::Normalize(m_xmf3Look);
	m_xmf3Right = Vector3::Normalize(Vector3::CrossProduct(m_xmf3Up, m_xmf3Look));
	m_xmf3Up = Vector3::Normalize(Vector3::CrossProduct(m_xmf3Look, m_xmf3Right));
}

void CPlayer::Update(float fTimeElapsed)
{
	Move(m_xmf3Velocity, false);

	m_pCamera->Update(this, m_xmf3Position, fTimeElapsed);
	m_pCamera->GenerateViewMatrix();

	XMFLOAT3 xmf3Deceleration = Vector3::Normalize(Vector3::ScalarProduct(m_xmf3Velocity, -1.0f));
	float fLength = Vector3::Length(m_xmf3Velocity);
	float fDeceleration = m_fFriction * fTimeElapsed;
	if (fDeceleration > fLength) fDeceleration = fLength;
	m_xmf3Velocity = Vector3::Add(m_xmf3Velocity, xmf3Deceleration, fDeceleration);

}
void CPlayer::Animate(float fElapsedTime)
{
	ReloadBullet(fElapsedTime);
	m_fBulletCooltime -= fElapsedTime;
	m_fBombCooltime -= fElapsedTime;

	TracingEnermy();

	if (m_pMesh)
	{
		m_pMesh->m_xmOOBB.Transform(m_xmOOBB, XMLoadFloat4x4(&m_xmf4x4World));
		XMStoreFloat4(&m_xmOOBB.Orientation, XMQuaternionNormalize(XMLoadFloat4(&m_xmOOBB.Orientation)));
	}
}

void CPlayer::Render(HDC hDCFrameBuffer, CCamera *pCamera)
{
	m_xmf4x4World._11 = m_xmf3Right.x; m_xmf4x4World._12 = m_xmf3Right.y; m_xmf4x4World._13 = m_xmf3Right.z;
	m_xmf4x4World._21 = m_xmf3Up.x; m_xmf4x4World._22 = m_xmf3Up.y; m_xmf4x4World._23 = m_xmf3Up.z;
	m_xmf4x4World._31 = m_xmf3Look.x; m_xmf4x4World._32 = m_xmf3Look.y; m_xmf4x4World._33 = m_xmf3Look.z;
	m_xmf4x4World._41 = m_xmf3Position.x; m_xmf4x4World._42 = m_xmf3Position.y; m_xmf4x4World._43 = m_xmf3Position.z;

	m_xmf4x4World = Matrix4x4::Multiply(XMMatrixRotationRollPitchYaw(XMConvertToRadians(90.0f), 0.0f, 0.0f), m_xmf4x4World);
	
	CGameObject::Render(hDCFrameBuffer, pCamera);
}

bool CPlayer::CanShot()
{
	if (m_bShotBullet && m_iBulletNum > 0 && m_fBulletCooltime < 0) {
		m_bShotBullet = false;
		return true;
	}

	return false;
}

CBullet * CPlayer::ShotBullet()
{
	m_bShotBullet = false;
	--m_iBulletNum;
	CCubeMesh *pObjectCubeMesh = new CCubeMesh(1.0f, 1.0f, 1.0f);
	CBullet * pBullet = new CBullet;
	pBullet->SetMesh(pObjectCubeMesh);
	pBullet->SetColor(RGB(255, 0, 0));
	pBullet->SetMovingSpeed(BULLETSPEED);
	pBullet->SetRotationSpeed(600.0f);
	pBullet->SetPosition(m_xmf3Position);
	pBullet->MoveForward(4);
	pBullet->SetMovingDirection(m_xmf3Look);
	pBullet->SetRotationAxis(m_xmf3Look);
	m_fBulletCooltime = m_fBulletInitCooltime;
	return pBullet;
}

void CPlayer::ReloadBullet(float fElapseTime)
{
	if (m_bReload) {
		m_bReloadTime -= fElapseTime;
		if (m_bReloadTime < 0) {
			m_bReload = false;
			m_iBulletNum = m_iBulletMaxNum;
			m_bReloadTime = m_bReloadInitTime;
		}
	}
}

bool CPlayer::ShotBomb()
{
	bool isTrue = false;
	if (m_iBombNum > 0 && m_bShotBomb && m_fBombCooltime < 0) {
		--m_iBombNum;
		isTrue = true;
		m_fBombCooltime = m_fBombInitCooltime;
	}
	m_bShotBomb = false;

	return isTrue;

}

void CPlayer::PickingEnermy(const std::list<std::shared_ptr<CEnermy>>& plEnermy)
{
	XMVECTOR vecPosition = XMLoadFloat3(&m_pCamera->m_xmf3Position);
	XMFLOAT4X4 CanraWorldMatrix;

	CanraWorldMatrix._11 = m_pCamera->m_xmf3Right.x;	CanraWorldMatrix._12 = m_pCamera->m_xmf3Right.y; CanraWorldMatrix._13 = m_pCamera->m_xmf3Right.z;
	CanraWorldMatrix._21 = m_pCamera->m_xmf3Up.x;		CanraWorldMatrix._22 = m_pCamera->m_xmf3Up.y;	 CanraWorldMatrix._23 = m_pCamera->m_xmf3Up.z;
	CanraWorldMatrix._31 = m_pCamera->m_xmf3Look.x;		CanraWorldMatrix._32 = m_pCamera->m_xmf3Look.y;	 CanraWorldMatrix._33 = m_pCamera->m_xmf3Look.z;
	CanraWorldMatrix._41 = 0;
	CanraWorldMatrix._42 = 0;
	CanraWorldMatrix._43 = 0;
	XMFLOAT3 Result;
	Result = Vector3::TransformNormal(m_xmf3PickRay, CanraWorldMatrix);
	XMVECTOR vecPickingRay = XMLoadFloat3(&Result);

	float distance = 10000.0f;
	float nearPickedZ = 10000.f;
	for (const std::shared_ptr<CEnermy> & data : plEnermy) 
		if (data->m_xmOOBB.Intersects(vecPosition, vecPickingRay, distance) && data->GetPosition().z <nearPickedZ) {
			m_pTracingObject = data;
			m_bTraceEnermy = true;
		}

}

void CPlayer::TracingEnermy()
{
	static int num = 0;
	if (m_bTraceEnermy) {
		XMFLOAT3 EndLook = Vector3::Normalize(Vector3::Subtract(m_pTracingObject->GetPosition(), m_xmf3Position));
		XMFLOAT3 RotationDirection = Vector3::Normalize(Vector3::Subtract(EndLook, m_xmf3Look));
		XMFLOAT3 RotaitionAdd;
		RotaitionAdd = Vector3::ScalarProduct(RotationDirection, 0.01f);

		if (EndLook.x > m_xmf3Look.x && m_xmf3Look.x + RotaitionAdd.x > EndLook.x) {
			m_xmf3Look = EndLook;
		}
		else if(EndLook.x < m_xmf3Look.x && m_xmf3Look.x + RotaitionAdd.x < EndLook.x) {
			m_xmf3Look = EndLook;
		}

		else m_xmf3Look = Vector3::Add(m_xmf3Look, RotaitionAdd);
		m_xmf3Right = Vector3::CrossProduct(m_xmf3Look, XMFLOAT3(0, -1, 0));
		m_xmf3Up = Vector3::CrossProduct(m_xmf3Look, m_xmf3Right);
	

	}
}





////////////////////////////////////////////////////////////////////////////////////////////