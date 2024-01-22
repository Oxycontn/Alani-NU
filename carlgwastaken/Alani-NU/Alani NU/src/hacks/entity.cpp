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
	auto ptr = Read<uintptr_t>(global.modules.client + offset::dwEntityList);

	if (ptr != 0)
		return ptr;
}

uintptr_t CEntity::GetPlayerController(uintptr_t entityList, uintptr_t localPlayer, int i)
{
	auto list1 = Read<uintptr_t>(entityList + (8 * (i & 0x7FFF) >> 9) + 16);

	if (!list1)
		return 0;
	
	auto pc = Read<uintptr_t>(list1 + 120 * (i & 0x1FF));

	if (pc == localPlayer)
		return 0;
	
	return pc;
}

CEntity* CEntity::PlayerController(uintptr_t playerController) {
	return new CEntity(playerController);
}

CEntity* CEntity::GetpCSPlayerPawn(uintptr_t entityList, uintptr_t playerController, int i)
{
	auto playerPawn = Read<uintptr_t>(playerController + offset::Pawn);

	auto list2 = Read<uintptr_t>(entityList + 0x8 * ((playerPawn & 0x7FFF) >> 9) + 16);

	auto pCSPlayerPawn = Read<uintptr_t>(list2 + 120 * (playerPawn & 0x1FF));

	return new CEntity(pCSPlayerPawn);
}

int CEntity::Health() const noexcept
{
	return Read<int>(entityAddress + offset::m_iHealth);
}

int CEntity::Team() const noexcept
{
	return Read<int>(entityAddress + offset::m_iTeamNum);
}

int CEntity::Armor() const noexcept
{
	return Read<int>(entityAddress + offset::m_ArmorValue);
}

bool CEntity::spottedState() const noexcept
{
	return Read<bool>(entityAddress + offset::m_entitySpottedState + 0x08);
}

uintptr_t CEntity::Gamescene() const noexcept
{
	return Read<uintptr_t>(entityAddress + offset::m_pGameSceneNode);
}

uintptr_t CEntity::Bonearray() const noexcept
{
	return Read<uintptr_t>(Gamescene() + 0x160 + 0x80);
}

Vector CEntity::Feet(uintptr_t bonearray) const noexcept
{
	return Read<Vector>(entityAddress + offset::Origin);
}

Vector CEntity::Head(uintptr_t bonearray) const noexcept
{
	return Read<Vector>(Bonearray() + 24 * 32);
}

Vector CEntity::Bone(int bone) const noexcept
{
	return Read<Vector>(Bonearray() + bone * 32);
}

std::string CEntity::GetWeaponName()
{
	uintptr_t clippingWeapon = Read<uintptr_t>(entityAddress + offset::m_pClippingWeapon);
	uintptr_t vWeaponDataPtr = Read<uintptr_t>(clippingWeapon + 0x360);
	uintptr_t weaponNameAddress = Read<uintptr_t>(vWeaponDataPtr + offset::m_szName);

	char outBuffer[MAX_PATH];
	//mem.ReadRaw(weaponNameAddress, outBuffer, sizeof(outBuffer));

	std::string name = std::string(outBuffer);

	return name;
}
 
std::string CEntity::GetPlayerName(uintptr_t playerController)
{
	uintptr_t playerNameAddress = Read<uintptr_t>(playerController + offset::m_sSanitizedPlayerName);

	char outBuffer[MAX_PATH];
	//mem.ReadRaw(playerNameAddress, outBuffer, sizeof(outBuffer));

	std::string name = std::string(outBuffer);

	return name;
}

std::string CEntity::GetWeaponNameLocal(uintptr_t pawn)
{
	uintptr_t clippingWeapon = Read<uintptr_t>(pawn + offset::m_pClippingWeapon);
	uintptr_t vWeaponDataPtr = Read<uintptr_t>(clippingWeapon + 0x360);
	uintptr_t weaponNameAddress = Read<uintptr_t>(vWeaponDataPtr + offset::m_szName);

	char outBuffer[MAX_PATH];
	//mem.ReadRaw(weaponNameAddress, outBuffer, sizeof(outBuffer));

	std::string name = std::string(outBuffer);

	return name;
}

std::string CEntity::ReadDesignerName(uintptr_t designerNameptr)
{
	char outBuffer[MAX_PATH];
	//mem.ReadRaw(designerNameptr, outBuffer, sizeof(outBuffer));

	std::string name = std::string(outBuffer);

	return name;
}