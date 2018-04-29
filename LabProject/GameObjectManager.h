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
	//오브젝트를 추가하는 함수
	void newBullet(CBullet *);
	void newEnermy(CEnermy *);
	void newBounusObject(CBonusObject *);

	//오브젝트를 제거하는 함수
	void deleteBullet(std::shared_ptr<CBullet>);
	void deleteEnermy(std::shared_ptr<CEnermy>);
	void deleteBonusObject(std::shared_ptr<CBonusObject>);

public:

	//리스트를 가져오는 함수
	std::list<std::shared_ptr<CGameObject>> & GetplGameObjects();
	std::list<std::shared_ptr<CBullet>> & GetplBullets();
	std::list<std::shared_ptr<CEnermy>> & GetplEnermys();
	std::list<std::shared_ptr<CBonusObject>> & GetplBonusObjects();
};

