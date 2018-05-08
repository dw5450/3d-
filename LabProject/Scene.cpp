#include "stdafx.h"
#include "Scene.h"
//장면을 그립니다
//오브젝트들을 생성, 위치 시킵니다.
//충돌체크를 실행합니다.

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
	
}

void CScene::ReleaseObjects()
{

}

void CScene::Animate(float fElapsedTime)
{
	m_pPlayer->Animate(fElapsedTime);
}

void CScene::Render(HDC hDCFrameBuffer, CCamera * pCamera)
{
	m_pPlayer->Render(hDCFrameBuffer, pCamera);

}


CNomalStage::CNomalStage()
{
}

CNomalStage::~CNomalStage()
{
	if (m_pBoss) delete m_pBoss;
}

void CNomalStage::BuildObjects()
{
	CScene::BuildObjects();

	CExplosiveObject::PrepareExplosion();

	//벽설정
	float fHalfWidth = 45.0f, fHalfHeight = 45.0f, fHalfDepth = WALL_HALF_DEPTH;									//1.0f 당 1m
	CWallMesh *pWallCubeMesh = new CWallMesh(fHalfWidth * 2.0f, fHalfHeight * 2.0f, fHalfDepth * 2.0f, 20);
	pWallCubeMesh->SetOOBB(XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(fHalfWidth, fHalfHeight, fHalfDepth), XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f));

	m_pWallsObject = new CWallsObject();
	
	m_pWallsObject->SetPosition(0, 0, WALL_HALF_DEPTH);
	m_pWallsObject->SetMovingDirection(XMFLOAT3(0, 0, 0));
	m_pWallsObject->m_fMovingSpeed = 0.0f;
	m_pWallsObject->SetMesh(pWallCubeMesh);
	m_pWallsObject->SetColor(RGB(0, 0, 0));
	m_pWallsObject->m_xmf3MovingDirection = XMFLOAT3(0, 0, 0);m_pWallsObject->m_pxmf4WallPlanes[0] = XMFLOAT4(+1.0f, 0.0f, 0.0f, fHalfWidth);
	m_pWallsObject->m_pxmf4WallPlanes[1] = XMFLOAT4(-1.0f, 0.0f, 0.0f, fHalfWidth);
	m_pWallsObject->m_pxmf4WallPlanes[2] = XMFLOAT4(0.0f, +1.0f, 0.0f, fHalfHeight);
	m_pWallsObject->m_pxmf4WallPlanes[3] = XMFLOAT4(0.0f, -1.0f, 0.0f, fHalfHeight);
	m_pWallsObject->m_pxmf4WallPlanes[4] = XMFLOAT4(0.0f, 0.0f, +1.0f, fHalfDepth);
	m_pWallsObject->m_pxmf4WallPlanes[5] = XMFLOAT4(0.0f, 0.0f, -1.0f, fHalfDepth);

}

void CNomalStage::ReleaseObjects()
{
	if(m_pWallsObject) delete m_pWallsObject;
	if (m_pPlayer) delete m_pPlayer;
}

void CNomalStage::Animate(float fElapsedTime)
{
	ResponObject(fElapsedTime);

	if (m_pPlayer->CanShot())
		m_plBullets.emplace_back(m_pPlayer->ShotBullet());


	m_pWallsObject->Animate(fElapsedTime);
	m_pPlayer->Animate(fElapsedTime);

	if (m_pBoss) {
		m_pBoss->Animate(fElapsedTime);
		m_pBoss->TraceObject(m_pPlayer);
		if (m_pBoss->CanShot())	m_plBullets.emplace_back(m_pBoss->ShotBullet());
	}
	

	for (std::shared_ptr<CBullet> & data : m_plBullets)
		data->Animate(fElapsedTime);

for (std::shared_ptr<CEnermy> & data : m_plEnermys) {
	data->TraceObject(m_pPlayer);
	data->Animate(fElapsedTime);
}

for (std::shared_ptr<CBonusObject> & data : m_plBonusObjects)
data->Animate(fElapsedTime);

if (m_pPlayer->ShotBomb()) {
	for (std::shared_ptr<CEnermy> & data : m_plEnermys) {
		data->m_bBlowingUp = true;
	}
}

CheckPlayerByWallCollision(fElapsedTime);
CheckBulletByWallCollision();
CheckBnousObjectByWallCollision();
CheckEnermyByBulletCollisions();
CheckBonusObjectBulletCollisions();
CheckPlayerByEnermyCollisions();
CheckPlayerByBulletCollisions();

RemoveEnermy();
RemoveBonusObject();
RemoveBullet();


}

