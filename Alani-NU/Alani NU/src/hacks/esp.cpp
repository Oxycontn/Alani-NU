#pragma warning( disable : 4244 4312 4305 )

#include "esp.h"
#include "..\entity\entity.h"

#include "rcs.h"

#include "../classes/global.hpp"
#include "../overlay/overlay.hpp"
#include "../classes/bone.hpp"
#include "aimbot.h"
#include "..\mem\memory.h"
#include "..\entity\local.h"
#include "..\entity\wentity.h"

//esp
void CEntityLoop::EspThread()
{
    overlay.CreateOverlay();

    overlay.CreateDevice();

    overlay.CreateImGui();

    while (true)
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(1));

        if (global.threads.stopEsp)
            std::terminate();

        overlay.StartRender();

        //menu calls
        if (GetAsyncKeyState(VK_INSERT) & 1)
            overlay.RenderMenu = !overlay.RenderMenu;

        if (overlay.RenderMenu)
        {
            SetWindowLong(overlay.overlay, GWL_EXSTYLE, WS_EX_TOOLWINDOW | WS_EX_TRANSPARENT | WS_EX_TOPMOST);
            menu.NewMenu();
            overlay.EndRender();
        }
        else
            SetWindowLong(overlay.overlay, GWL_EXSTYLE, WS_EX_TOOLWINDOW | WS_EX_TRANSPARENT | WS_EX_LAYERED | WS_EX_TOPMOST);

        //read localplayer
        ReadLocalPlayer();

        //read entites then call render inside loop
        EspLoop();

        overlay.EndRender();
    }
}

void CEntityLoop::ReadLocalPlayer()
{
    uintptr_t localPlayerController = driver.Read<uintptr_t>(global.modules.client + offset::dwLocalPlayerController);
    auto localPlayerPawn = CLocal::GetLocalPawn();
    view_matrix_t vm = CLocal::GetViewMatrix();

    int localTeam = localPlayerPawn->Team();
    int health = localPlayerPawn->Health();

    std::string weaponName = localPlayerPawn->GetWeaponNameLocal();

    Vector position = localPlayerPawn->Position();

    global.localPlayer.localPlayerController = localPlayerController;
    global.localPlayer.team = localTeam;
    global.localPlayer.health = health;
    global.localPlayer.vm = vm;
    global.localPlayer.position = position;
    global.localPlayer.weaponName = weaponName;
}

void CEntityLoop::EspLoop()
{
    uintptr_t entityList = CEntity::GetEntityList();
    int entMax = driver.Read<int>(entityList + offset::dwGameEntitySystem_getHighestEntityIndex);

    for (int i = 1; i < entMax; ++i)
    {
        if (overlay.RenderMenu)
            continue;

        uintptr_t playerController = CEntity::GetPlayerController(entityList, global.localPlayer.localPlayerController, i);
        auto pc = CEntity::PlayerController(playerController);

        //plyer entities
        if (i <= 64)
        {
            auto pCSPlayerPawn = CEntity::GetpCSPlayerPawn(entityList, playerController, i);

            int health = pCSPlayerPawn->Health();
            int playerTeam = pCSPlayerPawn->Team();
            int armor = pCSPlayerPawn->Armor();

            if (health <= 0 || health > 100)
                continue;

            if (global.features.glowEnemy) {
                //glow
            }
            if (global.features.glowTeam) {
                // glow 
            }

            uintptr_t gamescene = pCSPlayerPawn->Gamescene();
            uintptr_t bonearray = pCSPlayerPawn->Bonearray();

            Vector readFeet = pCSPlayerPawn->Feet(bonearray);
            Vector readBoneHead = pCSPlayerPawn->Bone(bones::head);

            // made a wrapper for everything below but it was bugging out, not sure why. we'll keep this here for now i guess.
            Vector head;

            head.x = readFeet.x;
            head.y = readFeet.y;
            head.z = readFeet.z + 75.f;

            std::string playerName = CEntity::GetPlayerName(playerController);
            std::string weaponName = pCSPlayerPawn->GetWeaponName();

            global.player.health = health;
            global.player.team = playerTeam; 
            global.player.armor = armor;
            global.player.feet = readFeet;
            global.player.boneHead = readBoneHead;
            global.player.head = head;
            global.player.gamescene = gamescene;
            global.player.bonearray = bonearray;
            global.player.name = playerName;
            global.player.weapon = weaponName;

            RenderEsp();
        }

        //world entities
        if (i >= 65)
        {
            auto entityController = CWEntity::EntityController(playerController);

            auto entityOwner = entityController->EntityOwner();
            if (entityOwner != -1)
                continue;

            auto pEntity = entityController->PEntity();

            auto designerNameptr = CWEntity::DesignerNamePtr(pEntity);

            std::string designerName = CWEntity::ReadDesignerName(designerNameptr);

            if (designerName.find("weapon"))
                continue;

            auto gameScene = entityController->Gamescene();

            Vector entityOrigin = CWEntity::EntityPosition(gameScene);

            global.world.designerName = designerName;
            global.world.entityOrigin = entityOrigin;

            RenderWorld();
        }

        RenderFov();
    }
}

