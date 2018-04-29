#include "stdafx.h"
#include "GameObjectManager.h"


CGameObjectManager::CGameObjectManager()
{
}


CGameObjectManager::~CGameObjectManager()
{
}

void CGameObjectManager::newBullet(CBullet * pbullet)
{
	m_plGameObjects.emplace_back(pbullet);
	m_plBullets.emplace_back(pbullet);
}

void CGameObjectManager::newEnermy(CEnermy * pEnermy)
{
	m_plGameObjects.emplace_back(pEnermy);
	m_plEnermys.emplace_back(pEnermy);
}

void CGameObjectManager::newBounusObject(CBonusObject * pBonusObject)
{
	m_plGameObjects.emplace_back(pBonusObject);
	m_plBonusObjects.emplace_back(pBonusObject);
}




void CGameObjectManager::deleteBullet(std::shared_ptr<CBullet> pBullet)
{
	m_plGameObjects.remove(pBullet);
	m_plBullets.remove(pBullet);
}

void CGameObjectManager::deleteEnermy(std::shared_ptr<CEnermy> pEnermy)
{
	m_plGameObjects.remove(pEnermy);
	m_plEnermys.remove(pEnermy);
}

void CGameObjectManager::deleteBonusObject(std::shared_ptr<CBonusObject> pBonusObject)
{
	m_plGameObjects.remove(pBonusObject);
	m_plBonusObjects.remove(pBonusObject);
}




std::list<std::shared_ptr<CGameObject>> & CGameObjectManager::GetplGameObjects()
{
	return m_plGameObjects;
}

std::list<std::shared_ptr<CBullet>> & CGameObjectManager::GetplBullets()
{
	return m_plBullets;
}

std::list<std::shared_ptr<CEnermy>> & CGameObjectManager::GetplEnermys()
{
	return m_plEnermys;
}

std::list<std::shared_ptr<CBonusObject>> & CGameObjectManager::GetplBonusObjects()
{
	return m_plBonusObjects;
}


