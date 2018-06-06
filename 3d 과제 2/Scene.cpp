//-----------------------------------------------------------------------------
// File: CScene.cpp
//-----------------------------------------------------------------------------

#include "stdafx.h"
#include "Scene.h"

CScene::CScene()
{
}

CScene::~CScene()
{
}

void CScene::BuildObjects(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList)
{
	m_pd3dGraphicsRootSignature = CreateGraphicsRootSignature(pd3dDevice);		//그래픽 파이프 라인의 파라매터 설정

#ifdef _WITH_TEXT_MESH
	CMesh *pUfoMesh = new CMesh(pd3dDevice, pd3dCommandList, L"UFO.txt");
#else
	CMesh *pUfoMesh = new CMesh(pd3dDevice, pd3dCommandList, L"UFO.bin");
#endif

#ifdef _WITH_TEXT_MESH
	CMesh *pFlyerMesh = new CMesh(pd3dDevice, pd3dCommandList, L"FlyerPlayership.txt");
#else
	CMesh *pFlyerMesh = new CMesh(pd3dDevice, pd3dCommandList, L"FlyerPlayership.bin");
#endif

	m_nObjects = 5;
	m_ppObjects = new CGameObject*[m_nObjects];

	CLightingShader *pShader = new CLightingShader();
	pShader->CreateShader(pd3dDevice, m_pd3dGraphicsRootSignature);
	pShader->CreateShaderVariables(pd3dDevice, pd3dCommandList);

	m_ppObjects[0] = new CGameObject();
	m_ppObjects[0]->SetMesh(pUfoMesh);
	m_ppObjects[0]->SetShader(pShader);
	m_ppObjects[0]->SetPosition(6.0f, 0.0f, 13.0f);

	m_ppObjects[1] = new CGameObject();
	m_ppObjects[1]->SetMesh(pUfoMesh);
	m_ppObjects[1]->SetShader(pShader);
	m_ppObjects[1]->SetPosition(10.0f, -2.0f, 8.0f);

	m_ppObjects[2] = new CGameObject();
	m_ppObjects[2]->SetMesh(pUfoMesh);
	m_ppObjects[2]->SetShader(pShader);
	m_ppObjects[2]->SetPosition(-5.0f, -4.0f, 11.0f);

	m_ppObjects[3] = new CGameObject();
	m_ppObjects[3]->SetMesh(pUfoMesh);
	m_ppObjects[3]->SetShader(pShader);
	m_ppObjects[3]->SetPosition(-10.0f, -2.0f, 8.0f);

	m_ppObjects[4] = new CGameObject();
	m_ppObjects[4]->SetMesh(pFlyerMesh);
	m_ppObjects[4]->SetShader(pShader);
	m_ppObjects[4]->SetPosition(0.0f, 4.0f, 20.0f);
	m_ppObjects[4]->Rotate(0.0f, 180.0f, 0.0f);
}

ID3D12RootSignature *CScene::CreateGraphicsRootSignature(ID3D12Device *pd3dDevice)
{
	ID3D12RootSignature *pd3dGraphicsRootSignature = NULL;

	D3D12_ROOT_PARAMETER pd3dRootParameters[2];
	pd3dRootParameters[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_32BIT_CONSTANTS;
	pd3dRootParameters[0].Constants.Num32BitValues = 16;
	pd3dRootParameters[0].Constants.ShaderRegister = 0; //World
	pd3dRootParameters[0].Constants.RegisterSpace = 0;
	pd3dRootParameters[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;
	pd3dRootParameters[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_32BIT_CONSTANTS;
	pd3dRootParameters[1].Constants.Num32BitValues = 32;
	pd3dRootParameters[1].Constants.ShaderRegister = 1; //Camera
	pd3dRootParameters[1].Constants.RegisterSpace = 0;
	pd3dRootParameters[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;

	D3D12_ROOT_SIGNATURE_FLAGS d3dRootSignatureFlags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT | D3D12_ROOT_SIGNATURE_FLAG_DENY_HULL_SHADER_ROOT_ACCESS | D3D12_ROOT_SIGNATURE_FLAG_DENY_DOMAIN_SHADER_ROOT_ACCESS | D3D12_ROOT_SIGNATURE_FLAG_DENY_GEOMETRY_SHADER_ROOT_ACCESS | D3D12_ROOT_SIGNATURE_FLAG_DENY_PIXEL_SHADER_ROOT_ACCESS;
	D3D12_ROOT_SIGNATURE_DESC d3dRootSignatureDesc;
	::ZeroMemory(&d3dRootSignatureDesc, sizeof(D3D12_ROOT_SIGNATURE_DESC));
	d3dRootSignatureDesc.NumParameters = _countof(pd3dRootParameters);
	d3dRootSignatureDesc.pParameters = pd3dRootParameters;
	d3dRootSignatureDesc.NumStaticSamplers = 0;
	d3dRootSignatureDesc.pStaticSamplers = NULL;
	d3dRootSignatureDesc.Flags = d3dRootSignatureFlags;

	ID3DBlob *pd3dSignatureBlob = NULL;
	ID3DBlob *pd3dErrorBlob = NULL;
	D3D12SerializeRootSignature(&d3dRootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1, &pd3dSignatureBlob, &pd3dErrorBlob);
	pd3dDevice->CreateRootSignature(0, pd3dSignatureBlob->GetBufferPointer(), pd3dSignatureBlob->GetBufferSize(), __uuidof(ID3D12RootSignature), (void **)&pd3dGraphicsRootSignature);
	if (pd3dSignatureBlob) pd3dSignatureBlob->Release();
	if (pd3dErrorBlob) pd3dErrorBlob->Release();

	return(pd3dGraphicsRootSignature);
}

void CScene::ReleaseObjects()
{
	if (m_pd3dGraphicsRootSignature) m_pd3dGraphicsRootSignature->Release();

	if (m_ppObjects)
	{
		for (int j = 0; j < m_nObjects; j++) if (m_ppObjects[j]) delete m_ppObjects[j];
		delete[] m_ppObjects;
	}
}

void CScene::ReleaseUploadBuffers()
{
	if (m_ppObjects)
	{
		for (int j = 0; j < m_nObjects; j++) if (m_ppObjects[j]) m_ppObjects[j]->ReleaseUploadBuffers();
	}
}

bool CScene::OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
{
	return(false);
}

bool CScene::OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
{
	return(false);
}

bool CScene::ProcessInput()
{
	return(false);
}

void CScene::AnimateObjects(float fTimeElapsed)
{
	for (int j = 0; j < m_nObjects; j++)
	{
		m_ppObjects[j]->Animate(fTimeElapsed);
	}
}

void CScene::Render(ID3D12GraphicsCommandList *pd3dCommandList, CCamera *pCamera)
{
	pCamera->SetViewportsAndScissorRects(pd3dCommandList);
	pd3dCommandList->SetGraphicsRootSignature(m_pd3dGraphicsRootSignature);
	pCamera->UpdateShaderVariables(pd3dCommandList);

	for (int j = 0; j < m_nObjects; j++)
	{
		if (m_ppObjects[j]) m_ppObjects[j]->Render(pd3dCommandList, pCamera);
	}
}