void CEntityLoop::Bone()
{
    for (int i = 0; i < sizeof(boneConnections) / sizeof(boneConnections[0]); ++i)
    {
        int bone1 = boneConnections[i].bone1;
        int bone2 = boneConnections[i].bone2;

        Vector VectorBone1 = driver.Read<Vector>(global.player.bonearray + bone1 * 32);
        Vector VectorBone2 = driver.Read<Vector>(global.player.bonearray + bone2 * 32);

        Vector b1;
        Vector b2;

        if (Vector::WTS(global.localPlayer.vm, VectorBone1, b1) &&
            Vector::WTS(global.localPlayer.vm, VectorBone2, b2))
        {
            if (global.localPlayer.team == global.player.team)
            {
                Render::Line(b1.x, b1.y, b2.x, b2.y, global.features.teamBoneColor, global.features.teamBoneAlpha, 1.5);
                if (global.features.teamJoint)
                    Render::Circle(b2.x, b2.y, global.player.circleRadius, white, 255, true);
            }
            else if (global.localPlayer.team != global.player.team)
            {
                Render::Line(b1.x, b1.y, b2.x, b2.y, global.features.enemyBoneColor, global.features.enemyBoneAlpha, 1.5);
                if (global.features.enemyJoint)
                    Render::Circle(b2.x, b2.y, global.player.circleRadius, white, 255, true);
            }
        }
    }
}

//render
void CEntityLoop::RenderWorld()
{
    Vector entity;

    if (Vector::WTS(global.localPlayer.vm, global.world.entityOrigin, entity))
    {
        float distance = global.localPlayer.position.CalculateDistance(entity) / 500;
        float fontSize = 100.f / distance;
        std::string weaponName = global.world.designerName.substr(7);

        if (global.features.weapon)
        {
            if (global.world.designerName.find("weapon_molotov" || "weapon_decoy" || "weapon_flashbang" || "weapon_hegrenade" || "weapon_smokegrenade" || "weapon_incgrenade"))
            {
                Render::Text(fontSize, entity.x - 10, entity.y + 10, white, 255, weaponName);
                Render::Rect(entity.x - 10, entity.y - 10, 40, 20, white, 255, 1.5);
            }
        }

        if (global.features.molotov)
        {
            if (!global.world.designerName.find("weapon_molotov"))
            {
                Render::Text(fontSize, entity.x - 10, entity.y + 10, white, 255, weaponName);
                Render::Rect(entity.x - 10, entity.y - 10, 40, 20, white, 255, 1.5);
            }
        }

        if (global.features.incgrenade)
        {
            if (!global.world.designerName.find("weapon_incgrenade"))
            {
                Render::Text(fontSize, entity.x - 10, entity.y + 10, white, 255, weaponName);
                Render::Rect(entity.x - 10, entity.y - 10, 40, 20, white, 255, 1.5);
            }
        }

        if (global.features.decoy)
        {
            if (!global.world.designerName.find("weapon_decoy"))
            {
                Render::Text(fontSize, entity.x - 10, entity.y + 10, white, 255, weaponName);
                Render::Rect(entity.x - 10, entity.y - 10, 40, 20, white, 255, 1.5);
            }
        }

        if (global.features.flash)
        {
            if (!global.world.designerName.find("weapon_flashbang"))
            {
                Render::Text(fontSize, entity.x - 10, entity.y + 10, white, 255, weaponName);
                Render::Rect(entity.x - 10, entity.y - 10, 40, 20, white, 255, 1.5);
            }
        }

        if (global.features.grenade)
        {
            if (!global.world.designerName.find("weapon_hegrenade"))
            {
                Render::Text(fontSize, entity.x - 10, entity.y + 10, white, 255, weaponName);
                Render::Rect(entity.x - 10, entity.y - 10, 40, 20, white, 255, 1.5);
            }
        }

        if (global.features.smoke)
        {
            if (!global.world.designerName.find("weapon_smokegrenade"))
            {
                Render::Text(fontSize, entity.x - 10, entity.y + 10, white, 255, weaponName);
                Render::Rect(entity.x - 10, entity.y - 10, 40, 20, white, 255, 1.5);
            }
        }

        if (global.features.c4)
        {
            if (!global.world.designerName.find("weapon_c4"))
            {
                Render::Text(fontSize, entity.x - 10, entity.y + 10, white, 255, weaponName);
                Render::Rect(entity.x - 10, entity.y - 10, 40, 20, white, 255, 1.5);
            }
        }
    }
}

