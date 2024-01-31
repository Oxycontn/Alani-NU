#include <thread>

#include "../math/vector.hpp"
#include "..\entity\entity.h"

class CEntityLoop
{
public:
	void EspThread();

	void EspLoop();
	void Bone(view_matrix_t viewMatrix, int localTeam, int playerTeam, Vector feet, Vector head, CEntity* pCSPlayerPawn);

	// as discussed, no point in making entire classes for 1 / 2 functions.
	// if they're inheriting, that's a different story.
	void RenderEsp(view_matrix_t viewMatrix, Vector playerBoneHead, Vector playerFeet, Vector playerHead, Vector localPos, std::string weaponName, int localTeam, int playerTeam, std::string playerName, int armorValue, int healthValue, CEntity* pCSPlayerPawn, bool spottedState);
	void RenderWorld(Vector localPos, Vector entityPos, view_matrix_t viewMatrix, std::string designerName);
	void RenderFov(std::string localWeaponName);
};

inline CEntityLoop entityloop;