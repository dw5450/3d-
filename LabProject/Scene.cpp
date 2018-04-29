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

void CNomalStage::Render(HDC hDCFrameBuffer, CCamera * pCamera)
{
	CScene::Render(hDCFrameBuffer, pCamera);
	
	m_pWallsObject->Render(hDCFrameBuffer, pCamera);
}

void CNomalStage::CheckObjectByWallCollisions()
{
}

void CNomalStage::ResponEnermy(float fElapsedTime)
{
	m_fBonusObjectResponTime -= fElapsedTime;
	if (m_fBonusObjectResponTime < 0) {
		m_fBonusObjectResponTime = m_fEnermyResponInitTime;
		CBonusObject * BonusObject = new CBonusObject;


		delete BonusObject;
	}


	m_fEnermyResponTime -= fElapsedTime;

}


