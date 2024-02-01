#pragma once
#include "..\mem\memory.h"
#include "..\math\vector.hpp"

//updates faster then aimpunch!
struct aimPunchCache
{
	DWORD64 count = 0;
	DWORD64 data = 0;
};

class CLocal
{
public:
	CLocal(uintptr_t localPlayerPawn);

	static CLocal GetLocalPawn();

	static uintptr_t GetLocalController();
	static view_matrix_t GetViewMatrix();
	static Vector2 AimPunchAngle(aimPunchCache AimPunch);
	static int GetWeaponGroup(std::string weaponName);

	int Health() const noexcept;
	int Team() const noexcept;
	Vector Position() const noexcept;
	int Ping() const noexcept;
	std::string  GetWeaponNameLocal() const noexcept;;
	Vector2 ViewAngle() const noexcept;
	bool Ammo() const noexcept;
	int ShotsFired() const noexcept;
	int Flags() const noexcept;
	aimPunchCache AimPunch() const noexcept;
	Vector2 EyePosition() const noexcept;

private:
	uintptr_t localPlayerPawn{};
};