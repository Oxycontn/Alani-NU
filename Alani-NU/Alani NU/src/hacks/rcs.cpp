#pragma warning( disable : 4244 4312 4305 )

#include "rcs.h"
#include "..\classes\global.hpp"
#include "..\entity\entity.h"
#include "aimbot.h"
#include "..\mem\memory.h"
#include "..\entity\local.h"

void CRCS::RcsLoop()
{
	std::string weaponName = global.localPlayer.weaponName;
	auto localPlayerPawn = CLocal::GetLocalPawn();
	int shotsFired = localPlayerPawn->ShotsFired();

	if (global.features.rcsenable && !aimbot.aimbotRCS)
	{
		if (weaponName == "weapon_ak47" || weaponName == "weapon_aug" || weaponName == "weapon_famas" || weaponName == "weapon_galilar" || weaponName == "weapon_m4a1_silencer" || weaponName == "weapon_m4a1" || weaponName == "weapon_sg553" || weaponName == "weapon_mac10" || weaponName == "weapon_mp7" || weaponName == "weapon_mp9" || weaponName == "weapon_mp5sd" || weaponName == "weapon_ump45" || weaponName == "weapon_bizon" || weaponName == "weapon_p90")
		{
			if (shotsFired != 0 && GetAsyncKeyState(VK_LBUTTON))
			{
				Vector2 viewAngles = localPlayerPawn->ViewAngle();

				aimPunchCache aimpunchCache = localPlayerPawn->AimPunch();

				Vector2 aimPunch = CLocal::AimPunchAngle(aimpunchCache);

				if (!aimPunch.x && !aimPunch.y)
					return;

				newAngle.x = ((viewAngles.x + oldAngle.x) - (aimPunch.x * global.features.rcsscaleX));
				newAngle.y = ((viewAngles.y + oldAngle.y) - (aimPunch.y * global.features.rcsscaleY));
				newAngle.z = 0.0f;

				newAngle = Vector::Clamp(newAngle);

				newAngle = Vector::Normalize(newAngle);

				if (newAngle.x == 0)
					return;

				//mem.Write<Vector>(global.modules.client + offset::dwViewAngles, newAngle);

				oldAngle.x = aimPunch.x * global.features.rcsscaleX;
				oldAngle.y = aimPunch.y * global.features.rcsscaleY;
				oldAngle.z = 0.0f;
			}
			else
			{
				oldAngle.x = 0.0f;
				oldAngle.y = 0.0f;
				oldAngle.z = 0.0f;
			}
		}
	}
}