void CEntityLoop::RenderEsp()
{
    Vector feet;
    Vector head;
    Vector boneHead;

    if (Vector::WTS(global.localPlayer.vm, global.player.boneHead, boneHead) &&
        Vector::WTS(global.localPlayer.vm, global.player.feet, feet) &&
        Vector::WTS(global.localPlayer.vm, global.player.head, head))
    {
        float height = (feet.y - boneHead.y) * 1.35;
        float width = height / 1.5f;

        float feetDistance = global.localPlayer.position.CalculateDistance(feet);
        float headDistance = global.localPlayer.position.CalculateDistance(head);

        float textDistance = feetDistance / headDistance;

        float headHeight = (feet.y - boneHead.y) / 14;

        std::string weaponName;

        if (!global.player.weapon.find("weapon_"))
        {
            weaponName = global.player.weapon;
            weaponName = weaponName.substr(7);
        }

        global.player.circleRadius = abs(feet.y - head.y) / 80;

        float Rbar = min((2.0f * (100 - global.player.health)) / 100.0f, 1.0f);
        float Gbar = min((2.0f * global.player.health) / 100.0f, 1.0f);
        RGB healthBr = { Rbar, Gbar, 0 };

        //team esp
        if (global.localPlayer.team == global.player.team)
        {
            //2d box
            if (global.features.teamcombo == 1 && global.features.teamenable)
            {
                Render::RectFilled(
                    feet.x - width / 2,
                    head.y,
                    width,
                    height,
                    global.features.teaamcolor,
                    global.features.teamalpha);

                Render::Rect(
                    feet.x - width / 2,
                    head.y,
                    width,
                    height,
                    white,
                    255,
                    1.5);
            }

            //3d boxes
            if (global.features.teamcombo == 2 && global.features.teamenable)
            {

            }

            //corner 
            if (global.features.teamcombo == 3 && global.features.teamenable)
            {
                Render::Line(head.x - width / 2.1, head.y, head.x - width / 5, head.y, global.features.enemycolor, 255, 1.5);
                Render::Line(feet.x + width / 5, head.y, feet.x + width / 2.1, head.y, global.features.enemycolor, 255, 1.5);
                Render::Line(head.x - width / 2.1, feet.y + height / 8.9, head.x - width / 5, feet.y + height / 8.9, global.features.enemycolor, 255, 1.5);
                Render::Line(feet.x + width / 5, feet.y + height / 8.9, feet.x + width / 2.1, feet.y + height / 8.9, global.features.enemycolor, 255, 1.5);
                Render::Line(head.x - width / 2.1, head.y + height / 10, head.x - width / 2.1, head.y, global.features.enemycolor, 255, 1.5);
                Render::Line(feet.x + width / 2.1, head.y + height / 10, feet.x + width / 2.1, head.y, global.features.enemycolor, 255, 1.5);
                Render::Line(head.x - width / 2.1, feet.y - height / 40, head.x - width / 2.1, feet.y + height / 8.9, global.features.enemycolor, 255, 1.5);
                Render::Line(feet.x + width / 2.1, feet.y - height / 40, feet.x + width / 2.1, feet.y + height / 8.9, global.features.enemycolor, 255, 1.5);
            }

            //bone esp
            if (global.features.teamskel)
            {
                entityloop.Bone();
            }

            //head
            if (global.features.teamhead)
            {
                Render::Circle(
                    boneHead.x,
                    boneHead.y,
                    headHeight,
                    global.features.teamHeaadColor,
                    global.features.teamHeadAlpha,
                    0
                );
            }

            //snap lines
            if (global.features.teamsnap)
            {
                Render::Line(feet.x, feet.y, screenWidth / 2, screenHeight, global.features.teamSnapColor, global.features.teamSnapAlpha, 1.5f);
            }

            //health
            if (global.features.teamhealth)
            {
                //shadow
                Render::RectFilled(
                    feet.x - (width / 1.77),
                    head.y,
                    width / 30,
                    height,
                    white,
                    255
                );
                //health render
                Render::RectFilled(
                    feet.x - (width / 1.78),
                    head.y + (height * (100 - global.player.health) / 100),
                    width / 40,
                    height - (height * (100 - global.player.health) / 100),
                    healthBr,
                    200
                );
            }

            //armor
            if (global.features.teamarmor)
            {
                //shadow
                Render::RectFilled(
                    feet.x + (width / 1.9),
                    head.y,
                    width / 30,
                    height,
                    white,
                    255
                );

                //armor render
                Render::RectFilled(
                    feet.x + (width / 1.890),
                    head.y + (height * (100 - global.player.armor) / 100),
                    width / 40,
                    height - (height * (100 - global.player.armor) / 100),
                    blue,
                    200
                );
            }

            //weapon 
            if (global.features.teamweapon)
            {
                Render::Text(10.f / textDistance, feet.x - width / 2, head.y + height, white, 255, weaponName);
            }

            //name
            if (global.features.teamname)
            {
                Render::Text(10.f / textDistance, feet.x - width / 2, head.y - 10, white, 255, global.player.name);
            }
        }

        //enemy esp
        if (global.localPlayer.team != global.player.team)
        {
            //2d box
            if (global.features.enemycombo == 1 && global.features.enemyenable)
            {
                Render::RectFilled(
                    feet.x - width / 2,
                    head.y,
                    width,
                    height,
                    global.features.enemycolor,
                    global.features.enemyalpha);

                Render::Rect(
                    feet.x - width / 2,
                    head.y,
                    width,
                    height,
                    white,
                    255,
                    1.5
                );
            }

            //3d boxes
            if (global.features.enemycombo == 2 && global.features.enemyenable)
            {

            }

            //corner 
            if (global.features.enemycombo == 3 && global.features.enemyenable)
            {
                Render::Line(head.x - width / 2.1, head.y, head.x - width / 5, head.y, global.features.enemycolor, 255, 1.5);
                Render::Line(feet.x + width / 5, head.y, feet.x + width / 2.1, head.y, global.features.enemycolor, 255, 1.5);
                Render::Line(head.x - width / 2.1, feet.y + height / 8.9, head.x - width / 5, feet.y + height / 8.9, global.features.enemycolor, 255, 1.5);
                Render::Line(feet.x + width / 5, feet.y + height / 8.9, feet.x + width / 2.1, feet.y + height / 8.9, global.features.enemycolor, 255, 1.5);
                Render::Line(head.x - width / 2.1, head.y + height / 10, head.x - width / 2.1, head.y, global.features.enemycolor, 255, 1.5);
                Render::Line(feet.x + width / 2.1, head.y + height / 10, feet.x + width / 2.1, head.y, global.features.enemycolor, 255, 1.5);
                Render::Line(head.x - width / 2.1, feet.y - height / 40, head.x - width / 2.1, feet.y + height / 8.9, global.features.enemycolor, 255, 1.5);
                Render::Line(feet.x + width / 2.1, feet.y - height / 40, feet.x + width / 2.1, feet.y + height / 8.9, global.features.enemycolor, 255, 1.5);
            }

            //bone esp
            if (global.features.enemyskel)
            {
                entityloop.Bone();
            }

            //head
            if (global.features.enemyhead)
            {
                Render::Circle(
                    boneHead.x,
                    boneHead.y,
                    headHeight,
                    global.features.enemyHeaadColor,
                    global.features.enemyHeadAlpha,
                    0
                );
            }

            //snap lines
            if (global.features.enemysnap)
            {
                Render::Line(feet.x, feet.y, screenWidth / 2, screenHeight, global.features.enemySnapColor, global.features.enemySnapAlpha, 1.5f);
            }

            //health
            if (global.features.enemyhealth)
            {
                //shadow
                Render::RectFilled(
                    feet.x - (width / 1.77),
                    head.y,
                    width / 30,
                    height,
                    white,
                    255
                );
                //health render
                Render::RectFilled(
                    feet.x - (width / 1.78),
                    head.y + (height * (100 - global.player.health) / 100),
                    width / 40,
                    height - (height * (100 - global.player.health) / 100),
                    healthBr,
                    200
                );
            }

            //armor
            if (global.features.enemyarmor)
            {
                //shadow
                Render::RectFilled(
                    feet.x + (width / 1.9),
                    head.y,
                    width / 30,
                    height,
                    white,
                    255
                );

                //armor render
                Render::RectFilled(
                    feet.x + (width / 1.890),
                    head.y + (height * (100 - global.player.armor) / 100),
                    width / 40,
                    height - (height * (100 - global.player.armor) / 100),
                    blue,
                    200
                );
            }

            //weapon 
            if (global.features.enemyweapon)
            {
                Render::Text(10.f / textDistance, feet.x - width / 2, head.y + height, white, 255, weaponName);
            }

            //name
            if (global.features.enemyname)
            {
                Render::Text(10.f / textDistance, feet.x - width / 2, head.y - 10, white, 255, global.player.name);
            }
        }
    }
}

