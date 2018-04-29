#pragma once

#include "GameObject.h"
#include "Player.h"
#include "GameObjectManager.h"

class CScene
{
public:
	CScene();
	virtual ~CScene();

	CPlayer						*m_pPlayer = NULL;
	CGameObjectManager			*m_pGameObjectManager;


	virtual void BuildObjects();
	virtual void ReleaseObjects();

	virtual void Animate(float fElapsedTime);
	virtual void Render(HDC hDCFrameBuffer, CCamera *pCamera);

	virtual void OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);
	virtual void OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);


};



class CNomalStage : public CScene
{
public:
	CNomalStage();
	virtual ~CNomalStage();

	virtual void BuildObjects();
	virtual void ReleaseObjects();

	virtual void Animate(float fElapsedTime)
	{
		ResponEnermy(fElapsedTime);

		for (std::shared_ptr<CEnermy> & pEnermy : m_pGameObjectManager->GetplEnermys())
			pEnermy->TraceObject(m_pPlayer);


		m_pWallsObject->Animate(fElapsedTime);
		m_pPlayer->Animate(fElapsedTime);
		auto ObejctpList = m_pGameObjectManager->GetplGameObjects();
		for (std::shared_ptr<CGameObject> & pObject : ObejctpList)
			pObject->Animate(fElapsedTime);

	}
	virtual void Render(HDC hDCFrameBuffer, CCamera *pCamera);

public:
	CWallsObject		*m_pWallsObject = NULL;		
	float				m_fEnermyResponInitTime = ENERMYRESPONTIME;
	float				m_fEnermyResponTime = ENERMYRESPONTIME;
	float				m_fBonusObjectInitResponTime = BONOUSOBJECTRESPONTIME;
	float				m_fBonusObjectResponTime = BONOUSOBJECTRESPONTIME;

public:
	void CheckObjectByWallCollisions();
	void CheckObjectByBulletCollisions();

	void ResponEnermy(float fElapsedTime);
};
