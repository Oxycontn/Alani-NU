#pragma warning( disable : 4244 4312 4305 )

#include "rcs.h"
#include "..\classes\global.hpp"
#include "..\entity\entity.h"
#include "aimbot.h"
#include "..\mem\memory.h"
#include "..\entity\local.h"

void CRCS::RcsLoop()
{
	if (global.features.rcsenable)
	{
		auto localPlayerPawn = CLocal::GetLocalPawn();
		int shotsFired = localPlayerPawn.ShotsFired();

		std::string weaponName = localPlayerPawn.GetWeaponNameLocal();
		int weaponGroup = CLocal::GetWeaponGroup(weaponName);

		if (!aimbot.aimbotRCS && weaponGroup == 1 || weaponGroup == 5)
		{
			if (shotsFired != 0 && GetAsyncKeyState(VK_LBUTTON))
			{
				Vector2 viewAngles = localPlayerPawn.ViewAngle();

				aimPunchCache aimpunchCache = localPlayerPawn.AimPunch();

				Vector2 aimPunch = CLocal::AimPunchAngle(aimpunchCache);

				if (!aimPunch.x || !aimPunch.y)
					return;

				newAngle.x = ((viewAngles.x + oldAngle.x) - (aimPunch.x * global.features.rcsscaleX));
				newAngle.y = ((viewAngles.y + oldAngle.y) - (aimPunch.y * global.features.rcsscaleY));
				newAngle.z = 0.0f;

				Vector2 screenOffset = Vector2::AngleToScreenOffset(newAngle.y, newAngle.x, viewAngles.y, viewAngles.x, 30.f);
				mouse_event(MOUSEEVENTF_MOVE, static_cast<DWORD>(screenOffset.x), static_cast<DWORD>(screenOffset.y), 0, 0);

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