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
protected:
	std::list<CBullet *>			m_plBullets;
	CWallsObject * m_pWallsObject = NULL;
	std::list<CEnermy *>				m_listpEnermys;
	CBoss		* m_pBoss;
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
	void CheckEnermyByBulletCollisions();
	void CheckPlayerByBulletCollisions();
	void CheckBossByBulletCollisions();

public :  //Remove
	void RemoveBullet();
	void RemoveEnermy();
};
