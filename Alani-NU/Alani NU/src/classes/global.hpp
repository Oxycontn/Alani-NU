#pragma warning( disable : 4305 )

#include "../math/vector.hpp"
#include "../../ext/ImGui/imgui.h" 
#include "../overlay/overlay.hpp" 

#include <string>

class CGlobals
{
public:
	struct features_t
	{
		//player entites bools
		bool worldenable;
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
		float ARaimbotfov;
		float ARaimbotsmooth;;
		int ARaimbotdistance;
		int ARaimbotcombobone;
		int ARaimbotcombokey;
		bool ARaimbotautoshot;
		bool ARaimbotvisable;
		bool ARaimbotfovcircle;
		int ARautosleep;

		//SMG
		bool SMGaimbotenable;
		float SMGaimbotfov;
		float SMGaimbotsmooth;
		int SMGaimbotdistance;
		int SMGaimbotcombobone;
		int SMGaimbotcombokey;
		bool SMGaimbotautoshot;
		bool SMGaimbotvisable;
		bool SMGaimbotfovcircle;
		int SMGautosleep;

		//SR
		bool SRaimbotenable;
		float SRaimbotfov;
		float SRaimbotsmooth;
		int SRaimbotdistance;
		int SRaimbotcombobone;
		int SRaimbotcombokey;
		bool SRaimbotautoshot;
		bool SRaimbotvisable;
		bool SRaimbotfovcircle;
		int SRautosleep;

		//SG
		bool SGaimbotenable;
		float SGaimbotfov;
		float SGaimbotsmooth;
		int SGaimbotdistance;
		int SGaimbotcombobone;
		int SGaimbotcombokey;
		bool SGaimbotautoshot;
		bool SGaimbotvisable;
		bool SGaimbotfovcircle;
		int SGautosleep;

		//PS
		bool PSaimbotenable;
		float PSaimbotfov;
		float PSaimbotsmooth;
		int PSaimbotdistance;
		int PSaimbotcombobone;
		int PSaimbotcombokey;
		bool PSaimbotautoshot;
		bool PSaimbotvisable;
		bool PSaimbotfovcircle;
		int PSautosleep;

		//rcs
		bool rcsenable;
		float rcsscaleX;
		float rcsscaleY;
	};

	struct modules_t
	{
		uintptr_t client;
		uintptr_t engine;
	};

	struct threads_t
	{
		bool stopEsp = false;
		bool stopAimbot = false;
	};

	features_t features;
	modules_t modules;
	threads_t threads;
};
inline CGlobals global;

namespace offset 
{
	// player classes
	constexpr std::ptrdiff_t dwEntityList = 0x17CE6A0;
	constexpr std::ptrdiff_t dwLocalPlayerController = 0x181DC98;
	constexpr std::ptrdiff_t dwLocalPlayerPawn = 0x16D4F48;
	constexpr std::ptrdiff_t dwViewMatrix = 0x182CEA0;

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