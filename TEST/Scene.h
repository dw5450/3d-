//-----------------------------------------------------------------------------
// File: Scene.h
//-----------------------------------------------------------------------------

#pragma once

#define RemoveObjects(pObjectList) \
	auto  Itor = pObjectList.begin(); \
	for (;Itor != pObjectList.end();) {\
		CGameObject * object =  *Itor; \
	if (!object->m_bActive ){\
		Itor = pObjectList.erase(Itor);\
	}\
	else ++Itor;\
	}

#include "Shader.h"
#include "Player.h"

class CScene
{
public:
    CScene();
    ~CScene();

	bool OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);
	bool OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);

	void BuildObjects(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList);
	void ReleaseObjects();

	ID3D12RootSignature *CreateGraphicsRootSignature(ID3D12Device *pd3dDevice);
	ID3D12RootSignature *GetGraphicsRootSignature() { return(m_pd3dGraphicsRootSignature); }

	bool ProcessInput();
    void AnimateObjects(float fTimeElapsed);
    void Render(ID3D12GraphicsCommandList *pd3dCommandList, CCamera *pCamera=NULL);

	void ReleaseUploadBuffers();

private:			//매쉬
	CMesh * m_pWallMesh;
	CMesh * m_pNomalEnermyMesh;

private:			//쉐이더
	CObjectsShader * m_pObjectShader;
	CWallObjectShader * m_pWallShader;


public:
	CPlayer * m_pPlayer = nullptr;
	CBoss	* m_pBoss = nullptr;
protected:
	std::list<CBullet *>			m_plBullets;
	CWallsObject * m_pWallsObject = NULL;
	std::list<CEnermy *>				m_listpEnermys;
	ID3D12RootSignature			*m_pd3dGraphicsRootSignature = NULL;

public:		//responEnermy
	float				m_fEnermyResponInitTime = ENERMYRESPONTIME;
	float				m_fEnermyResponTime = ENERMYRESPONTIME;
	size_t				m_iResponCnt = 0;
	void ResponEnermy(float fElapsedTime);

public :	//충돌체크
	void CheckPlayerByWallCollision(float fElapseTime);
	void CheckBulletByWallCollision();
	void CheckEnermyByWallCollision();
	void CheckPlayerByEnermyCollisions();
	void CheckEnermyByBulletCollisions();
	void CheckPlayerByBulletCollisions();
	void CheckBossByBulletCollisions();


	float bBomb = false;
	float fBombElapseTime = 0;
	float fBombSpeed = 125.0f;
	XMFLOAT3 xmf3BombPosition;
	void BlowUpEnermy(float fElapseTime, float fBombDistance);


public :  //Remove
	void RemoveBullet();
	void RemoveEnermy();
};
