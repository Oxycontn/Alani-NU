#include "local.h"
#include "..\classes\global.hpp"


CLocal::CLocal(uintptr_t localPlayerPawn)
{
	CLocal::localPlayerPawn = localPlayerPawn;
}

CLocal* CLocal::GetLocalPawn()
{
	uintptr_t localPlayerPawn = driver.Read<uintptr_t>(global.modules.client + offset::dwLocalPlayerPawn);

	return new CLocal(localPlayerPawn);
}

uintptr_t CLocal::GetLocalController()
{
	return driver.Read<uintptr_t>(global.modules.client + offset::dwLocalPlayerController);
}

view_matrix_t CLocal::GetViewMatrix()
{
	return driver.Read<view_matrix_t>(global.modules.client + offset::dwViewMatrix);
}

int CLocal::Health() const noexcept
{
	return driver.Read<int>(localPlayerPawn + offset::m_iHealth);
}

int CLocal::Team() const noexcept
{
	return driver.Read<int>(localPlayerPawn + offset::m_iTeamNum);
}

Vector CLocal::Position() const noexcept
{
	return driver.Read<Vector>(localPlayerPawn + offset::Origin);
}

int CLocal::Ping() const noexcept
{
	return driver.Read<int>(localPlayerPawn + offset::m_iPing);
}

std::string CLocal::GetWeaponNameLocal() const noexcept
{
	uintptr_t clippingWeapon = driver.Read<uintptr_t>(localPlayerPawn + offset::m_pClippingWeapon);
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

Vector2 CLocal::ViewAngle() const noexcept
{
	return driver.Read<Vector2>(localPlayerPawn + offset::v_angle);
}

bool CLocal::Ammo() const noexcept
{
	uintptr_t clippingWeapon = driver.Read<uintptr_t>(localPlayerPawn + offset::m_pClippingWeapon);

	return driver.Read<bool>(clippingWeapon + 0x1570);
}

int CLocal::ShotsFired() const noexcept
{
	return driver.Read<int>(localPlayerPawn + offset::m_iShotsFired);
}

int CLocal::Flags() const noexcept
{
	return driver.Read<int32_t>(localPlayerPawn + offset::m_fFlags);
}

aimPunchCache CLocal::AimPunch() const noexcept
{
	return driver.Read<aimPunchCache>(localPlayerPawn + offset::m_aimPunchCache);
}

Vector2 CLocal::AimPunchAngle(aimPunchCache AimPunch)
{
	return driver.Read<Vector2>(AimPunch.data + (AimPunch.count - 1) * sizeof(Vector));
}

Vector2 CLocal::EyePosition() const noexcept
{
	return driver.Read<Vector2>(localPlayerPawn + 0x1518);
}

int CLocal::GetWeaponGroup(std::string weaponName)
{
	int weaponGroup;

	//ar
	if (weaponName == "weapon_ak47" || weaponName == "weapon_aug" || weaponName == "weapon_famas" || weaponName == "weapon_galilar" || weaponName == "weapon_m4a1_silencer" || weaponName == "weapon_m4a1" || weaponName == "weapon_sg553")
		weaponGroup = 1;

	//shotgun
	if (weaponName == "weapon_mag7" || weaponName == "weapon_sawedoff" || weaponName == "weapon_nova" || weaponName == "weapon_xm1014")
		weaponGroup = 2;

	//pistols
	if (weaponName == "weapon_cz75a" || weaponName == "weapon_deagle" || weaponName == "weapon_elite" || weaponName == "weapon_fiveseven" || weaponName == "weapon_glock" || weaponName == "weapon_hkp2000" || weaponName == "weapon_p250" || weaponName == "weapon_revolver" || weaponName == "weapon_tec9" || weaponName == "weapon_usp_silencer")
		weaponGroup = 3;

	//snipers
	if (weaponName == "weapon_awp" || weaponName == "weapon_g3sg1" || weaponName == "weapon_scar20" || weaponName == "weapon_ssg08")
		weaponGroup = 4;

	//smgs
	if (weaponName == "weapon_mac10" || weaponName == "weapon_mp7" || weaponName == "weapon_mp9" || weaponName == "weapon_mp5sd" || weaponName == "weapon_ump45" || weaponName == "weapon_bizon" || weaponName == "weapon_p90")
		weaponGroup = 5;

	return weaponGroup;
}