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
	m_pGameObjectManager = new CGameObjectManager;
}

void CScene::ReleaseObjects()
{
	delete m_pGameObjectManager;
}

void CScene::Animate(float fElapsedTime)
{
	m_pPlayer->Animate(fElapsedTime);

	auto Objectlist = m_pGameObjectManager->GetplGameObjects();
	for (const std::shared_ptr<CGameObject> & object : Objectlist)
		object->Animate(fElapsedTime);
}

void CScene::Render(HDC hDCFrameBuffer, CCamera * pCamera)
{
	m_pPlayer->Render(hDCFrameBuffer, pCamera);

	auto Objectlist = m_pGameObjectManager->GetplGameObjects();
	for (const std::shared_ptr<CGameObject> & object : Objectlist)
		object->Render(hDCFrameBuffer, pCamera);
}

CNomalStage::CNomalStage()
{
}

CNomalStage::~CNomalStage()
{
}

void CNomalStage::BuildObjects()
{
	CScene::BuildObjects();

	CExplosiveObject::PrepareExplosion();

	//벽설정
	float fHalfWidth = 45.0f, fHalfHeight = 45.0f, fHalfDepth = 500.0f;									//1.0f 당 1m
	CWallMesh *pWallCubeMesh = new CWallMesh(fHalfWidth * 2.0f, fHalfHeight * 2.0f, fHalfDepth * 2.0f, 20);
	pWallCubeMesh->SetOOBB(XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(fHalfWidth, fHalfHeight, fHalfDepth * 0.1f), XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f));

	m_pWallsObject = new CWallsObject();
	m_pWallsObject->SetPosition(0.0f, 0.0f, 0.0f);
	m_pWallsObject->SetMesh(pWallCubeMesh);
	m_pWallsObject->SetColor(RGB(0, 0, 0));
	m_pWallsObject->m_pxmf4WallPlanes[0] = XMFLOAT4(+1.0f, 0.0f, 0.0f, fHalfWidth);
	m_pWallsObject->m_pxmf4WallPlanes[1] = XMFLOAT4(-1.0f, 0.0f, 0.0f, fHalfWidth);
	m_pWallsObject->m_pxmf4WallPlanes[2] = XMFLOAT4(0.0f, +1.0f, 0.0f, fHalfHeight);
	m_pWallsObject->m_pxmf4WallPlanes[3] = XMFLOAT4(0.0f, -1.0f, 0.0f, fHalfHeight);
	m_pWallsObject->m_pxmf4WallPlanes[4] = XMFLOAT4(0.0f, 0.0f, +1.0f, 10);
	m_pWallsObject->m_pxmf4WallPlanes[5] = XMFLOAT4(0.0f, 0.0f, -1.0f, 10);

}

void CNomalStage::ReleaseObjects()
{
	if(m_pWallsObject) delete m_pWallsObject;
	if (m_pPlayer) delete m_pPlayer;
	if(m_pGameObjectManager) delete m_pGameObjectManager;
}

void CNomalStage::Animate(float fElapsedTime)
{
	ResponEnermy(fElapsedTime);

	for (std::shared_ptr<CEnermy> & pEnermy : m_pGameObjectManager->GetplEnermys())
		pEnermy->TraceObject(m_pPlayer);


	m_pWallsObject->Animate(fElapsedTime);
	m_pPlayer->Animate(fElapsedTime);

	CheckPlayerByWallCollision(fElapsedTime);

	auto ObejctpList = m_pGameObjectManager->GetplGameObjects();
	for (std::shared_ptr<CGameObject> & pObject : ObejctpList)
		pObject->Animate(fElapsedTime);

}

void CNomalStage::Render(HDC hDCFrameBuffer, CCamera * pCamera)
{
	CScene::Render(hDCFrameBuffer, pCamera);
	
	m_pWallsObject->Render(hDCFrameBuffer, pCamera);
}

