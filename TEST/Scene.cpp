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
	m_pd3dGraphicsRootSignature = CreateGraphicsRootSignature(pd3dDevice);

	//매쉬 설정
	m_pWallMesh =  new CWallCubeMesh(pd3dDevice, pd3dCommandList, 100.0f, 100.0f, 250.0f);
	m_pWallMesh->SetAABB(XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(50.0f, 50.0f, 125.0f));
	
	m_pNomalEnermyMesh = new CVariousColorsCubeMesh(pd3dDevice, pd3dCommandList, 2.0f, 2.0f, 2.0f);
	m_pNomalEnermyMesh->SetAABB(XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(1.0f, 1.0f, 1.0f));
	
	CVariousColorsCubeMesh * m_BossMesh = new CVariousColorsCubeMesh(pd3dDevice, pd3dCommandList, 20.0f, 20.0f, 20.0f);
	m_BossMesh->SetAABB(XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(10.0f, 10.0f, 10.0f));

	//쉐이더 설정
	m_pObjectShader = new CObjectsShader();
	m_pObjectShader->CreateShader(pd3dDevice, m_pd3dGraphicsRootSignature);
	m_pObjectShader->CreateShaderVariables(pd3dDevice, pd3dCommandList);

	m_pWallShader = new CWallObjectShader();
	m_pWallShader->CreateShader(pd3dDevice, m_pd3dGraphicsRootSignature);
	m_pWallShader->CreateShaderVariables(pd3dDevice, pd3dCommandList);


	//폭발 준비

	CExplosiveObject::PrepareExplosion(pd3dDevice, pd3dCommandList);



	//벽을 만들어보자
	m_pWallsObject = new CWallsObject();
	m_pWallsObject->SetPosition(0.0f, 0.0f, 125.0f);
	m_pWallsObject->SetMovingDirection(XMFLOAT3(0.0f, 0.0f, 0.0f));
	m_pWallsObject->m_fMovingSpeed = 0.0f;
	m_pWallsObject->SetMesh(m_pWallMesh);
	m_pWallsObject->SetShader(m_pWallShader);

	//보스 생성
	m_pBoss = new CBoss();
	m_pBoss->SetMesh(m_BossMesh);
	if (m_pObjectShader) m_pBoss->SetShader(m_pObjectShader);
	m_pBoss->SetPosition(XMFLOAT3(0.0f, 0.0f, 980.0f));
	m_pBoss->SetMovingDirection(XMFLOAT3(0, 0, 0));
	m_pBoss->SetRotationAxis(XMFLOAT3(0, 1, 0));
	m_pBoss->SetRotationSpeed(120);

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

	if (m_pWallsObject) delete m_pWallsObject;

	if (m_listpEnermys.size() > 0) {
		for (auto & data : m_listpEnermys)
			if (data) delete data;
		m_listpEnermys.~list();
	}

	if (m_pBoss) delete m_pBoss;
}
void CScene::ReleaseUploadBuffers()
{
	if (m_pWallsObject) m_pWallsObject->ReleaseUploadBuffers();

	if (m_listpEnermys.size() > 0) {
		for (auto & data : m_listpEnermys)
			if (data) data->ReleaseUploadBuffers();
	}

	if (m_pBoss) m_pBoss->ReleaseUploadBuffers();
}

