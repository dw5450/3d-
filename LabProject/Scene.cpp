#include "stdafx.h"
#include "Scene.h"
//����� �׸��ϴ�
//������Ʈ���� ����, ��ġ ��ŵ�ϴ�.
//�浹üũ�� �����մϴ�.

CScene::CScene()
{

}

CScene::~CScene()
{
}

void CScene::OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
{
}

void CScene::OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
{
}


void CScene::BuildObjects()
{
	//������
	float fHalfWidth = WALL_HALF_SIZE, fHalfHeight = WALL_HALF_SIZE, fHalfDepth = WALL_HALF_DEPTH;									//1.0f �� 1m
	CWallMesh *pWallCubeMesh = new CWallMesh(fHalfWidth * 2.0f, fHalfHeight * 2.0f, fHalfDepth * 2.0f, 20);
	pWallCubeMesh->SetAABB(XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(fHalfWidth, fHalfHeight, fHalfDepth));

	m_pWallsObject = new CWallsObject();
	m_pWallsObject->SetPosition(0, 0, WALL_HALF_DEPTH);
	m_pWallsObject->SetMovingDirection(XMFLOAT3(0, 0, 0));
	m_pWallsObject->m_fMovingSpeed = 0.0f;
	m_pWallsObject->SetMesh(pWallCubeMesh);
	m_pWallsObject->SetColor(RGB(0, 0, 0));

}

void CScene::ReleaseObjects()
{
	if (m_pWallsObject) delete m_pWallsObject;
	if (m_pPlayer) delete m_pPlayer;
}

void CScene::Animate(float fElapsedTime)
{
	ResponObject(fElapsedTime);

	if (m_pPlayer->CanShot())
		m_plBullets.emplace_back(m_pPlayer->ShotBullet());

	m_pPlayer->SetColor(RGB(0, 0, 255));

	m_pWallsObject->Animate(fElapsedTime);
	m_pPlayer->Animate(fElapsedTime);

	if (m_pBoss) {
		m_pBoss->Animate(fElapsedTime);
		m_pBoss->TraceObject(m_pPlayer);
		if (m_pBoss->CanShot())	m_plBullets.emplace_back(m_pBoss->ShotBullet());
	}

	for (std::shared_ptr<CBullet> & data : m_plBullets)
		data->Animate(fElapsedTime);

	float LOD2 = 250;
	float LOD3 = 500;
	if (m_plEnermys.size() != 0) {
		float LOD2 = 250 / static_cast<float>(m_plEnermys.size());
		float LOD3 = 500 / static_cast<float>(m_plEnermys.size());
	}
	for (std::shared_ptr<CEnermy> & data : m_plEnermys) {
		if (m_pPlayer->m_xmf3Look.z > 0) {
			if (m_pPlayer->m_xmf3Position.z + LOD3 < data->GetPosition().z) data->LOD = 3;
			else if (m_pPlayer->m_xmf3Position.z + LOD2 < data->GetPosition().z) data->LOD = 2;
			else data->LOD = 1;
		}

		if (m_pPlayer->m_xmf3Look.z < 0) {
			if (m_pPlayer->m_xmf3Position.z - LOD3 > data->GetPosition().z) data->LOD = 3;
			else if (m_pPlayer->m_xmf3Position.z - LOD2 > data->GetPosition().z) data->LOD = 2;
			else data->LOD = 1;
		}
		if (data->m_fElapseTime > 3.0)
			data->TraceObject(m_pPlayer);
		data->Animate(fElapsedTime);
	}

	if (m_pPlayer->ShotBomb() && !bBomb) {
		bBomb = true;
		xmf3BombPosition = m_pPlayer->GetPosition();
	}

	if (bBomb) {
		BlowUpEnermy(fElapsedTime, m_pPlayer->m_fBombDistance);
	}

	CheckPlayerByWallCollision(fElapsedTime);
	CheckBulletByWallCollision();
	CheckEnermyByWallCollision();
	CheckEnermyByBulletCollisions();
	CheckPlayerByEnermyCollisions();
	CheckPlayerByBulletCollisions();
	CheckBossByBulletCollisions();

	RemoveEnermy();
	RemoveBullet();


}

void CScene::Render(HDC hDCFrameBuffer, CCamera * pCamera)
{
	m_pWallsObject->Render(hDCFrameBuffer, pCamera);

	if (m_pBoss)
		m_pBoss->Render(hDCFrameBuffer, pCamera);

	for (std::shared_ptr<CBullet> & data : m_plBullets)
		data->Render(hDCFrameBuffer, pCamera);

	for (std::shared_ptr<CEnermy> & data : m_plEnermys)
		data->Render(hDCFrameBuffer, pCamera);

	m_pPlayer->Render(hDCFrameBuffer, pCamera);

	RendUI(hDCFrameBuffer, pCamera);

}

