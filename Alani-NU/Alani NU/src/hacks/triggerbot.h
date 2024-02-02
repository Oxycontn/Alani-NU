#pragma once


class CTrigger
{
public:
	void TriggerBot();

	bool TriggerEnable(int weaponGroup);
	float TriggerWait(int weaponGroup);
	int TriggerKey(int weaponGroup);
	bool TriggerTeam(int weaponGroup);
};
inline CTrigger trigger;