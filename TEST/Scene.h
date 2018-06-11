//-----------------------------------------------------------------------------
// File: Scene.h
//-----------------------------------------------------------------------------

#pragma once

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

	virtual bool ProcessInput();
    virtual void AnimateObjects(float fTimeElapsed);
    virtual void Render(ID3D12GraphicsCommandList *pd3dCommandList, CCamera *pCamera=NULL);

	void ReleaseUploadBuffers();

protected:
	ID3D12RootSignature			*m_pd3dGraphicsRootSignature = NULL;

	std::list<CGameObject * >						m_listpObjects;
	
};

class CGameScene : public CScene
{
public:
	CGameScene();
	~CGameScene();

	void BuildObjects(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList);
	void ReleaseObjects();



protected:
	CAirplanePlayer									* AirplanePlayer;
	std::list<CGameObject * >						m_listpEnermy;

protected:
	CPlayerShader * pPlayerShader;
	CWallObjectShader * pWallShader;

	CObjectsShader * pObjectShader;

public:
	virtual bool ProcessInput();
	virtual void AnimateObjects(float fTimeElapsed);
	virtual void Render(ID3D12GraphicsCommandList *pd3dCommandList, CCamera *pCamera = NULL);


};
