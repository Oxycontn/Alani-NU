#pragma warning( disable : 4244 4312 4305 )

#include "aimbot.h"
#include "..\classes\global.hpp"
#include "..\entity\entity.h"
#include "esp.h"
#include "..\classes\bone.hpp"
#include "rcs.h"
#include "..\mem\memory.h"
#include "..\entity\local.h"

void CAimbot::AimbotThread()
{
    while (true)
    {
		std::this_thread::sleep_for(std::chrono::milliseconds(10));

		if (global.threads.stopAimbot)
			std::terminate();

        AimbotLoop();

		rcs.RcsLoop();
    }
}

void CAimbot::AimbotLoop()
{
	uintptr_t entityList = CEntity::GetEntityList();
	
	std::string weaponName = global.localPlayer.weaponName;
	int weaponGroup = CLocal::GetWeaponGroup(weaponName);

	bool aimbotEnable = AimbotEnable(weaponGroup);

	if (aimbotEnable)
	{
		for (int i = 1; i < 64; ++i)
		{
			if (overlay.RenderMenu)
				continue;

			//entity varibles
			auto playerController = CEntity::GetPlayerController(entityList, global.localPlayer.localPlayerController, i);

			auto pCSPlayerPawn = CEntity::GetpCSPlayerPawn(entityList, playerController, i);

			int health = pCSPlayerPawn->Health();

			if (health <= 0 || health > 100)
				continue;

			int playerTeam = pCSPlayerPawn->Team();

			if (playerTeam == global.localPlayer.team)
				continue;

			bool spottedState = pCSPlayerPawn->spottedState();
			uintptr_t bonearray = pCSPlayerPawn->Bonearray();
			Vector playerPos = pCSPlayerPawn->Feet(bonearray);

			//local player varibles
			view_matrix_t viewMatrix = CLocal::GetViewMatrix();

			auto localPlayerPawn = CLocal::GetLocalPawn();
			Vector localPos = localPlayerPawn->Position();

			Vector2 viewAngle = localPlayerPawn->ViewAngle();
			bool ammo = localPlayerPawn->Ammo();

			int shotsFired = localPlayerPawn->ShotsFired();
			auto fFlags = localPlayerPawn->Flags();

			Vector2 eyePos = localPlayerPawn->EyePosition();
			Vector bonePos = AimbotBone(weaponGroup, bonearray);

			//settings
			float aimbotFov = AimbotFov(weaponGroup);

			bool aimbotVisable = AimbotVisable(weaponGroup);
			int vKey = AimbotKey(weaponGroup);

			bool aimbotAuto = AimbotAutoshoot(weaponGroup);
			float aimbotSmooth = AimbotSmooth(weaponGroup);

			float aimbotDistance = AimbotDistance(weaponGroup);
			int aimbotSleep = AimbotAutoSleep(weaponGroup);

			//calculations
			Vector2 aimPos = Vector2::AimbotAimCalculation(bonePos, localPos, viewAngle, fFlags);
			float pitch = aimPos.x;
			float yaw = aimPos.y;

			float distance = localPos.CalculateDistance(playerPos);
			float fov = Vector2::AimbotFovCalculation(aimPos, distance, aimbotFov);

			if (aimbotFov >= fov && aimbotDistance >= distance && ammo != 0)
			{
				showBoneAngle = true;
				aimbotRCS = true;

				Vector::WTS(viewMatrix, bonePos, boneAngle);

				if (GetAsyncKeyState(vKey))
				{
					pitch += viewAngle.x;
					yaw += viewAngle.y;

					if (global.features.rcsenable)
					{
						if (weaponGroup == 1 || weaponGroup == 5)
						{
							if (shotsFired != 0)
							{
								aimPunchCache aimpunchCache = localPlayerPawn->AimPunch();

								Vector2 aimPunch = CLocal::AimPunchAngle(aimpunchCache);

								if (!aimPunch.x && !aimPunch.y)
									return;

								pitch -= aimPunch.x * global.features.rcsscaleX;
								yaw -= aimPunch.y * global.features.rcsscaleY;

								rcs.oldAngle.x = aimPunch.x * global.features.rcsscaleX;
								rcs.oldAngle.y = aimPunch.y * global.features.rcsscaleY;
								rcs.oldAngle.z = 0.0f;
							}
							else
							{
								rcs.oldAngle.x = 0.0f;
								rcs.oldAngle.y = 0.0f;
								rcs.oldAngle.z = 0.0f;
							}
						}
					}

					aimbotSmooth *= 150;

					Vector2 screenOffset = Vector2::AngleToScreenOffset(yaw, pitch, viewAngle.y, viewAngle.x, aimbotSmooth);

					if (aimbotVisable)
					{
						if (spottedState)
						{
							mouse_event(MOUSEEVENTF_MOVE, static_cast<DWORD>(screenOffset.x), static_cast<DWORD>(screenOffset.y), 0, 0);
							if (aimbotAuto)
							{
								mouse_event(MOUSEEVENTF_LEFTDOWN, 0, 0, 0, 0);
								mouse_event(MOUSEEVENTF_LEFTUP, 0, 0, 0, 0);
								Sleep(aimbotSleep);
							}
						}
					}
					else
					{
						mouse_event(MOUSEEVENTF_MOVE, static_cast<DWORD>(screenOffset.x), static_cast<DWORD>(screenOffset.y), 0, 0);
						if (aimbotAuto)
						{
							mouse_event(MOUSEEVENTF_LEFTDOWN, 0, 0, 0, 0);
							mouse_event(MOUSEEVENTF_LEFTUP, 0, 0, 0, 0);
							Sleep(aimbotSleep);
						}
					}

				}

				break;
			}
			else
			{
				aimbotRCS = false;
				showBoneAngle = false;
			}
		}
	}
}