void CScene::RendUI(HDC hDCFrameBuffer, CCamera *pCamera)
{
	//UI�� �׷� ����
	pCamera->m_Viewport;
	int fViewportLeft = pCamera->m_Viewport.m_xStart;
	int fViewportTop = pCamera->m_Viewport.m_yStart;
	int fViewportRight = pCamera->m_Viewport.m_xStart + pCamera->m_Viewport.m_nWidth;
	int fViewportBottom = pCamera->m_Viewport.m_yStart + pCamera->m_Viewport.m_nHeight;



	//�÷��̾��� ������ �׷�����
	HPEN hPen = ::CreatePen(PS_SOLID, 0, RGB(0, 0, 0));
	HPEN hOldPen = (HPEN)::SelectObject(hDCFrameBuffer, hPen);

	Rectangle(hDCFrameBuffer, fViewportLeft, fViewportBottom - 70,
		fViewportLeft + 500, fViewportBottom);

	::SelectObject(hDCFrameBuffer, hOldPen);
	::DeleteObject(hPen);

	//���� ���� ����
	TCHAR str[100];
	wsprintf(str, TEXT("���� : "));
	TextOut(hDCFrameBuffer, fViewportLeft + 5, fViewportBottom - 65, str, lstrlen(str));


	hPen = ::CreatePen(PS_SOLID, 0, RGB(0,255,0));
	hOldPen = (HPEN)::SelectObject(hDCFrameBuffer, hPen);
	for (unsigned int i = 0; i < m_pPlayer->m_iLife; i++)
	{
		Rectangle(hDCFrameBuffer, fViewportLeft + 50 + 20 * i, fViewportBottom - 60,
			fViewportLeft + 60 + 20 * i, fViewportBottom - 50);
	}

	::SelectObject(hDCFrameBuffer, hOldPen);
	::DeleteObject(hPen);

	wsprintf(str, TEXT("�Ѿ� : "));
	TextOut(hDCFrameBuffer, fViewportLeft + 5, fViewportBottom - 45, str, lstrlen(str));


	hPen = ::CreatePen(PS_SOLID, 0, RGB(255, 0, 0));
	hOldPen = (HPEN)::SelectObject(hDCFrameBuffer, hPen);
	//�Ѿ�
	for (unsigned int i = 0; i <  m_pPlayer->m_iBulletNum; i++)
	{
		Rectangle(hDCFrameBuffer, fViewportLeft + 50 + 20 * i, fViewportBottom - 40,
			fViewportLeft + 60 + 20 * i, fViewportBottom - 30);
	}

	::SelectObject(hDCFrameBuffer, hOldPen);
	::DeleteObject(hPen);


	wsprintf(str, TEXT("��ź : "));
	TextOut(hDCFrameBuffer, fViewportLeft + 5, fViewportBottom - 25, str, lstrlen(str));

	hPen = ::CreatePen(PS_SOLID, 0, RGB(0, 0, 255));
	hOldPen = (HPEN)::SelectObject(hDCFrameBuffer, hPen);

	//��ź

	for (unsigned int i = 0; i <  m_pPlayer->m_iBombNum; i++)
	{
		Rectangle(hDCFrameBuffer, fViewportLeft + 50 + 20 * i, fViewportBottom - 10,
			fViewportLeft + 60 + 20 * i, fViewportBottom - 20);
	}
	::SelectObject(hDCFrameBuffer, hOldPen);
	::DeleteObject(hPen);



	//�̴ϸ��� �׷�����
	hPen = ::CreatePen(PS_SOLID, 0, RGB(0, 0, 0));
	hOldPen = (HPEN)::SelectObject(hDCFrameBuffer, hPen);

	Rectangle(hDCFrameBuffer, fViewportRight - 400, fViewportTop ,
		fViewportRight, fViewportTop + 40);

	::SelectObject(hDCFrameBuffer, hOldPen);
	::DeleteObject(hPen);

	//���� ��ġ�� �׷� ����

	hPen = ::CreatePen(PS_SOLID, 0, m_pPlayer->m_dwColor);
	hOldPen = (HPEN)::SelectObject(hDCFrameBuffer, hPen);

	int temp_y = (int)(40 - m_pPlayer->GetPosition().y * 0.4);
	int temp_z = (int)(m_pPlayer->GetPosition().z * 0.4);
	Rectangle(hDCFrameBuffer, fViewportRight - 400 + temp_z - 2, fViewportTop - 20 + temp_y - 2,
		fViewportRight - 400 + temp_z + 2, fViewportTop - 20 + temp_y + 2);

	::SelectObject(hDCFrameBuffer, hOldPen);
	::DeleteObject(hPen);


	//������ �׷� ����

	for (auto & pEnermy : m_plEnermys) {
		hPen = ::CreatePen(PS_SOLID, 0, pEnermy->m_dwColor);
		hOldPen = (HPEN)::SelectObject(hDCFrameBuffer, hPen);

		temp_y = (int)(40 - pEnermy->GetPosition().y * 0.4);
		temp_z = (int)(pEnermy->GetPosition().z * 0.4);
		Rectangle(hDCFrameBuffer, fViewportRight - 400 + temp_z - 1, fViewportTop - 20 + temp_y - 1,
			fViewportRight - 400 + temp_z + 1, fViewportTop - 20 + temp_y + 1);

		::SelectObject(hDCFrameBuffer, hOldPen);
		::DeleteObject(hPen);
	}



	//������ �׷� ����

	if(m_pBoss) {
		hPen = ::CreatePen(PS_SOLID, 0, m_pBoss->m_dwColor);
		hOldPen = (HPEN)::SelectObject(hDCFrameBuffer, hPen);

		temp_y = (int)(40 - m_pBoss->GetPosition().y * 0.4);
		temp_z = (int)(m_pBoss->GetPosition().z * 0.4);
		Rectangle(hDCFrameBuffer, fViewportRight - 400 + temp_z - 3, fViewportTop - 20 + temp_y - 3,
			fViewportRight - 400 + temp_z + 3, fViewportTop - 20 + temp_y + 3);

		::SelectObject(hDCFrameBuffer, hOldPen);
		::DeleteObject(hPen);
	}



}

