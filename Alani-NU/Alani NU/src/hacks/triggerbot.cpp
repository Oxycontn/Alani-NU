#include "triggerbot.h"
#include "..\entity\local.h"
#include "..\entity\entity.h"
#include "..\classes\global.hpp"

void CTrigger::TriggerBot()
{
	//local varibles
	auto localPlayerPawn = CLocal::GetLocalPawn();
	int crosshhairId = localPlayerPawn.CrosshairID();
	int localTeam = localPlayerPawn.Team();
	std::string weaponName = localPlayerPawn.GetWeaponNameLocal();
	int weaponGroup = CLocal::GetWeaponGroup(weaponName);

	//settings
	bool triggerEnable = TriggerEnable(weaponGroup);
	bool triggerTeam = TriggerTeam(weaponGroup);
	int vKey = TriggerKey(weaponGroup);
	float triggerWait = TriggerWait(weaponGroup);

	if (triggerEnable)
	{
		if (crosshhairId != -1)
		{
			auto entityList = CEntity::GetEntityList();

			auto currentPawn = CCrosshair::GetCurrentPawn(entityList, crosshhairId);

			int playerTeam = currentPawn.Team();

			if (GetAsyncKeyState(vKey))
			{
				if (triggerTeam && playerTeam == localTeam)
				{
					Sleep(triggerWait);
					mouse_event(MOUSEEVENTF_LEFTDOWN, 0, 0, 0, 0);
					mouse_event(MOUSEEVENTF_LEFTUP, 0, 0, 0, 0);
				}

				if (playerTeam != localTeam)
				{
					Sleep(triggerWait);
					mouse_event(MOUSEEVENTF_LEFTDOWN, 0, 0, 0, 0);
					mouse_event(MOUSEEVENTF_LEFTUP, 0, 0, 0, 0);
				}
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

bool CTrigger::TriggerEnable(int weaponGroup)
{
	bool enable{};

	switch (weaponGroup)
	{
	case 1:
		if (global.features.ARtriggerenable)
			enable = true;
		break;

	case 2:
		if (global.features.SGtriggerenable)
			enable = true;
		break;

	case 3:
		if (global.features.PStriggerenable)
			enable = true;
		break;

	case 4:
		if (global.features.SRtriggerenable)
			enable = true;
		break;

	case 5:
		if (global.features.SMGtriggerenable)
			enable = true;
		break;
	}

	return enable;
}

float CTrigger::TriggerWait(int weaponGroup)
{
	float wait{};

	switch (weaponGroup)
	{
	case 1:
		wait = global.features.ARtriggerwait;
		break;

	case 2:
		wait = global.features.SGtriggerwait;
		break;

	case 3:
		wait = global.features.PStriggerwait;
		break;

	case 4:
		wait = global.features.SRtriggerwait;
		break;

	case 5:
		wait = global.features.SMGtriggerwait;
		break;
	}

	return wait;
}

int CTrigger::TriggerKey(int weaponGroup)
{
	int vKey{};

	switch (weaponGroup)
	{
	case 1:
		switch (global.features.ARtriggerkey)
		{
		case 0:
			vKey = VK_XBUTTON2;
			break;

		case 1:
			vKey = VK_LSHIFT;
			break;

		case 2:
			vKey = VK_RBUTTON;
			break;

		case 3:
			vKey = VK_XBUTTON1;
			break;
		}
		break;

	case 2:
		switch (global.features.SGtriggerkey)
		{
		case 0:
			vKey = VK_XBUTTON2;
			break;

		case 1:
			vKey = VK_LSHIFT;
			break;

		case 2:
			vKey = VK_RBUTTON;
			break;

		case 3:
			vKey = VK_XBUTTON1;
			break;
		}
		break;

	case 3:
		switch (global.features.PStriggerkey)
		{
		case 0:
			vKey = VK_XBUTTON2;
			break;

		case 1:
			vKey = VK_LSHIFT;
			break;

		case 2:
			vKey = VK_RBUTTON;
			break;

		case 3:
			vKey = VK_XBUTTON1;
			break;
		}
		break;

	case 4:
		switch (global.features.SRtriggerkey)
		{
		case 0:
			vKey = VK_XBUTTON2;
			break;

		case 1:
			vKey = VK_LSHIFT;
			break;

		case 2:
			vKey = VK_RBUTTON;
			break;

		case 3:
			vKey = VK_XBUTTON1;
			break;
		}
		break;

	case 5:
		switch (global.features.SMGtriggerkey)
		{
		case 0:
			vKey = VK_XBUTTON2;
			break;

		case 1:
			vKey = VK_LSHIFT;
			break;

		case 2:
			vKey = VK_RBUTTON;
			break;

		case 3:
			vKey = VK_XBUTTON1;
			break;
		}
		break;
	}

	return vKey;
}

bool CTrigger::TriggerTeam(int weaponGroup)
{
	bool enable{};

	switch (weaponGroup)
	{
	case 1:
		if (global.features.ARtriggerteam)
			enable = true;
		break;

	case 2:
		if (global.features.SGtriggerteam)
			enable = true;
		break;

	case 3:
		if (global.features.PStriggerteam)
			enable = true;
		break;

	case 4:
		if (global.features.SRtriggerteam)
			enable = true;
		break;

	case 5:
		if (global.features.SMGtriggerteam)
			enable = true;
		break;
	}

	return enable;
}