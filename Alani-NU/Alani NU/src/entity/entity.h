#pragma once
#include "../math/vector.hpp"
#include "xstring"

class CEntity
{
public:
	CEntity(uintptr_t entity);
	
	static uintptr_t GetEntityList();
	static uintptr_t GetPlayerController(uintptr_t entityList, uintptr_t localPlayer, int i);
	static CEntity* GetpCSPlayerPawn(uintptr_t entityList, uintptr_t playerController, int i);
	static CEntity* PlayerController(uintptr_t playerController);

	int Health() const noexcept;
	int Team() const noexcept;
	int Armor() const noexcept;

	bool spottedState() const noexcept;

	uintptr_t Gamescene() const noexcept;
	// uses gamescene to return bonearray
	uintptr_t Bonearray() const noexcept;

	// most used when using bone
	Vector Feet(uintptr_t bonearray) const noexcept;
	Vector Head(uintptr_t bonearray) const noexcept;

	Vector Bone(int bone) const noexcept;

	std::string GetWeaponName();
	// will move into its own class eventually, along with playerController. let me do this alone so i have something to work on tmrw, thanks.
	static std::string GetPlayerName(uintptr_t playerController);

private:
	uintptr_t entityAddress;
};