void CScene::CheckPlayerByWallCollision(float fElapseTime)
{
	ContainmentType containType = m_pWallsObject->m_xmAABB.Contains(m_pPlayer->m_xmAABB);					//������ �浹�� üũ

	float wall_revise = WALL_HALF_SIZE - 3;

	if (m_pPlayer->GetPosition().x < -wall_revise) {
		m_pPlayer->SetColor(RGB(255, 0, 0));
		m_pPlayer->m_xmf3Position.x = -wall_revise;
	}

	else if (m_pPlayer->GetPosition().x > wall_revise) {
		m_pPlayer->SetColor(RGB(255, 0, 0));
		m_pPlayer->m_xmf3Position.x = wall_revise;

	}

	if (m_pPlayer->GetPosition().y < -wall_revise){
		m_pPlayer->SetColor(RGB(255, 0, 0));
		m_pPlayer->m_xmf3Position.y = -wall_revise;
	}

	else if (m_pPlayer->GetPosition().y > wall_revise) {
		m_pPlayer->SetColor(RGB(255, 0, 0));
		m_pPlayer->m_xmf3Position.y = wall_revise;

	}


	if (m_pPlayer->GetPosition().z < 3) {
		m_pPlayer->m_xmf3Position.z = 3;
	}

	else if (m_pPlayer->GetPosition().z > 997) {
		m_pPlayer->SetColor(RGB(255, 0, 0));
		m_pPlayer->m_xmf3Position.z = 997;

	}

	if (m_pPlayer->GetPosition().z > 0 && !m_pBoss)
		ResponBoss();


	if (m_pPlayer->m_xmf3Look.z > 0) {				//�÷��̾ ���� �ٶ� �� ���

		if (m_pPlayer->m_xmf3Position.z > 750)
			m_pWallsObject->SetPosition(0, 0, 850.0f);

		if (m_pPlayer->m_xmf3MovingDirection.z < 0) { //�ڷ� �� ���
			if (m_pPlayer->m_xmf3Position.z < m_pWallsObject->GetPosition().z - 112.5) {
				m_pWallsObject->MoveForward(-12.5f);
			}
		}
		else   // �ڷ� �����
		{
			if (m_pPlayer->m_xmf3Position.z > m_pWallsObject->GetPosition().z - 100) {
				m_pWallsObject->MoveForward(12.5f);
			}
		}

	}
	else                                            //�÷��̾ �ڸ� �ٶ� �� ���
	{
		if (m_pPlayer->m_xmf3Position.z < 250)
			m_pWallsObject->SetPosition(0, 0, 150.0f);

		if (m_pPlayer->m_xmf3MovingDirection.z >0) { //������ �� ���
			if (m_pPlayer->m_xmf3Position.z > m_pWallsObject->GetPosition().z + 100) {
				m_pWallsObject->MoveForward(12.5f);
			}
		}
		else   // �ڷ� �����
		{
			if (m_pPlayer->m_xmf3Position.z < m_pWallsObject->GetPosition().z + 100) {
				m_pWallsObject->MoveForward(-12.5f);
			}
		}
	}
}

