#pragma once

#include "GameObject.h"
#include "Player.h"

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
	std::list<std::shared_ptr<CBullet>>				m_plBullets;
	std::list<std::shared_ptr<CEnermy>>				m_plEnermys;
	std::list<std::shared_ptr<CBonusObject>>		m_plBonusObjects;


	float				m_fEnermyResponInitTime = ENERMYRESPONTIME;
	float				m_fEnermyResponTime = ENERMYRESPONTIME;
	float				m_fBonusObjectInitResponTime = BONOUSOBJECTRESPONTIME;
	float				m_fBonusObjectResponTime = BONOUSOBJECTRESPONTIME;

public:
	void  CheckPlayerByWallCollision(float fElapseTime);
	void CheckBulletByWallCollision();
	void CheckEnermyByBulletCollisions();

	void ResponObject(float fElapsedTime);
	void ShotBullet(CGameObject * pObject, float & fBulletCoolTime,const float fBulletMaxCoolTime);
};
