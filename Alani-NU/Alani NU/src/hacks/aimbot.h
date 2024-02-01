#pragma once
#include <thread>
#include <string>
#include "..\math\vector.hpp"
#include "..\entity\entity.h"

class CAimbot
{
public:
	Vector boneAngle;
	bool showBoneAngle, aimbotRCS;

	void AimbotThread();
	void AimbotLoop();

	int AimbotKey(int weaponGroup);
	Vector AimbotBone(int weaponGroup, CEntity pCSPlayerPawn);
	bool AimbotEnable(int weaponGroup);
	float AimbotFov(int weaponGroup);
	bool AimbotVisable(int weaponGroup);
	bool AimbotAutoshoot(int weaponGroup);
	float AimbotSmooth(int weaponGroup);
	int AimbotDistance(int weaponGroup);
	int AimbotAutoSleep(int weaponGroup);
};

inline CAimbot aimbot;