void CScene::CheckBulletByWallCollision()
{
	auto m_plBullets_itor = m_plBullets.begin();
	for (;m_plBullets_itor != m_plBullets.end();) {
		std::shared_ptr<CGameObject> bullet = *m_plBullets_itor;
		if (!bullet->m_xmAABB.Intersects(m_pWallsObject->m_xmAABB)) {
			m_plBullets_itor = m_plBullets.erase(m_plBullets_itor);
		}
		else ++m_plBullets_itor;
	}
}

void CScene::CheckEnermyByWallCollision()
{
	for (auto & data : m_plEnermys) {
		if (!data->m_xmAABB.Intersects(m_pWallsObject->m_xmAABB)) {
			data->m_xmf3MovingDirection = Vector3::ScalarProduct(data->m_xmf3MovingDirection, -1);
		}
	}

}

void CScene::CheckEnermyByBulletCollisions()
{
	for (std::shared_ptr<CEnermy> & Enermy : m_plEnermys) {
		for (auto & pBullet : m_plBullets) {
			if (pBullet->m_xmAABB.Intersects(Enermy->m_xmAABB) && !Enermy->m_bBlowingUp) {
				Enermy->m_bBlowingUp = true;
				pBullet->m_bActive = false;
			}
		}
	}
}

void CScene::CheckPlayerByEnermyCollisions()
{
	bool isEnermyBackPlayer = false;
	auto itor = m_plEnermys.begin();

	for (auto & pEnermy : m_plEnermys)
	{
		XMFLOAT3 ToEnermy = Vector3::Subtract(pEnermy->GetPosition(), m_pPlayer->m_xmf3Position);
		if (!isEnermyBackPlayer) {
			if (Vector3::DotProduct(ToEnermy, m_pPlayer->m_xmf3Look) < 0)
				isEnermyBackPlayer = true;
		}

		if (m_pPlayer->m_xmAABB.Intersects(pEnermy->m_xmAABB)) {
			if (!pEnermy->m_bBlowingUp) {
				pEnermy->m_bActive = false;
				m_pPlayer->m_iLife--;
			}
		}
	}
	if (isEnermyBackPlayer)
		m_pPlayer->SetColor(RGB(255, 0, 255));
}
void CScene::CheckPlayerByBulletCollisions()
{
	for (auto & pBullets : m_plBullets)
	{
		if (m_pPlayer->m_xmAABB.Intersects(pBullets->m_xmAABB)) {
			pBullets->m_bActive = false;
			m_pPlayer->m_iLife--;
		}
	}
}

void CScene::CheckBossByBulletCollisions()
{
	if (m_pBoss) {
		for (auto & pBullets : m_plBullets)
		{
			if (m_pBoss->m_xmAABB.Intersects(pBullets->m_xmAABB)) {
				pBullets->m_bActive = false;
				m_pBoss->m_iLife--;
			}
		}
	}
}

XMFLOAT3 CScene::GetPickRay(float xScreen, float yScreen)
{
	CCamera * pCamera = m_pPlayer->m_pCamera;
	float Left = (float)pCamera->m_Viewport.m_xStart;
	float Top = (float)pCamera->m_Viewport.m_yStart;
	XMFLOAT3 MouseFarPosition(0, 0, 0);
	float Width = (float)pCamera->m_Viewport.m_nWidth;
	float Height = (float)pCamera->m_Viewport.m_nHeight;
	////���� ��ǥ��� 
	MouseFarPosition.x = (float)(((2.0f * (xScreen - Left)) / Width) - 1.03);
	MouseFarPosition.y = (float)((-(2.0f * (yScreen - Top)) / Height) + 1.12);
	XMFLOAT4X4 InvProj(Matrix4x4::Inverse(pCamera->m_xmf4x4Projection));
	MouseFarPosition = Vector3::TransformCoord(MouseFarPosition, InvProj);
	MouseFarPosition.z = 1.0f;
	return Vector3::Normalize(MouseFarPosition);
}

