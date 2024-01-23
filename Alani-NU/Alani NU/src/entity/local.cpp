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