void CNomalStage::Render(HDC hDCFrameBuffer, CCamera * pCamera)
{
	m_pPlayer->Render(hDCFrameBuffer, pCamera);

	m_pWallsObject->Render(hDCFrameBuffer, pCamera);

	if (m_pBoss)
		m_pBoss->Render(hDCFrameBuffer, pCamera);

	for (std::shared_ptr<CBullet> & data : m_plBullets)
		data->Render(hDCFrameBuffer, pCamera);

	for (std::shared_ptr<CEnermy> & data : m_plEnermys)
		data->Render(hDCFrameBuffer, pCamera);

	for (std::shared_ptr<CBonusObject> & data : m_plBonusObjects)
		data->Render(hDCFrameBuffer, pCamera);
}

void CNomalStage::CheckPlayerByWallCollision(float fElapseTime)
{
	ContainmentType containType = m_pWallsObject->m_xmOOBB.Contains(m_pPlayer->m_xmOOBB);					//벽으로 충돌을 체크
	switch (containType)
	{
	case CONTAINS:					//포함될 경우
		break;

	case INTERSECTS:						//겹칠경우
	{
		int nPlaneIndex = -1;
		for (int j = 0; j < 6; j++)
		{
			PlaneIntersectionType intersectType = m_pPlayer->m_xmOOBB.Intersects(XMLoadFloat4(&m_pWallsObject->m_pxmf4WallPlanes[j]));
			if (intersectType == INTERSECTING)
			{
				if (j < 4) {
					XMVECTOR moveShift = -XMLoadFloat3(&m_pPlayer->m_xmf3MovingDirection);
					moveShift = moveShift * fElapseTime * m_pPlayer->m_fMovingSpeed;
					XMFLOAT3A result;
					XMStoreFloat3(&result, moveShift);

					m_pPlayer->Move(result, false);
				}
				break;
			}
		}
		break;
	}

	}
	if (m_pPlayer->GetPosition().z < 3) {
		m_pPlayer->m_xmf3Position.z = 3;
	}
	
	if (m_pPlayer->GetPosition().z > 997) {
		m_pPlayer->m_xmf3Position.z = 997;

	}

	if (WALL_HALF_DEPTH< m_pPlayer->GetPosition().z && m_pPlayer->GetPosition().z < 1000.0f - WALL_HALF_DEPTH) {
		if (m_pPlayer->GetPosition().z > m_pWallsObject->GetPosition().z) {
			if (m_pPlayer->m_xmf3MovingDirection.z > 0) {
				m_pWallsObject->SetPosition(XMFLOAT3(0, 0, m_pWallsObject->GetPosition().z + 25));
			}
		}
		else if (m_pPlayer->GetPosition().z < m_pWallsObject->GetPosition().z) {
			if (m_pPlayer->m_xmf3MovingDirection.z < 0) {
				m_pWallsObject->SetPosition(XMFLOAT3(0, 0, m_pWallsObject->GetPosition().z - 25));
			}
		}
	}

	if (m_pPlayer->GetPosition().z > 800 && !m_pBoss)
		ResponBoss();

}

void CNomalStage::CheckBulletByWallCollision()
{
	auto m_plBullets_itor = m_plBullets.begin();
	for (;m_plBullets_itor != m_plBullets.end();) {
		std::shared_ptr<CGameObject> bullet = *m_plBullets_itor;
		if (!bullet->m_xmOOBB.Intersects(m_pWallsObject->m_xmOOBB)) {
			m_plBullets_itor = m_plBullets.erase(m_plBullets_itor);
		}
		else ++m_plBullets_itor;
	}
}

void CNomalStage::CheckEnermyByBulletCollisions()
{
	for (std::shared_ptr<CEnermy> & Enermy : m_plEnermys) {
		auto m_plBullets_itor = m_plBullets.begin();
		for (;m_plBullets_itor != m_plBullets.end();) {
			std::shared_ptr<CGameObject> bullet = *m_plBullets_itor;
			if (bullet->m_xmOOBB.Intersects(Enermy->m_xmOOBB) && !Enermy->m_bBlowingUp){
				Enermy->m_bBlowingUp = true;
				m_plBullets_itor = m_plBullets.erase(m_plBullets_itor);
			}
			else ++m_plBullets_itor;
		}
	}
}

