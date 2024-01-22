#pragma warning( disable : 4305 )

#include "../math/vector.hpp"
#include "../../ext/ImGui/imgui.h" //for ImVec4!
#include "../overlay/overlay.hpp" //for rgb

#include <string>

class CGlobals
{
public:
	struct features_t
	{
		//player entites bools
		bool c4;
		bool molotov;
		bool smoke;
		bool flash;
		bool grenade;
		bool weapon;
		bool decoy;
		bool incgrenade;

		//team bools
		bool glowTeam;
		bool teamhead;
		bool teamhealth;
		bool teamskel;
		bool teamarmor;
		bool teamweapon;
		bool teamsnap;
		bool teamname;
		bool teamenable;
		bool teamJoint;
		int teamcombo;

		//team colors and alphas
		float teamalpha = 60;
		RGB teaamcolor = { 0, 0, 1 };
		float teamBoneAlpha = 255;
		RGB teamBoneColor = { 1,1,1 };
		float teamSnapAlpha = 255;
		RGB teamSnapColor = { 1,1,1 };
		float teamHeadAlpha = 255;
		RGB teamHeaadColor = { 1,1,1 };

		//enemy bools
		bool glowEnemy;
		bool enemyhead;
		bool enemyhealth;
		bool enemyskel;
		bool enemyarmor;
		bool enemyweapon;
		bool enemysnap;
		bool enemyname;
		bool enemyenable;
		bool enemyJoint;
		int enemycombo;

		//enemy colors and alphas
		float enemyalpha = 60;
		RGB enemycolor = { 1 ,0 , 0 };
		float enemyBoneAlpha = 255;
		RGB enemyBoneColor = { 1,1,1 };
		float enemySnapAlpha = 255;
		RGB enemySnapColor = { 1,1,1 };
		float enemyHeadAlpha = 255;
		RGB enemyHeaadColor = { 1,1,1 };

		// miscallenous
		const char* comboSelections[4] = { "None", "2D Boxes", "3D Boxes", "Corners" };
		const char* aimbotBoneSelections[4] = { "Head", "Neck", "Spine", "Crouch" };
		const char* aimbotKeySelections[4] = { "Mouse Button 2", "LShift", "LCTL", "Left Click" };
		const char* aimbotGunSelection[5] = { "Assault Rifles", "Shotguns", "Pistols", "Sniper Rifles", "SMGs"};

		//aimbot
		int aimbotGunCombo;

		//AR
		bool ARaimbotenable;
		float ARaimbotfov = 6.0f;
		float ARaimbotsmooth = 0.24;
		int ARaimbotdistance = 300;
		int ARaimbotcombobone;
		int ARaimbotcombokey;
		bool ARaimbotautoshot;
		bool ARaimbotvisable;
		bool ARaimbotfovcircle;
		int ARautosleep;

		//SMG
		bool SMGaimbotenable;
		float SMGaimbotfov = 6.0f;
		float SMGaimbotsmooth = 0.24;
		int SMGaimbotdistance = 300;
		int SMGaimbotcombobone;
		int SMGaimbotcombokey;
		bool SMGaimbotautoshot;
		bool SMGaimbotvisable;
		bool SMGaimbotfovcircle;
		int SMGautosleep;

		//SR
		bool SRaimbotenable;
		float SRaimbotfov = 6.0f;
		float SRaimbotsmooth = 0.24;
		int SRaimbotdistance = 300;
		int SRaimbotcombobone;
		int SRaimbotcombokey;
		bool SRaimbotautoshot;
		bool SRaimbotvisable;
		bool SRaimbotfovcircle;
		int SRautosleep;

		//SG
		bool SGaimbotenable;
		float SGaimbotfov = 6.0f;
		float SGaimbotsmooth = 0.24;
		int SGaimbotdistance = 300;
		int SGaimbotcombobone;
		int SGaimbotcombokey;
		bool SGaimbotautoshot;
		bool SGaimbotvisable;
		bool SGaimbotfovcircle;
		int SGautosleep;

