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
	CGameObjectManager			*m_pGameObjectManager = NULL;


	virtual void BuildObjects();
	virtual void ReleaseObjects();

	virtual void Animate(float fElapsedTime);
	virtual void Render(HDC hDCFrameBuffer, CCamera *pCamera);

	virtual void OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);
	virtual void OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);

	void ShotBullet(XMFLOAT3 xmf3_Position, XMFLOAT3 xmf3_Diretxion, float fElapseTime, float & fBulletCoolTime, float fBulletMaxCoolTime);


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
	float				m_fEnermyResponInitTime = ENERMYRESPONTIME;
	float				m_fEnermyResponTime = ENERMYRESPONTIME;
	float				m_fBonusObjectInitResponTime = BONOUSOBJECTRESPONTIME;
	float				m_fBonusObjectResponTime = BONOUSOBJECTRESPONTIME;

public:
	void CheckPlayerByWallCollision(float fElapsedTime);
	void CheckObjectByBulletCollisions();

	void ResponObject(float fElapsedTime);
	void ShotBullet(CGameObject * pObject, float & fBulletCoolTime,const float fBulletMaxCoolTime);
};
