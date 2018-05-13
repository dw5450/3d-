#pragma once

#include "GameObject.h"
#include "Player.h"


#define RemoveObjects(pObjectList) \
	auto  Itor = pObjectList.begin(); \
	for (;Itor != pObjectList.end();) {\
		std::shared_ptr<CGameObject> object =  *Itor; \
	if (!object->m_bActive ){\
		Itor = pObjectList.erase(Itor);\
	}\
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
	virtual void RendUI(HDC hDCFrameBuffer, CCamera *pCamera);

	virtual void OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);
	virtual void OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);

public:
	CWallsObject * m_pWallsObject = NULL;
	CBoss				*m_pBoss = NULL;
	std::list<std::shared_ptr<CBullet>>				m_plBullets;
	std::list<std::shared_ptr<CEnermy>>				m_plEnermys;

	float				m_fEnermyResponInitTime = ENERMYRESPONTIME;
	float				m_fEnermyResponTime = ENERMYRESPONTIME;
	size_t				m_iResponCnt = 0;


public:
	void CheckPlayerByWallCollision(float fElapseTime);
	void CheckBulletByWallCollision();
	void CheckEnermyByWallCollision();
	void CheckEnermyByBulletCollisions();
	void CheckPlayerByBulletCollisions();
	void CheckBossByBulletCollisions();

	void CheckPlayerByEnermyCollisions();

	XMFLOAT3 GetPickRay(float fMouseX, float fMouseY);


	float bBomb = false;
	float fBombElapseTime = 0;
	float fBombSpeed = WALL_HALF_DEPTH;
	XMFLOAT3 xmf3BombPosition;
	void BlowUpEnermy(float fElapseTime, float fBombDistance);

	void ResponObject(float fElapsedTime);
	void ResponBoss();

	void RemoveEnermy();
	void RemoveBullet();

};