void CNomalStage::CheckBonusObjectBulletCollisions()
{
	for (std::shared_ptr<CBonusObject> & pBonusObject : m_plBonusObjects) {
		auto m_plBullets_itor = m_plBullets.begin();
		for (;m_plBullets_itor != m_plBullets.end();) {
			std::shared_ptr<CGameObject> bullet = *m_plBullets_itor;
			if (bullet->m_xmOOBB.Intersects(pBonusObject->m_xmOOBB) && !pBonusObject->m_bBlowingUp) {
				if(!pBonusObject->m_bBlowingUp)
					++m_pPlayer->m_iBombNum;
				pBonusObject->m_bBlowingUp = true;
				m_plBullets_itor = m_plBullets.erase(m_plBullets_itor);
			}
			else ++m_plBullets_itor;
		}
	}
}

void CNomalStage::CheckPlayerByEnermyCollisions()
{
	bool isEnermyBackPlayer = false;
	auto itor = m_plEnermys.begin();

	for (;itor != m_plEnermys.end();)
	{
		std::shared_ptr<CEnermy> & pEnermy = *itor;
		if(pEnermy->GetPosition().z < m_pPlayer->GetPosition().z)
			isEnermyBackPlayer = true;

		if (m_pPlayer->m_xmOOBB.Intersects(pEnermy->m_xmOOBB)) {
			if (!pEnermy->m_bBlowingUp) {
				--m_pPlayer->m_iLife;
				itor = m_plEnermys.erase(itor);
			}
			else ++itor;
		}
		else ++itor;
	}

	if (isEnermyBackPlayer)
		m_pPlayer->SetColor(RGB(255, 0, 0));
	//else
		//m_pPlayer->SetColor(RGB(0, 0, 255));
}

void CNomalStage::CheckPlayerByBulletCollisions()
{
	auto itor = m_plBullets.begin();
	for (;itor != m_plBullets.end();)
	{
		std::shared_ptr<CBullet> & pBullet = *itor;

		if (m_pPlayer->m_xmOOBB.Intersects(pBullet->m_xmOOBB)) {
			--m_pPlayer->m_iLife;
			itor = m_plBullets.erase(itor);
		}
		else ++itor;
	}
}

XMFLOAT3 CNomalStage::GetPickRay(float xScreen, float yScreen)
{
	CCamera * pCamera = m_pPlayer->m_pCamera;
	float Left = (float)pCamera->m_Viewport.m_xStart;
	float Top = (float)pCamera->m_Viewport.m_yStart;
	XMFLOAT3 MouseFarPosition(0, 0, 0);
	float Width = (float)pCamera->m_Viewport.m_nWidth;
	float Height = (float)pCamera->m_Viewport.m_nHeight;
	////투영 좌표계로 
	MouseFarPosition.x = (float)(((2.0f * (xScreen - Left)) / Width) - 1.03);
	MouseFarPosition.y = (float)((-(2.0f * (yScreen - Top)) / Height) + 1.12);
	XMFLOAT4X4 InvProj(Matrix4x4::Inverse(pCamera->m_xmf4x4Projection));
	MouseFarPosition = Vector3::TransformCoord(MouseFarPosition, InvProj);
	MouseFarPosition.z = 1.0f;
	return Vector3::Normalize( MouseFarPosition);
}

void CNomalStage::CheckBnousObjectByWallCollision()
{
	for (std::shared_ptr<CBonusObject> & pBonusObject : m_plBonusObjects) {
			if (!pBonusObject->m_xmOOBB.Intersects(m_pWallsObject->m_xmOOBB)) {
				pBonusObject->m_xmf3MovingDirection = Vector3::Subtract(XMFLOAT3(0, 0, 0), pBonusObject->m_xmf3MovingDirection);
			}
	}
}

