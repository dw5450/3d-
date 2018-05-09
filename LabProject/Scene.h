#pragma once

#include "GameObject.h"
#include "Player.h"


#define RemoveExplosionObject(pObjectList) \
	auto  Itor = pObjectList.begin(); \
	for (;Itor != pObjectList.end();) {\
	std::shared_ptr<CExplosiveObject> pExplosionObject = *Itor;\
	if (pExplosionObject->m_bBlowingUp && pExplosionObject->m_fElapsedTimes > pExplosionObject->m_fDuration)\
		Itor = pObjectList.erase(Itor);\
	else ++Itor;\
	}



class CScene
{
public:
	CScene();
	virtual ~CScene();

	CPlayer						*m_pPlayer = NULL;


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

	virtual void Animate(float fElapsedTime);
	virtual void Render(HDC hDCFrameBuffer, CCamera *pCamera);

public:
	CWallsObject		*m_pWallsObject = NULL;
	CBoss				*m_pBoss = NULL;
	std::list<std::shared_ptr<CBullet>>				m_plBullets;
	std::list<std::shared_ptr<CEnermy>>				m_plEnermys;
	std::list<std::shared_ptr<CBonusObject>>		m_plBonusObjects;
	
	float				m_fEnermyResponInitTime = ENERMYRESPONTIME;
	float				m_fEnermyResponTime = ENERMYRESPONTIME;
	float				m_fBonusObjectInitResponTime = BONOUSOBJECTRESPONTIME;
	float				m_fBonusObjectResponTime = BONOUSOBJECTRESPONTIME;

public:
	void CheckPlayerByWallCollision(float fElapseTime);
	void CheckBulletByWallCollision();
	void CheckBnousObjectByWallCollision();
	void CheckEnermyByBulletCollisions();
	void CheckBonusObjectBulletCollisions();
	void CheckPlayerByEnermyCollisions();
	void CheckPlayerByBulletCollisions();

	XMFLOAT3 GetPickRay(float fMouseX, float fMouseY);


	float bBomb = false;
	float fBombElapseTime		=      0;
	float fBombSpeed = WALL_HALF_DEPTH;
	XMFLOAT3 xmf3BombPosition;
	void BlowUpEnermy(float fElapseTime);

	void ResponObject(float fElapsedTime);
	void ResponBoss();
	
	void RemoveEnermy();
	void RemoveBonusObject();
	void RemoveBullet();
};