void CNomalStage::CheckPlayerByWallCollision(float fElapsedTime)
{
	ContainmentType containType = m_pWallsObject->m_xmOOBB.Contains(m_pPlayer->m_xmOOBB);					//벽으로 충돌을 체크
	switch (containType)
	{
	case CONTAINS:					//포함될 경우
		break;

	case DISJOINT:			//만나지 않을 경우
	{
		int nPlaneIndex = -1;
		for (int j = 0; j < 6; j++)
		{
			PlaneIntersectionType intersectType = m_pPlayer->m_xmOOBB.Intersects(XMLoadFloat4(&m_pWallsObject->m_pxmf4WallPlanes[j]));
			if (intersectType == INTERSECTING)
			{
				nPlaneIndex = j;
				break;
			}
		}
		for (int j = 0; j < 6; j++)
		{
			PlaneIntersectionType intersectType = m_pPlayer->m_xmOOBB.Intersects(XMLoadFloat4(&m_pWallsObject->m_pxmf4WallPlanes[j]));
			if (intersectType == BACK)
			{
				nPlaneIndex = j;
				break;
			}
			if (nPlaneIndex < 0)
				break;

			if (nPlaneIndex < 4) {
				XMVECTOR moveDirection = -XMLoadFloat3(&m_pPlayer->m_xmf3MovingDirection);
				XMStoreFloat3(&m_pPlayer->m_xmf3MovingDirection, moveDirection);

				m_pPlayer->Move(m_pPlayer->m_xmf3MovingDirection, false);
			}

			if (nPlaneIndex < 6)					//충돌할시
			{
				m_pWallsObject->SetPosition(0.0f, 0.0f, m_pPlayer->GetPosition().z);
				break;
			}
		}
		break;
	}
	case INTERSECTS:						//만날경우?
	{
		int nPlaneIndex = -1;
		for (int j = 0; j < 6; j++)
		{
			PlaneIntersectionType intersectType = m_pPlayer->m_xmOOBB.Intersects(XMLoadFloat4(&m_pWallsObject->m_pxmf4WallPlanes[j]));
			if (intersectType == INTERSECTING)
			{
				nPlaneIndex = j;
				break;
			}
		}
		for (int j = 0; j < 6; j++)
		{
			PlaneIntersectionType intersectType = m_pPlayer->m_xmOOBB.Intersects(XMLoadFloat4(&m_pWallsObject->m_pxmf4WallPlanes[j]));
			if (intersectType == BACK)
			{
				nPlaneIndex = j;
				break;
			}
			if (nPlaneIndex < 0)
				break;

			if (nPlaneIndex < 4) {
				XMVECTOR moveDirection = -XMLoadFloat3(&m_pPlayer->m_xmf3MovingDirection);
				XMStoreFloat3(&m_pPlayer->m_xmf3MovingDirection, moveDirection);

				m_pPlayer->Move(m_pPlayer->m_xmf3MovingDirection, true);
			}

			if (nPlaneIndex < 6)					//충돌할시
			{
				m_pWallsObject->SetPosition(0.0f, 0.0f, m_pPlayer->GetPosition().z);
				break;
			}
		}
		break;
	}

	}




}

void CNomalStage::ResponEnermy(float fElapsedTime)
{
	CExplosiveObject::PrepareExplosion();
	CCubeMesh *pObjectCubeMesh = new CCubeMesh(2.0f, 2.0f, 2.0f);
	pObjectCubeMesh->SetOOBB(XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(2.0f, 2.0f, 2.0f), XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f));

	std::random_device rd;
	std::default_random_engine dre(rd());
	std::uniform_real_distribution<float> ufr(-WALL_HALF_SIZE, WALL_HALF_SIZE);
	std::uniform_real_distribution<float> ufrRotaionAngle(0.0f, 360.0f);

	m_fBonusObjectResponTime -= fElapsedTime;

	if (m_fBonusObjectResponTime < 0) {
		m_fBonusObjectResponTime = m_fBonusObjectInitResponTime;
		CBonusObject * BonusObject = new CBonusObject;
		BonusObject->m_bActive = true;
		BonusObject->SetMesh(pObjectCubeMesh);
		BonusObject->SetColor(RGB(225, 35, 35));
		BonusObject->SetPosition(XMFLOAT3(ufr(dre), ufr(dre), m_pPlayer->GetPosition().z + 145 + ufr(dre)));
		BonusObject->SetMovingSpeed(BONUSOBJECTSPEED);
		BonusObject->SetRotationAxis(XMFLOAT3(ufrRotaionAngle(dre), ufrRotaionAngle(dre), ufrRotaionAngle(dre)));
		BonusObject->SetRotationSpeed(120);

		m_pGameObjectManager->newBounusObject(BonusObject);
		
		//delete BonusObject;
	}


	m_fEnermyResponTime -= fElapsedTime;

	if (m_fEnermyResponTime  < 0) {
		m_fEnermyResponTime = m_fEnermyResponInitTime;
		CEnermy * Enermy = new CEnermy;
		Enermy->m_bActive = true;
		Enermy->SetMesh(pObjectCubeMesh);
		Enermy->SetColor(RGB(35, 120, 35));
		Enermy->SetPosition(XMFLOAT3(ufr(dre), ufr(dre), m_pPlayer->GetPosition().z + 145 + ufr(dre)));
		Enermy->SetMovingSpeed(ENERMYSPEED);
		Enermy->SetRotationAxis(XMFLOAT3(ufrRotaionAngle(dre), ufrRotaionAngle(dre), ufrRotaionAngle(dre)));
		Enermy->SetRotationSpeed(120);

		m_pGameObjectManager->newEnermy(Enermy);

		//delete Enermy;
	}

}