void CEntityLoop::RenderFov()
{
    std::string weaponName = global.localPlayer.weaponName;
    int weaponGroup = CLocal::GetWeaponGroup(weaponName);

    //AR
    if (weaponGroup == 1)
        if (global.features.ARaimbotfovcircle)
        {
            Render::Circle(screenWidth / 2, screenHeight / 2, global.features.ARaimbotfov * 1.7, white, 255, false);
            if (aimbot.showBoneAngle)
                Render::Line(screenWidth / 2, screenHeight / 2, aimbot.boneAngle.x, aimbot.boneAngle.y, green, 255, 1.5);
        }

    //SG
    if (weaponGroup == 2)
        if (global.features.SGaimbotfovcircle)
        {
            Render::Circle(screenWidth / 2, screenHeight / 2, global.features.SGaimbotfov * 1.7, white, 255, false);
            if (aimbot.showBoneAngle)
                Render::Line(screenWidth / 2, screenHeight / 2, aimbot.boneAngle.x, aimbot.boneAngle.y, green, 255, 1.5);
        }

    //Pistols
    if (weaponGroup == 3)
        if (global.features.PSaimbotfovcircle)
        {
            Render::Circle(screenWidth / 2, screenHeight / 2, global.features.PSaimbotfov * 1.7, white, 255, false);
            if (aimbot.showBoneAngle)
                Render::Line(screenWidth / 2, screenHeight / 2, aimbot.boneAngle.x, aimbot.boneAngle.y, green, 255, 1.5);
        }

    //SR
    if (weaponGroup == 4)
        if (global.features.SRaimbotfovcircle)
        {
            Render::Circle(screenWidth / 2, screenHeight / 2, global.features.SRaimbotfov * 1.7, white, 255, false);
            if (aimbot.showBoneAngle)
                Render::Line(screenWidth / 2, screenHeight / 2, aimbot.boneAngle.x, aimbot.boneAngle.y, green, 255, 1.5);
        }

    //SMG
    if (weaponGroup == 5)
        if (global.features.SMGaimbotfovcircle)
        {
            Render::Circle(screenWidth / 2, screenHeight / 2, global.features.SMGaimbotfov * 1.7, white, 255, false);
            if (aimbot.showBoneAngle)
                Render::Line(screenWidth / 2, screenHeight / 2, aimbot.boneAngle.x, aimbot.boneAngle.y, green, 255, 1.5);
        }

}