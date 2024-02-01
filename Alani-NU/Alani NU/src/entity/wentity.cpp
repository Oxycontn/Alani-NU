#include "wentity.h"
#include "..\classes\global.hpp"

CWEntity::CWEntity(uintptr_t entityController)
{
	CWEntity::entityController = entityController;
}

CWEntity CWEntity::EntityController(uintptr_t entityController)
{
	return CWEntity(entityController);
}

int CWEntity::EntityOwner() const noexcept
{
	return driver.Read<int>(entityController + offset::m_hOwnerEntity);
}

uintptr_t CWEntity::PEntity() const noexcept
{
	return driver.Read<uintptr_t>(entityController + offset::m_pEntity);
}

uintptr_t CWEntity::DesignerNamePtr(uintptr_t pEntity)
{
	return driver.Read<uintptr_t>(pEntity + offset::m_designerName);
}

std::string CWEntity::ReadDesignerName(uintptr_t designerNameptr)
{
	std::string name;

	for (int i = 0; i < 260; ++i)
	{
		char letter = driver.Read<char>(designerNameptr + i);

		if (letter == NULL)
			break;

		name += letter;
	}

	return name;
}

uintptr_t CWEntity::Gamescene() const noexcept
{
	return driver.Read<uintptr_t>(entityController + offset::m_pGameSceneNode);
}

Vector CWEntity::EntityPosition(uintptr_t gameScene)
{
	return driver.Read<Vector>(gameScene + offset::m_vecAbsOrigin);
}