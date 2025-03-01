#pragma warning( disable : 4244 4312 4305 )

#include "entity.h"

#include "../classes/global.hpp"
#include "..\mem\memory.h"

CEntity::CEntity(uintptr_t entityAddress)
{
	CEntity::entityAddress = entityAddress;
}

uintptr_t CEntity::GetEntityList()
{
	auto ptr = driver.Read<uintptr_t>(global.modules.client + offset::dwEntityList);

	if (ptr != 0)
		return ptr;
}

uintptr_t CEntity::GetPlayerController(uintptr_t entityList, uintptr_t localPlayer, int i)
{
	auto list1 = driver.Read<uintptr_t>(entityList + (8 * (i & 0x7FFF) >> 9) + 16);

	if (!list1)
		return 0;

	auto pc = driver.Read<uintptr_t>(list1 + 120 * (i & 0x1FF));

	if (pc == localPlayer)
		return 0;

	return pc;
}

CEntity CEntity::GetpCSPlayerPawn(uintptr_t entityList, uintptr_t playerController, int i, uintptr_t localPlayerPawn)
{
	auto playerPawn = driver.Read<uintptr_t>(playerController + offset::Pawn);

	auto list2 = driver.Read<uintptr_t>(entityList + 0x8 * ((playerPawn & 0x7FFF) >> 9) + 16);

	auto pCSPlayerPawn = driver.Read<uintptr_t>(list2 + 120 * (playerPawn & 0x1FF));

	if (pCSPlayerPawn == localPlayerPawn)
		global.misc.localPlayerPawnIndex = i;

	return CEntity(pCSPlayerPawn);
}

int CEntity::Health() const noexcept
{
	return driver.Read<int>(entityAddress + offset::m_iHealth);
}

int CEntity::Team() const noexcept
{
	return driver.Read<int>(entityAddress + offset::m_iTeamNum);
}

int CEntity::Armor() const noexcept
{
	return driver.Read<int>(entityAddress + offset::m_ArmorValue);
}

bool CEntity::spottedState() const noexcept
{
	uintptr_t spottedState = driver.Read<uintptr_t>(entityAddress + offset::m_entitySpottedState + 0x08);

	return spottedState & (1 << global.misc.localPlayerPawnIndex - 1);
}

uintptr_t CEntity::Gamescene() const noexcept
{
	return driver.Read<uintptr_t>(entityAddress + offset::m_pGameSceneNode);
}

uintptr_t CEntity::Bonearray() const noexcept
{
	return driver.Read<uintptr_t>(Gamescene() + 0x160 + 0x80);
}

Vector CEntity::Feet() const noexcept
{
	return driver.Read<Vector>(entityAddress + offset::Origin);
}

Vector CEntity::Head(uintptr_t bonearray) const noexcept
{
	return driver.Read<Vector>(Bonearray() + 24 * 32);
}

Vector CEntity::Bone(int bone) const noexcept
{
	return driver.Read<Vector>(Bonearray() + bone * 32);
}

std::string CEntity::GetWeaponName()
{
	uintptr_t clippingWeapon = driver.Read<uintptr_t>(entityAddress + offset::m_pClippingWeapon);
	uintptr_t vWeaponDataPtr = driver.Read<uintptr_t>(clippingWeapon + 0x360);
	uintptr_t weaponNameAddress = driver.Read<uintptr_t>(vWeaponDataPtr + offset::m_szName);

	std::string name;

	for (int i = 0; i < 260; ++i)
	{
		char letter = driver.Read<char>(weaponNameAddress + i);

		if (letter == NULL)
			break;

		name += letter;
	}

	return name;
}

std::string CEntity::GetPlayerName(uintptr_t playerController)
{
	uintptr_t playerNameAddress = driver.Read<uintptr_t>(playerController + offset::m_sSanitizedPlayerName);

	std::string name;

	for (int i = 0; i < 260; ++i)
	{
		char letter = driver.Read<char>(playerNameAddress + i);

		if (letter == NULL)
			break;

		name += letter;
	}

	return name;
}