		//PS
		bool PSaimbotenable;
		float PSaimbotfov = 6.0f;
		float PSaimbotsmooth = 0.24;
		int PSaimbotdistance = 300;
		int PSaimbotcombobone;
		int PSaimbotcombokey;
		bool PSaimbotautoshot;
		bool PSaimbotvisable;
		bool PSaimbotfovcircle;
		int PSautosleep;

		//rcs
		bool rcsenable;
		float rcsscaleX = 2.f;
		float rcsscaleY = 2.f;
	};

	struct modules_t
	{
		uintptr_t client;
		uintptr_t engine;
	};

	struct player_t
	{
		uintptr_t pCSPlayerPawn;
		uintptr_t playerPawn;
		uintptr_t playerController;
		uintptr_t dwEntityList, listEntry1, listEntry2;
		
		int health, team, armor;

		bool IsAlive()
		{
			return health > 0 && health <= 100;
		}

		float flash_alpha;

		Vector feet, boneHead, head;

		uintptr_t gamescene, bonearray;

		std::string weapon = "Invalid";
		std::string name = "Invalid";
		std::string healthStr = std::to_string(health); // for use in the ESP later on.

		float circleRadius;
	};

	struct local_t
	{
		uintptr_t localPlayerController, localPlayerPawn;

		int health, team, ping;

		Vector position;

		view_matrix_t vm;

		std::string weaponName;
	};

	struct world_t
	{
		std::string designerName;
		Vector entityOrigin;
	};

	features_t features;
	modules_t modules;
	player_t player;
	local_t localPlayer;
	world_t world;
};
inline CGlobals global;

namespace offset 
{
	// player classes
	constexpr std::ptrdiff_t dwEntityList = 0x17C26A0;
	constexpr std::ptrdiff_t dwLocalPlayerController = 0x1811C98;
	constexpr std::ptrdiff_t dwLocalPlayerPawn = 0x16C8F48;
	constexpr std::ptrdiff_t dwViewMatrix = 0x1820EA0;
	constexpr std::ptrdiff_t dwViewAngles = 0x1884F30;
	constexpr std::ptrdiff_t dwSensitivity = 0x181FA98;

	// engine2_dll
	constexpr std::ptrdiff_t dwNetworkGameClient = 0x4E0988;
	constexpr std::ptrdiff_t dwNetworkGameClient_signOnState = 0x240;

	// CCSPlayerController
	constexpr std::ptrdiff_t m_iHealth = 0x32C;
	constexpr std::ptrdiff_t Origin = 0x1224;
	constexpr std::ptrdiff_t m_sSanitizedPlayerName = 0x750;
	constexpr std::ptrdiff_t m_ArmorValue = 0x1510;
	constexpr std::ptrdiff_t m_iPing = 0x720;

	// pawn shit
	constexpr std::ptrdiff_t Pawn = 0x60C;
	
	// CBasePlayerController
	constexpr std::ptrdiff_t m_iTeamNum = 0x3BF;

	// weapon
	constexpr std::ptrdiff_t m_pClippingWeapon = 0x12B0;
	constexpr std::ptrdiff_t m_szName = 0xC18;
	constexpr std::ptrdiff_t m_hOwnerEntity = 0x418;
	constexpr std::ptrdiff_t m_nSubclassID = 0x358;

	// aimbot
	constexpr std::ptrdiff_t v_angle = 0x114C;
	constexpr std::ptrdiff_t vecViewOffset = 0xC48;
	constexpr std::ptrdiff_t m_iShotsFired = 0x1420;
	constexpr std::ptrdiff_t m_aimPunchCache = 0x1740;
	constexpr std::ptrdiff_t m_entitySpottedState = 0x1638;
	constexpr std::ptrdiff_t m_fFlags = 0x3C8;

	// world entites
	constexpr std::ptrdiff_t m_pEntity = 0x10;
	constexpr std::ptrdiff_t m_pGameSceneNode = 0x310;
	constexpr std::ptrdiff_t m_designerName = 0x20;
	constexpr std::ptrdiff_t m_vecAbsOrigin = 0xC8;
	constexpr std::ptrdiff_t dwGameEntitySystem_getHighestEntityIndex = 0x1510;
}