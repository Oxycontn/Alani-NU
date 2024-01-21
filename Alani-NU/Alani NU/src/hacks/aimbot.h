#pragma once
#include <thread>
#include <string>
#include "..\math\vector.hpp"

class CAimbot
{
public:
	Vector boneAngle;
	bool showBoneAngle, aimbotRCS;

	void AimbotThread();
	void AimbotLoop();
	int AimbotKey(std::string weaponName);
	Vector AimbotBone(std::string weaponName, uintptr_t boneArray);
	bool AimbotEnable(std::string weaponName);
	float AimbotFov(std::string weaponName);
	bool AimbotVisable(std::string weaponName);
	bool AimbotAutoshoot(std::string weaponName);
	float AimbotSmooth(std::string weaponName);
	int AimbotDistance(std::string weaponName);
	int AimbotAutoSleep(std::string weaponName);
};

inline CAimbot aimbot;