/*
weaponGroup identifier : 
1 = AR
2 = Shotguns
3 = Pistols
4 = Snipers
5 = Smgs
*/

int CAimbot::AimbotKey(int weaponGroup)
{
	int vKey{};

	switch (weaponGroup)
	{
	case 1:
		switch (global.features.ARaimbotcombokey)
		{
		case 0:
			vKey = VK_XBUTTON2;
			break;

		case 1:
			vKey = VK_LSHIFT;
			break;

		case 2:
			vKey = VK_LCONTROL;
			break;

		case 3:
			vKey = VK_LBUTTON;
			break;
		}
		break;

	case 2:
		switch (global.features.SGaimbotcombokey)
		{
		case 0:
			vKey = VK_XBUTTON2;
			break;

		case 1:
			vKey = VK_LSHIFT;
			break;

		case 2:
			vKey = VK_LCONTROL;
			break;

		case 3:
			vKey = VK_LBUTTON;
			break;
		}
		break;

	case 3:
		switch (global.features.PSaimbotcombokey)
		{
		case 0:
			vKey = VK_XBUTTON2;
			break;

		case 1:
			vKey = VK_LSHIFT;
			break;

		case 2:
			vKey = VK_LCONTROL;
			break;

		case 3:
			vKey = VK_LBUTTON;
			break;
		}
		break;

	case 4:
		switch (global.features.SRaimbotcombokey)
		{
		case 0:
			vKey = VK_XBUTTON2;
			break;

		case 1:
			vKey = VK_LSHIFT;
			break;

		case 2:
			vKey = VK_LCONTROL;
			break;

		case 3:
			vKey = VK_LBUTTON;
			break;
		}
		break;

	case 5:
		switch (global.features.SMGaimbotcombokey)
		{
		case 0:
			vKey = VK_XBUTTON2;
			break;

		case 1:
			vKey = VK_LSHIFT;
			break;

		case 2:
			vKey = VK_LCONTROL;
			break;

		case 3:
			vKey = VK_LBUTTON;
			break;
		}
		break;
	}

	return vKey;
}

Vector CAimbot::AimbotBone(int weaponGroup, uintptr_t bonearray)
{
	Vector bone{};

	Vector boneHead = driver.Read<Vector>(bonearray + bones::head * 32);
	Vector boneNeck = driver.Read<Vector>(bonearray + bones::neck * 32);
	Vector boneSpine = driver.Read<Vector>(bonearray + bones::spine * 32);
	Vector boneCock = driver.Read<Vector>(bonearray + bones::cock * 32);

	switch (weaponGroup)
	{
	case 1:
		switch (global.features.ARaimbotcombobone)
		{
		case 0:
			bone = boneHead;
			break;

		case 1:
			bone = boneNeck;
			break;

		case 2:
			bone = boneSpine;
			break;

		case 3:
			bone = boneCock;
			break;
		}
		break;

	case 2:
		switch (global.features.SGaimbotcombobone)
		{
		case 0:
			bone = boneHead;
			break;

		case 1:
			bone = boneNeck;
			break;

		case 2:
			bone = boneSpine;
			break;

		case 3:
			bone = boneCock;
			break;
		}
		break;

	case 3:
		switch (global.features.PSaimbotcombobone)
		{
		case 0:
			bone = boneHead;
			break;

		case 1:
			bone = boneNeck;
			break;

		case 2:
			bone = boneSpine;
			break;

		case 3:
			bone = boneCock;
			break;
		}
		break;

	case 4:
		switch (global.features.SRaimbotcombobone)
		{
		case 0:
			bone = boneHead;
			break;

		case 1:
			bone = boneNeck;
			break;

		case 2:
			bone = boneSpine;
			break;

		case 3:
			bone = boneCock;
			break;
		}
		break;

	case 5:
		switch (global.features.SMGaimbotcombobone)
		{
		case 0:
			bone = boneHead;
			break;

		case 1:
			bone = boneNeck;
			break;

		case 2:
			bone = boneSpine;
			break;

		case 3:
			bone = boneCock;
			break;
		}
		break;
	}

	//return bone
	return bone;
}