void CNomalStage::ResponObject(float fElapsedTime)
{
	if (m_pPlayer->GetPosition().z > 855)
		return;
	CExplosiveObject::PrepareExplosion();
	CCubeMesh *pObjectCubeMesh = new CCubeMesh(2.0f, 2.0f, 2.0f);
	pObjectCubeMesh->SetOOBB(XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(2.0f, 2.0f, 2.0f), XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f));

	std::random_device rd;
	std::default_random_engine dre(rd());
	std::uniform_real_distribution<float> ufr(-WALL_HALF_SIZE, WALL_HALF_SIZE);
	std::uniform_real_distribution<float> ufrRotaionAngle(0.0f, 360.0f);

	m_fBonusObjectResponTime -= fElapsedTime;

	if (m_fBonusObjectResponTime < 0 ) {
		m_fBonusObjectResponTime = m_fBonusObjectInitResponTime;
		CBonusObject * BonusObject = new CBonusObject;
		BonusObject->m_bActive = true;
		BonusObject->SetMesh(pObjectCubeMesh);
		BonusObject->SetColor(RGB(225, 35, 35));
		BonusObject->SetPosition(XMFLOAT3(ufr(dre), ufr(dre), m_pPlayer->GetPosition().z + 100 + ufr(dre)));
		BonusObject->SetMovingDirection(XMFLOAT3(ufr(dre), ufr(dre), 0));
		BonusObject->SetMovingSpeed(BONUSOBJECTSPEED);
		BonusObject->SetRotationAxis(XMFLOAT3(ufrRotaionAngle(dre), ufrRotaionAngle(dre), ufrRotaionAngle(dre)));
		BonusObject->SetRotationSpeed(120);
		
		m_plBonusObjects.emplace_back(BonusObject);
	}


	m_fEnermyResponTime -= fElapsedTime;

	std::uniform_int_distribution<int> randEnermyuid(0, 1);
	UINT randEnermy = randEnermyuid(dre);
	if (m_fEnermyResponTime  < 0) {
		m_fEnermyResponTime = m_fEnermyResponInitTime;
		CEnermy * Enermy = new CEnermy;
		Enermy->m_bActive = true;
		Enermy->SetMesh(pObjectCubeMesh);
		Enermy->SetColor(RGB(235 * randEnermy, 0, 235));
		Enermy->SetPosition(XMFLOAT3(ufr(dre), ufr(dre), m_pPlayer->GetPosition().z + 145 + ufr(dre)));
		Enermy->SetMovingDirection(XMFLOAT3(0, 0, 0));
		Enermy->SetMovingSpeed(ENERMYSPEED + randEnermy* ENERMYSPEED);
		Enermy->SetRotationAxis(XMFLOAT3(ufrRotaionAngle(dre), ufrRotaionAngle(dre), ufrRotaionAngle(dre)));
		Enermy->SetRotationSpeed(120);


		m_plEnermys.emplace_back(Enermy);
		//delete Enermy;
	}

}

void CNomalStage::ResponBoss()
{
	CExplosiveObject::PrepareExplosion();
	CCubeMesh *pObjectCubeMesh = new CCubeMesh(8.0f, 8.0f, 8.0f);
	pObjectCubeMesh->SetOOBB(XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(8.0f, 8.0f, 8.0f), XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f));

	m_fBonusObjectResponTime = m_fBonusObjectInitResponTime;
	CBoss * BossObject = new CBoss;
	BossObject->m_bActive = true;
	BossObject->SetMesh(pObjectCubeMesh);
	BossObject->SetColor(RGB(180, 82, 162));
	BossObject->SetPosition(XMFLOAT3(0, 0, 990));
	BossObject->SetMovingDirection(XMFLOAT3(0,0, -1));
	BossObject->SetMovingSpeed(10);
	BossObject->SetRotationAxis(BossObject->GetLook());
	BossObject->SetRotationSpeed(120);

	m_pBoss = BossObject;
}

void CNomalStage::RemoveEnermy()
{
	RemoveExplosionObject(m_plEnermys)
}

void CNomalStage::RemoveBonusObject()
{
	RemoveExplosionObject(m_plBonusObjects)
}

void CNomalStage::RemoveBullet()
{
	auto itor = m_plBullets.begin();
	for (;itor != m_plBullets.end();)
	{
		std::shared_ptr<CBullet> pBullet = *itor;
		if (pBullet->ChecksShootingRange())
			itor = m_plBullets.erase(itor);
		
		else  ++itor;
	}
}