void CScene::ResponEnermy(float fElapsedTime)
{
	if (m_pPlayer) {
		if (m_pPlayer->GetPosition().z > 855)
			return;

		std::random_device rd;
		std::default_random_engine dre(rd());
		std::uniform_real_distribution<float> ufr(-WALL_HALF_SIZE, WALL_HALF_SIZE);
		std::uniform_real_distribution<float> ufrRotaionAngle(0.0f, 360.0f);

		m_fEnermyResponTime -= fElapsedTime;

		if (m_fEnermyResponTime < 0) {
			++m_iResponCnt;

			if (m_iResponCnt % 10) {
				m_fEnermyResponTime = m_fEnermyResponInitTime;
				CEnermy * Enermy = new CEnermy;
				if (m_pNomalEnermyMesh) Enermy->SetMesh(m_pNomalEnermyMesh);
				if (m_pObjectShader) Enermy->SetShader(m_pObjectShader);
				XMFLOAT3 temp_position = XMFLOAT3(ufr(dre), ufr(dre), m_pPlayer->GetPosition().z + 100);
				Enermy->SetPosition(temp_position);
				Enermy->SetMovingDirection(XMFLOAT3(ufr(dre), ufr(dre), ufr(dre)));
				Enermy->SetMovingSpeed(ENERMYSPEED);
				Enermy->SetRotationAxis(XMFLOAT3(ufrRotaionAngle(dre), ufrRotaionAngle(dre), ufrRotaionAngle(dre)));
				Enermy->SetRotationSpeed(120);

				m_listpEnermys.emplace_back(Enermy);
			}

			// 빨간색 적을 만들어보자
			else {
				/*CEnermy * Enermy = new CREnermy;
				Enermy->SetPosition(XMFLOAT3(ufr(dre), ufr(dre), m_pPlayer->GetPosition().z + 100));
				Enermy->SetMovingDirection(XMFLOAT3(ufr(dre), ufr(dre), ufr(dre)));
				Enermy->SetRotationAxis(XMFLOAT3(ufrRotaionAngle(dre), ufrRotaionAngle(dre), ufrRotaionAngle(dre)));
				Enermy->SetRotationSpeed(120);

				m_plEnermys.emplace_back(Enermy);*/
				m_fEnermyResponTime = m_fEnermyResponInitTime;
			}
			//delete Enermy;
		}
	}

}

void CScene::CheckPlayerByWallCollision(float fElapseTime)
{
	ContainmentType containType = m_pWallsObject->m_xmAABB.Contains(m_pPlayer->m_xmAABB);					//벽으로 충돌을 체크
	float wall_revise = WALL_HALF_SIZE - 3;
	XMFLOAT3 position = m_pPlayer->GetPosition();
	if (m_pPlayer->GetPosition().x < -wall_revise) {
		m_pPlayer->SetPosition(XMFLOAT3(-wall_revise, position.y, position.z));
	}

	else if (m_pPlayer->GetPosition().x > wall_revise) {
		m_pPlayer->SetPosition(XMFLOAT3(wall_revise, position.y, position.z));

	}

	if (m_pPlayer->GetPosition().y < -wall_revise) {
		m_pPlayer->SetPosition(XMFLOAT3(position.x, -wall_revise, position.z));
	}

	else if (m_pPlayer->GetPosition().y > wall_revise) {
		m_pPlayer->SetPosition(XMFLOAT3(position.x, wall_revise, position.z));

	}

	if (m_pPlayer->GetPosition().z < 3) {
		m_pPlayer->SetPosition(XMFLOAT3(position.x, position.y, 3.0f));
	}

	else if (m_pPlayer->GetPosition().z > 997) {
		m_pPlayer->SetPosition(XMFLOAT3(position.x, position.y,997.0f));

	}
	//XMMATRIX mtxRotate = XMMatrixRotationRollPitchYaw(XMConvertToRadians(-90.0f), 0.0f, 0.0f);
	//XMFLOAT3 MoveDirection = Vector3::TransformCoord(m_pPlayer->m_xmf3MovingDirection, mtxRotate);

	const XMFLOAT3 & MoveDirection = m_pPlayer->m_xmf3MovingDirection;
	if (m_pPlayer->GetLook().z > 0) {				//플레이어가 앞을 바라 볼 경우
		if (m_pPlayer->GetPosition().z > 750)
			m_pWallsObject->SetPosition(0, 0, 850.0f);

		if (MoveDirection.z < 0) { //뒤로 갈 경우
			if (m_pPlayer->GetPosition().z < m_pWallsObject->GetPosition().z - 112.5) {
				m_pWallsObject->MoveForward(-12.5f);
			}
		}
		else   // 앞으로 갈경우
		{
			if (m_pPlayer->GetPosition().z > m_pWallsObject->GetPosition().z - 100) {
				m_pWallsObject->MoveForward(12.5f);
			}
		}
	}
	else                                            //플레이어가 뒤를 바라 볼 경우
	{
		if (m_pPlayer->GetPosition().z < 250)
			m_pWallsObject->SetPosition(0, 0, 150.0f);

		if (MoveDirection.z >0) { //앞으로 갈 경우
			if (m_pPlayer->GetPosition().z > m_pWallsObject->GetPosition().z + 100) {
				m_pWallsObject->MoveForward(12.5f);
			}
		}
		else   // 뒤로 갈경우
		{
			if (m_pPlayer->GetPosition().z < m_pWallsObject->GetPosition().z + 100) {
				m_pWallsObject->MoveForward(-12.5f);
			}
		}
	}
}

