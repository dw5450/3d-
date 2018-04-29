#pragma once
#include "GameObject.h"
#include <list>
#include <memory>
class CGameObjectManager
{
public:
	CGameObjectManager();
	~CGameObjectManager();

private:
	std::list<std::shared_ptr<CGameObject>>		m_plGameObjects;
	std::list<std::shared_ptr<CBullet>>			m_plBullets;
	std::list<std::shared_ptr<CEnermy>>			m_plEnermys;
	std::list<std::shared_ptr<CBonusObject>>	m_plBonusObjects;

public:
	//������Ʈ�� �߰��ϴ� �Լ�
	void newBullet(CBullet *);
	void newEnermy(CEnermy *);
	void newBounusObject(CBonusObject *);

	//������Ʈ�� �����ϴ� �Լ�
	void deleteBullet(std::shared_ptr<CBullet>);
	void deleteEnermy(std::shared_ptr<CEnermy>);
	void deleteBonusObject(std::shared_ptr<CBonusObject>);

public:

	//����Ʈ�� �������� �Լ�
	std::list<std::shared_ptr<CGameObject>> & GetplGameObjects();
	std::list<std::shared_ptr<CBullet>> & GetplBullets();
	std::list<std::shared_ptr<CEnermy>> & GetplEnermys();
	std::list<std::shared_ptr<CBonusObject>> & GetplBonusObjects();
};

