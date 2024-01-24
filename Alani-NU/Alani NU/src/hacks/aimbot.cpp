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
		std::this_thread::sleep_for(std::chrono::milliseconds(1));

		if (global.threads.stopAimbot)
			std::terminate();

        AimbotLoop();

		rcs.RcsLoop();
    }
}

void CAimbot::AimbotLoop()
{
	uintptr_t entityList = CEntity::GetEntityList();

	for (int i = 1; i < 64; ++i)
	{
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
		view_matrix_t viewMatrix = global.localPlayer.vm;
		std::string weaponName = global.localPlayer.weaponName;

		auto localPlayerPawn = CLocal::GetLocalPawn();
		Vector localPos = localPlayerPawn->Position();

		Vector2 viewAngle = localPlayerPawn->ViewAngle();
		bool ammo = localPlayerPawn->Ammo();

		int shotsFired = localPlayerPawn->ShotsFired();
		auto fFlags = localPlayerPawn->Flags();

		Vector2 eyePos = localPlayerPawn->EyePosition();
		Vector bonePos = AimbotBone(weaponName, bonearray);

		//calculations
		Vector2 aimPos = Vector2::AimbotAimCalculation(bonePos, localPos, viewAngle, fFlags);
		float pitch = aimPos.x;
		float yaw = aimPos.y;

		float distance = localPos.CalculateDistance(playerPos);
		float fov = Vector2::AimbotFovCalculation(aimPos, eyePos);

		//settings
		bool aimbotEnable = AimbotEnable(weaponName);
		float aimbotFov = AimbotFov(weaponName);

		bool aimbotVisable = AimbotVisable(weaponName);
		int vKey = AimbotKey(weaponName);

		bool aimbotAuto = AimbotAutoshoot(weaponName);
		float aimbotSmooth = AimbotSmooth(weaponName);

		float aimbotDistance = AimbotDistance(weaponName);
		int aimbotSleep = AimbotAutoSleep(weaponName);

		if (aimbotEnable)
		{
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
						if (weaponName == "weapon_ak47" || weaponName == "weapon_aug" || weaponName == "weapon_famas" || weaponName == "weapon_galilar" || weaponName == "weapon_m4a1_silencer" || weaponName == "weapon_m4a1" || weaponName == "weapon_sg553" || weaponName == "weapon_mac10" || weaponName == "weapon_mp7" || weaponName == "weapon_mp9" || weaponName == "weapon_mp5sd" || weaponName == "weapon_ump45" || weaponName == "weapon_bizon" || weaponName == "weapon_p90")
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

int CAimbot::AimbotKey(std::string weaponName)
{
	int vKey{};

	//AR
	if (weaponName == "weapon_ak47" || weaponName == "weapon_aug" || weaponName == "weapon_famas" || weaponName == "weapon_galilar" || weaponName == "weapon_m4a1_silencer" || weaponName == "weapon_m4a1" || weaponName == "weapon_sg553")
	{
		if (global.features.ARaimbotcombokey == 0)
			vKey = VK_XBUTTON2;
		else if (global.features.ARaimbotcombokey == 1)
			vKey = VK_LSHIFT;
		else if (global.features.ARaimbotcombokey == 2)
			vKey = VK_LCONTROL;
		else if (global.features.ARaimbotcombokey == 3)
			vKey = VK_LBUTTON;
	}

	//SG
	if (weaponName == "weapon_mag7" || weaponName == "weapon_sawedoff" || weaponName == "weapon_nova" || weaponName == "weapon_xm1014")
	{
		if (global.features.SGaimbotcombokey == 0)
			vKey = VK_XBUTTON2;
		else if (global.features.SGaimbotcombokey == 1)
			vKey = VK_LSHIFT;
		else if (global.features.SGaimbotcombokey == 2)
			vKey = VK_LCONTROL;
		else if (global.features.SGaimbotcombokey == 3)
			vKey = VK_LBUTTON;
	}

	//Pistols
	if (weaponName == "weapon_cz75a" || weaponName == "weapon_deagle" || weaponName == "weapon_elite" || weaponName == "weapon_fiveseven" || weaponName == "weapon_glock" || weaponName == "weapon_hkp2000" || weaponName == "weapon_p250" || weaponName == "weapon_revolver" || weaponName == "weapon_tec9" || weaponName == "weapon_usp_silencer")
	{
		if (global.features.PSaimbotcombokey == 0)
			vKey = VK_XBUTTON2;
		else if (global.features.PSaimbotcombokey == 1)
			vKey = VK_LSHIFT;
		else if (global.features.PSaimbotcombokey == 2)
			vKey = VK_LCONTROL;
		else if (global.features.PSaimbotcombokey == 3)
			vKey = VK_LBUTTON;
	}

	//SR
	if (weaponName == "weapon_awp" || weaponName == "weapon_g3sg1" || weaponName == "weapon_scar20" || weaponName == "weapon_ssg08")
	{
		if (global.features.SRaimbotcombokey == 0)
			vKey = VK_XBUTTON2;
		else if (global.features.SRaimbotcombokey == 1)
			vKey = VK_LSHIFT;
		else if (global.features.SRaimbotcombokey == 2)
			vKey = VK_LCONTROL;
		else if (global.features.SRaimbotcombokey == 3)
			vKey = VK_LBUTTON;
	}

	//SMG
	if (weaponName == "weapon_mac10" || weaponName == "weapon_mp7" || weaponName == "weapon_mp9" || weaponName == "weapon_mp5sd" || weaponName == "weapon_ump45" || weaponName == "weapon_bizon" || weaponName == "weapon_p90")
	{
		if (global.features.SMGaimbotcombokey == 0)
			vKey = VK_XBUTTON2;
		else if (global.features.SMGaimbotcombokey == 1)
			vKey = VK_LSHIFT;
		else if (global.features.SMGaimbotcombokey == 2)
			vKey = VK_LCONTROL;
		else if (global.features.SMGaimbotcombokey == 3)
			vKey = VK_LBUTTON;
	}

	return vKey;
}

Vector CAimbot::AimbotBone(std::string weaponName, uintptr_t bonearray)
{
	Vector bone;

	Vector boneHead = driver.Read<Vector>(bonearray + bones::head * 32);
	Vector boneNeck = driver.Read<Vector>(bonearray + bones::neck * 32);
	Vector boneSpine = driver.Read<Vector>(bonearray + bones::spine * 32);
	Vector boneCock = driver.Read<Vector>(bonearray + bones::cock * 32);

	//AR
	if (weaponName == "weapon_ak47" || weaponName == "weapon_aug" || weaponName == "weapon_famas" || weaponName == "weapon_galilar" || weaponName == "weapon_m4a1_silencer" || weaponName == "weapon_m4a1" || weaponName == "weapon_sg553")
	{
		if (global.features.ARaimbotcombobone == 0)
			bone = boneHead;
		else if (global.features.ARaimbotcombobone == 1)
			bone = boneNeck;
		else if (global.features.ARaimbotcombobone == 2)
			bone = boneSpine;
		else if (global.features.ARaimbotcombobone == 3)
			bone = boneCock;
	}

	//SG
	if (weaponName == "weapon_mag7" || weaponName == "weapon_sawedoff" || weaponName == "weapon_nova" || weaponName == "weapon_xm1014")
	{
		if (global.features.SGaimbotcombobone == 0)
			bone = boneHead;
		else if (global.features.SGaimbotcombobone == 1)
			bone = boneNeck;
		else if (global.features.SGaimbotcombobone == 2)
			bone = boneSpine;
		else if (global.features.SGaimbotcombobone == 3)
			bone = boneCock;
	}

	//Pistols
	if (weaponName == "weapon_cz75a" || weaponName == "weapon_deagle" || weaponName == "weapon_elite" || weaponName == "weapon_fiveseven" || weaponName == "weapon_glock" || weaponName == "weapon_hkp2000" || weaponName == "weapon_p250" || weaponName == "weapon_revolver" || weaponName == "weapon_tec9" || weaponName == "weapon_usp_silencer")
	{
		if (global.features.PSaimbotcombobone == 0)
			bone = boneHead;
		else if (global.features.PSaimbotcombobone == 1)
			bone = boneNeck;
		else if (global.features.PSaimbotcombobone == 2)
			bone = boneSpine;
		else if (global.features.PSaimbotcombobone == 3)
			bone = boneCock;
	}

	//SR
	if (weaponName == "weapon_awp" || weaponName == "weapon_g3sg1" || weaponName == "weapon_scar20" || weaponName == "weapon_ssg08")
	{
		if (global.features.SRaimbotcombobone == 0)
			bone = boneHead;
		else if (global.features.SRaimbotcombobone == 1)
			bone = boneNeck;
		else if (global.features.SRaimbotcombobone == 2)
			bone = boneSpine;
		else if (global.features.SRaimbotcombobone == 3)
			bone = boneCock;
	}

	//SMG
	if (weaponName == "weapon_mac10" || weaponName == "weapon_mp7" || weaponName == "weapon_mp9" || weaponName == "weapon_mp5sd" || weaponName == "weapon_ump45" || weaponName == "weapon_bizon" || weaponName == "weapon_p90")
	{
		if (global.features.SMGaimbotcombobone == 0)
			bone = boneHead;
		else if (global.features.SMGaimbotcombobone == 1)
			bone = boneNeck;
		else if (global.features.SMGaimbotcombobone == 2)
			bone = boneSpine;
		else if (global.features.SMGaimbotcombobone == 3)
			bone = boneCock;
	}

	//return bone
	return bone;
}

bool CAimbot::AimbotEnable(std::string weaponName)
{
	bool enable = false;

	//AR
	if (weaponName == "weapon_ak47" || weaponName == "weapon_aug" || weaponName == "weapon_famas" || weaponName == "weapon_galilar" || weaponName == "weapon_m4a1_silencer" || weaponName == "weapon_m4a1" || weaponName == "weapon_sg553")
		if (global.features.ARaimbotenable)
			enable = true;

	//SG
	if (weaponName == "weapon_mag7" || weaponName == "weapon_sawedoff" || weaponName == "weapon_nova" || weaponName == "weapon_xm1014")
		if (global.features.SGaimbotenable)
			enable = true;

	//Pistols
	if (weaponName == "weapon_cz75a" || weaponName == "weapon_deagle" || weaponName == "weapon_elite" || weaponName == "weapon_fiveseven" || weaponName == "weapon_glock" || weaponName == "weapon_hkp2000" || weaponName == "weapon_p250" || weaponName == "weapon_revolver" || weaponName == "weapon_tec9" || weaponName == "weapon_usp_silencer")
		if (global.features.PSaimbotenable)
			enable = true;

	//SR
	if (weaponName == "weapon_awp" || weaponName == "weapon_g3sg1" || weaponName == "weapon_scar20" || weaponName == "weapon_ssg08")
		if (global.features.SRaimbotenable)
			enable = true;

	//SMG
	if (weaponName == "weapon_mac10" || weaponName == "weapon_mp7" || weaponName == "weapon_mp9" || weaponName == "weapon_mp5sd" || weaponName == "weapon_ump45" || weaponName == "weapon_bizon" || weaponName == "weapon_p90")
		if (global.features.SMGaimbotenable)
			enable = true;

	return enable;
}

float CAimbot::AimbotFov(std::string weaponName)
{
	float fov = 0;

	//AR
	if (weaponName == "weapon_ak47" || weaponName == "weapon_aug" || weaponName == "weapon_famas" || weaponName == "weapon_galilar" || weaponName == "weapon_m4a1_silencer" || weaponName == "weapon_m4a1" || weaponName == "weapon_sg553")
		fov = global.features.ARaimbotfov;

	//SG
	if (weaponName == "weapon_mag7" || weaponName == "weapon_sawedoff" || weaponName == "weapon_nova" || weaponName == "weapon_xm1014")
		fov = global.features.SGaimbotfov;

	//Pistols
	if (weaponName == "weapon_cz75a" || weaponName == "weapon_deagle" || weaponName == "weapon_elite" || weaponName == "weapon_fiveseven" || weaponName == "weapon_glock" || weaponName == "weapon_hkp2000" || weaponName == "weapon_p250" || weaponName == "weapon_revolver" || weaponName == "weapon_tec9" || weaponName == "weapon_usp_silencer")
		fov = global.features.PSaimbotfov;

	//SR
	if (weaponName == "weapon_awp" || weaponName == "weapon_g3sg1" || weaponName == "weapon_scar20" || weaponName == "weapon_ssg08")
		fov = global.features.SRaimbotfov;

	//SMG
	if (weaponName == "weapon_mac10" || weaponName == "weapon_mp7" || weaponName == "weapon_mp9" || weaponName == "weapon_mp5sd" || weaponName == "weapon_ump45" || weaponName == "weapon_bizon" || weaponName == "weapon_p90")
		fov = global.features.SMGaimbotfov;

	return fov;
}

bool CAimbot::AimbotVisable(std::string weaponName)
{
	bool visable = false;

	//AR
	if (weaponName == "weapon_ak47" || weaponName == "weapon_aug" || weaponName == "weapon_famas" || weaponName == "weapon_galilar" || weaponName == "weapon_m4a1_silencer" || weaponName == "weapon_m4a1" || weaponName == "weapon_sg553")
		if (global.features.ARaimbotvisable)
			visable = true;

	//SG
	if (weaponName == "weapon_mag7" || weaponName == "weapon_sawedoff" || weaponName == "weapon_nova" || weaponName == "weapon_xm1014")
		if (global.features.SGaimbotvisable)
			visable = true;

	//Pistols
	if (weaponName == "weapon_cz75a" || weaponName == "weapon_deagle" || weaponName == "weapon_elite" || weaponName == "weapon_fiveseven" || weaponName == "weapon_glock" || weaponName == "weapon_hkp2000" || weaponName == "weapon_p250" || weaponName == "weapon_revolver" || weaponName == "weapon_tec9" || weaponName == "weapon_usp_silencer")
		if (global.features.PSaimbotvisable)
			visable = true;

	//SR
	if (weaponName == "weapon_awp" || weaponName == "weapon_g3sg1" || weaponName == "weapon_scar20" || weaponName == "weapon_ssg08")
		if (global.features.SRaimbotvisable)
			visable = true;

	//SMG
	if (weaponName == "weapon_mac10" || weaponName == "weapon_mp7" || weaponName == "weapon_mp9" || weaponName == "weapon_mp5sd" || weaponName == "weapon_ump45" || weaponName == "weapon_bizon" || weaponName == "weapon_p90")
		if (global.features.SMGaimbotvisable)
			visable = true;

	return visable;
}

bool CAimbot::AimbotAutoshoot(std::string weaponName)
{
	bool autoShoot = false;

	//AR
	if (weaponName == "weapon_ak47" || weaponName == "weapon_aug" || weaponName == "weapon_famas" || weaponName == "weapon_galilar" || weaponName == "weapon_m4a1_silencer" || weaponName == "weapon_m4a1" || weaponName == "weapon_sg553")
		if (global.features.ARaimbotautoshot)
			autoShoot = true;

	//SG
	if (weaponName == "weapon_mag7" || weaponName == "weapon_sawedoff" || weaponName == "weapon_nova" || weaponName == "weapon_xm1014")
		if (global.features.SGaimbotautoshot)
			autoShoot = true;

	//Pistols
	if (weaponName == "weapon_cz75a" || weaponName == "weapon_deagle" || weaponName == "weapon_elite" || weaponName == "weapon_fiveseven" || weaponName == "weapon_glock" || weaponName == "weapon_hkp2000" || weaponName == "weapon_p250" || weaponName == "weapon_revolver" || weaponName == "weapon_tec9" || weaponName == "weapon_usp_silencer")
		if (global.features.PSaimbotautoshot)
			autoShoot = true;

	//SR
	if (weaponName == "weapon_awp" || weaponName == "weapon_g3sg1" || weaponName == "weapon_scar20" || weaponName == "weapon_ssg08")
		if (global.features.SRaimbotautoshot)
			autoShoot = true;

	//SMG
	if (weaponName == "weapon_mac10" || weaponName == "weapon_mp7" || weaponName == "weapon_mp9" || weaponName == "weapon_mp5sd" || weaponName == "weapon_ump45" || weaponName == "weapon_bizon" || weaponName == "weapon_p90")
		if (global.features.SMGaimbotautoshot)
			autoShoot = true;

	return autoShoot;
}

float CAimbot::AimbotSmooth(std::string weaponName)
{
	float smooth = 0;

	//AR
	if (weaponName == "weapon_ak47" || weaponName == "weapon_aug" || weaponName == "weapon_famas" || weaponName == "weapon_galilar" || weaponName == "weapon_m4a1_silencer" || weaponName == "weapon_m4a1" || weaponName == "weapon_sg553")
		smooth = global.features.ARaimbotsmooth;

	//SG
	if (weaponName == "weapon_mag7" || weaponName == "weapon_sawedoff" || weaponName == "weapon_nova" || weaponName == "weapon_xm1014")
		smooth = global.features.SGaimbotsmooth;

	//Pistols
	if (weaponName == "weapon_cz75a" || weaponName == "weapon_deagle" || weaponName == "weapon_elite" || weaponName == "weapon_fiveseven" || weaponName == "weapon_glock" || weaponName == "weapon_hkp2000" || weaponName == "weapon_p250" || weaponName == "weapon_revolver" || weaponName == "weapon_tec9" || weaponName == "weapon_usp_silencer")
		smooth = global.features.PSaimbotsmooth;

	//SR
	if (weaponName == "weapon_awp" || weaponName == "weapon_g3sg1" || weaponName == "weapon_scar20" || weaponName == "weapon_ssg08")
		smooth = global.features.SRaimbotsmooth;

	//SMG
	if (weaponName == "weapon_mac10" || weaponName == "weapon_mp7" || weaponName == "weapon_mp9" || weaponName == "weapon_mp5sd" || weaponName == "weapon_ump45" || weaponName == "weapon_bizon" || weaponName == "weapon_p90")
		smooth = global.features.SMGaimbotsmooth;

	return smooth;
}

int CAimbot::AimbotDistance(std::string weaponName)
{
	int distance = 0;

	//AR
	if (weaponName == "weapon_ak47" || weaponName == "weapon_aug" || weaponName == "weapon_famas" || weaponName == "weapon_galilar" || weaponName == "weapon_m4a1_silencer" || weaponName == "weapon_m4a1" || weaponName == "weapon_sg553")
		distance = global.features.ARaimbotdistance;

	//SG
	if (weaponName == "weapon_mag7" || weaponName == "weapon_sawedoff" || weaponName == "weapon_nova" || weaponName == "weapon_xm1014")
		distance = global.features.SGaimbotdistance;

	//Pistols
	if (weaponName == "weapon_cz75a" || weaponName == "weapon_deagle" || weaponName == "weapon_elite" || weaponName == "weapon_fiveseven" || weaponName == "weapon_glock" || weaponName == "weapon_hkp2000" || weaponName == "weapon_p250" || weaponName == "weapon_revolver" || weaponName == "weapon_tec9" || weaponName == "weapon_usp_silencer")
		distance = global.features.PSaimbotdistance;

	//SR
	if (weaponName == "weapon_awp" || weaponName == "weapon_g3sg1" || weaponName == "weapon_scar20" || weaponName == "weapon_ssg08")
		distance = global.features.SRaimbotdistance;

	//SMG
	if (weaponName == "weapon_mac10" || weaponName == "weapon_mp7" || weaponName == "weapon_mp9" || weaponName == "weapon_mp5sd" || weaponName == "weapon_ump45" || weaponName == "weapon_bizon" || weaponName == "weapon_p90")
		distance = global.features.SMGaimbotdistance;

	return distance;
}

int CAimbot::AimbotAutoSleep(std::string weaponName)
{
	int sleep = 0;

	//AR
	if (weaponName == "weapon_ak47" || weaponName == "weapon_aug" || weaponName == "weapon_famas" || weaponName == "weapon_galilar" || weaponName == "weapon_m4a1_silencer" || weaponName == "weapon_m4a1" || weaponName == "weapon_sg553")
		sleep = global.features.ARautosleep;

	//SG
	if (weaponName == "weapon_mag7" || weaponName == "weapon_sawedoff" || weaponName == "weapon_nova" || weaponName == "weapon_xm1014")
		sleep = global.features.SGautosleep;

	//Pistols
	if (weaponName == "weapon_cz75a" || weaponName == "weapon_deagle" || weaponName == "weapon_elite" || weaponName == "weapon_fiveseven" || weaponName == "weapon_glock" || weaponName == "weapon_hkp2000" || weaponName == "weapon_p250" || weaponName == "weapon_revolver" || weaponName == "weapon_tec9" || weaponName == "weapon_usp_silencer")
		sleep = global.features.PSautosleep;

	//SR
	if (weaponName == "weapon_awp" || weaponName == "weapon_g3sg1" || weaponName == "weapon_scar20" || weaponName == "weapon_ssg08")
		sleep = global.features.SRautosleep;

	//SMG
	if (weaponName == "weapon_mac10" || weaponName == "weapon_mp7" || weaponName == "weapon_mp9" || weaponName == "weapon_mp5sd" || weaponName == "weapon_ump45" || weaponName == "weapon_bizon" || weaponName == "weapon_p90")
		sleep = global.features.SMGautosleep;

	return sleep;
}