void CScene::CheckBulletByWallCollision()
{
	auto m_plBullets_itor = m_plBullets.begin();
	for (;m_plBullets_itor != m_plBullets.end();) {
		CGameObject* bullet = *m_plBullets_itor;
		if (!bullet->m_xmAABB.Intersects(m_pWallsObject->m_xmAABB)) {
			m_plBullets_itor = m_plBullets.erase(m_plBullets_itor);
		}
		else ++m_plBullets_itor;
	}
}

void CScene::CheckEnermyByWallCollision()
{
	for (auto & data : m_listpEnermys) {
		if (!data->m_xmAABB.Intersects(m_pWallsObject->m_xmAABB)) {
			data->m_xmf3MovingDirection = Vector3::ScalarProduct(data->m_xmf3MovingDirection, -1);
		}
	}
}

void CScene::CheckEnermyByBulletCollisions()
{
	for (auto & Enermy : m_listpEnermys) {
		for (auto & pBullet : m_plBullets) {
			if (pBullet->m_xmAABB.Intersects(Enermy->m_xmAABB) && !Enermy->m_bBlowingUp) {
				Enermy->m_bBlowingUp = true;
				pBullet->m_bActive = false;
			}
		}
	}
}

void CScene::CheckPlayerByBulletCollisions()
{
}

void CScene::CheckBossByBulletCollisions()
{

}

void CScene::RemoveBullet()
{
	RemoveObjects(m_plBullets);
}

void CScene::RemoveEnermy()
{
	RemoveObjects(m_listpEnermys);
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
	//적들을 리스폰 시킨다.
	ResponEnermy(fTimeElapsed);


	//플레이어가 총알을 쏜다.
	
	if (m_pPlayer->CanShot()){
		m_plBullets.emplace_back(m_pPlayer->ShotBullet());
	}


	//애니메이트
	if (m_pPlayer) m_pPlayer->Animate(fTimeElapsed);

	if (m_pWallsObject) m_pWallsObject->Animate(fTimeElapsed);

	for (auto & data : m_plBullets)
		data->Animate(fTimeElapsed);

	for (auto & data : m_listpEnermys)
		if (data) data->Animate(fTimeElapsed);
	
	if (m_pBoss) m_pBoss->Animate(fTimeElapsed);


	//충돌체크를 진행한다.
	CheckPlayerByWallCollision(fTimeElapsed);
	CheckEnermyByWallCollision();
	CheckBulletByWallCollision();
	CheckEnermyByBulletCollisions();

	RemoveBullet();
	RemoveEnermy();


}

void CScene::Render(ID3D12GraphicsCommandList *pd3dCommandList, CCamera *pCamera)
{
	pCamera->SetViewportsAndScissorRects(pd3dCommandList);
	pd3dCommandList->SetGraphicsRootSignature(m_pd3dGraphicsRootSignature);
	pCamera->UpdateShaderVariables(pd3dCommandList);

	//벽그리기
	if(m_pWallsObject) m_pWallsObject->Render(pd3dCommandList, pCamera);

	for (auto & data : m_plBullets)
		if(data) data->Render(pd3dCommandList, pCamera);
	
	for (auto & data : m_listpEnermys)
		if (data) data->Render(pd3dCommandList, pCamera);

	if (m_pBoss) m_pBoss->Render(pd3dCommandList, pCamera);

}