bool CAimbot::AimbotEnable(int weaponGroup)
{
	bool enable = false;

	switch (weaponGroup)
	{
	case 1:
		if (global.features.ARaimbotenable)
			enable = true;
		break;

	case 2:
		if (global.features.SGaimbotenable)
			enable = true;
		break;

	case 3:
		if (global.features.PSaimbotenable)
			enable = true;
		break;

	case 4:
		if (global.features.SRaimbotenable)
			enable = true;
		break;

	case 5:
		if (global.features.SMGaimbotenable)
			enable = true;
		break;
	}

	return enable;
}

float CAimbot::AimbotFov(int weaponGroup)
{
	float fov{};

	switch (weaponGroup)
	{
	case 1:
		fov = global.features.ARaimbotfov;
		break;

	case 2:
		fov = global.features.SGaimbotfov;
		break;

	case 3:
		fov = global.features.PSaimbotfov;
		break;

	case 4:
		fov = global.features.SRaimbotfov;
		break;

	case 5:
		fov = global.features.SMGaimbotfov;
		break;
	}

	return fov;
}

bool CAimbot::AimbotVisable(int weaponGroup)
{
	bool visable = false;

	switch (weaponGroup)
	{
	case 1:
		if (global.features.ARaimbotvisable)
			visable = true;
		break;

	case 2:
		if (global.features.SGaimbotvisable)
			visable = true;
		break;

	case 3:
		if (global.features.PSaimbotvisable)
			visable = true;
		break;

	case 4:
		if (global.features.SRaimbotvisable)
			visable = true;
		break;

	case 5:
		if (global.features.SMGaimbotvisable)
			visable = true;
		break;
	}

	return visable;
}

bool CAimbot::AimbotAutoshoot(int weaponGroup)
{
	bool autoShoot = false;

	switch (weaponGroup)
	{
	case 1:
		if (global.features.ARaimbotautoshot)
			autoShoot = true;
		break;

	case 2:
		if (global.features.SGaimbotautoshot)
			autoShoot = true;
		break;

	case 3:
		if (global.features.PSaimbotautoshot)
			autoShoot = true;
		break;

	case 4:
		if (global.features.SRaimbotautoshot)
			autoShoot = true;
		break;

	case 5:
		if (global.features.SMGaimbotautoshot)
			autoShoot = true;
		break;
	}

	return autoShoot;
}

float CAimbot::AimbotSmooth(int weaponGroup)
{
	float smooth{};

	switch (weaponGroup)
	{
	case 1:
		smooth = global.features.ARaimbotsmooth;
		break;

	case 2:
		smooth = global.features.SGaimbotsmooth;
		break;

	case 3:
		smooth = global.features.PSaimbotsmooth;
		break;

	case 4:
		smooth = global.features.SRaimbotsmooth;
		break;

	case 5:
		smooth = global.features.SMGaimbotsmooth;
		break;
	}

	return smooth;
}

int CAimbot::AimbotDistance(int weaponGroup)
{
	int distance{};

	switch (weaponGroup)
	{
	case 1:
		distance = global.features.ARaimbotdistance;
		break;

	case 2:
		distance = global.features.SGaimbotdistance;
		break;

	case 3:
		distance = global.features.PSaimbotdistance;
		break;

	case 41:
		distance = global.features.SRaimbotdistance;
		break;

	case 5:
		distance = global.features.SMGaimbotdistance;
		break;
	}

	return distance;
}

int CAimbot::AimbotAutoSleep(int weaponGroup)
{
	int sleep{};

	switch (weaponGroup)
	{
	case 1:
		sleep = global.features.ARautosleep;
		break;

	case 2:
		sleep = global.features.SGautosleep;
		break;

	case 3:
		sleep = global.features.PSautosleep;
		break;

	case 4:
		sleep = global.features.SRautosleep;
		break;

	case 5:
		sleep = global.features.SMGautosleep;
		break;
	}

	return sleep;
}