void CScene::BlowUpEnermy(float fElapseTime, float fBombDistance)
{
	static bool collision_boss = false;
	fBombElapseTime += fElapseTime;

	float distance = fBombElapseTime * fBombSpeed;

	if (distance > fBombDistance) {
		fBombElapseTime = 0;
		bBomb = false;
		collision_boss = false;
		return;
	}

	XMFLOAT3 extents(distance, distance, distance);

	BoundingBox bb(xmf3BombPosition, extents);
	for (std::shared_ptr<CEnermy> & data : m_plEnermys) {
		ContainmentType containType = data->m_xmAABB.Contains(bb);					//������ �浹�� üũ
		if (containType == INTERSECTING)
			data->m_bBlowingUp = true;
	}

	if (m_pBoss) {
		ContainmentType containType = m_pBoss->m_xmAABB.Contains(bb);					//������ �浹�� üũ
		if (containType == INTERSECTING && !collision_boss) {
			collision_boss = true;
			m_pBoss->m_iLife-=5;
		}
	}

	
}

void CScene::ResponObject(float fElapsedTime)
{
	if (m_pPlayer->GetPosition().z > 855)
		return;


	std::random_device rd;
	std::default_random_engine dre(rd());
	std::uniform_real_distribution<float> ufr(-WALL_HALF_SIZE, WALL_HALF_SIZE);
	std::uniform_real_distribution<float> ufrRotaionAngle(0.0f, 360.0f);

	m_fEnermyResponTime -= fElapsedTime;

	std::uniform_int_distribution<size_t> randEnermyuid(CEnermy::Blue, CEnermy::Pink);
	size_t randEnermy = randEnermyuid(dre);
	if (m_fEnermyResponTime  < 0) {
		++m_iResponCnt;

		if (m_iResponCnt % 10) {
			m_fEnermyResponTime = m_fEnermyResponInitTime;
			CEnermy * Enermy = new CEnermy;
			Enermy->SetByType(randEnermy);
			Enermy->SetPosition(XMFLOAT3(ufr(dre), ufr(dre), m_pPlayer->GetPosition().z + 100));
			Enermy->SetMovingDirection(XMFLOAT3(ufr(dre), ufr(dre), ufr(dre)));
			Enermy->SetRotationAxis(XMFLOAT3(ufrRotaionAngle(dre), ufrRotaionAngle(dre), ufrRotaionAngle(dre)));
			Enermy->SetRotationSpeed(120);

			m_plEnermys.emplace_back(Enermy);
		}
		else {
			m_fEnermyResponTime = m_fEnermyResponInitTime;
			CEnermy * Enermy = new CEnermy;
			Enermy->SetByType(CEnermy::Red);
			Enermy->SetPosition(XMFLOAT3(ufr(dre), ufr(dre), m_pPlayer->GetPosition().z + 100));
			Enermy->SetMovingDirection(XMFLOAT3(ufr(dre), ufr(dre), ufr(dre)));
			Enermy->SetRotationAxis(XMFLOAT3(ufrRotaionAngle(dre), ufrRotaionAngle(dre), ufrRotaionAngle(dre)));
			Enermy->SetRotationSpeed(120);

			m_plEnermys.emplace_back(Enermy);
		}	
		//delete Enermy;
	}

}

void CScene::ResponBoss()
{
	CExplosiveObject::PrepareExplosion();
	CCubeMesh *pObjectCubeMesh = new CCubeMesh(8.0f, 8.0f, 8.0f);
	pObjectCubeMesh->SetAABB(XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(8.0f, 8.0f, 8.0f));

	CBoss * BossObject = new CBoss;
	BossObject->m_bActive = true;
	BossObject->SetMesh(pObjectCubeMesh);
	BossObject->SetColor(RGB(180, 82, 162));
	BossObject->SetPosition(XMFLOAT3(0, 0, 990));
	BossObject->SetMovingDirection(XMFLOAT3(0, 0, 0));
	BossObject->SetMovingSpeed(10);
	BossObject->SetRotationAxis(BossObject->GetLook());
	BossObject->SetRotationSpeed(120);

	m_pBoss = BossObject;
}

void CScene::RemoveEnermy()
{
	auto  Itor = m_plEnermys.begin(); 
	for (;Itor != m_plEnermys.end();) {
		std::shared_ptr<CEnermy> object = *Itor; 
		if (!object->m_bActive) {
			if (object->m_iType == CEnermy::Red)
				m_pPlayer->m_iBombNum++;
			Itor = m_plEnermys.erase(Itor);
		}
		else ++Itor;
	}
}

void CScene::RemoveBullet()
{
	RemoveObjects(m_plBullets);
}

