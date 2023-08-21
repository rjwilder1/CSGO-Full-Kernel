#pragma once
#include "Modules.h"
#include "Drawing.h"
#include "Mouse.h"
#include "Offsets.h"
#include "Keymap.h"
#include "Weapons.h"
BOOL ESPEXTRAS = FALSE;
BOOL AIMBOT = true;
BOOL ShowFOV = true;
BOOL TriggerBot = false;
int fovrange = 40;

void ExitThread()
{
	Print("Exiting driver...");
	if (isWin32Thread)
		UnspoofWin32Thread();

	PsTerminateSystemThread(STATUS_SUCCESS);
}

void lclick()
{
	MouseClick(MOUSE_LEFT_BUTTON_DOWN, 0, 0);
	MouseClick(MOUSE_LEFT_BUTTON_UP, 0, 0);
}

void aimbot(Vector2 loc, int rx, int ry)
{
	if (iszero(loc) == FALSE && AIMBOT)
	{
		long relativeX = loc.x - rx;
		long relativeY = loc.y - ry;
		MouseMove(relativeX, relativeY + 5);
	}
}

bool SaveWhileLoop()
{
	if (!UpdateKeyMap())
		ExitThread();

	if (KeyDown(VK_NUMPAD5))
	{
		ShowFOV = !ShowFOV;
		Sleep(200);
	}

	if (KeyDown(VK_HOME))
	{
		AIMBOT = !AIMBOT;
		Sleep(200);
	}
	
	if (KeyDown(VK_XBUTTON1))
	{
		TriggerBot = !TriggerBot;
		Sleep(200);
	}

	if (KeyDown(VK_OEM_PLUS))
	{
		fovrange = fovrange + 4;
		Sleep(100);
	}

	if (KeyDown(VK_RSHIFT))
	{
		ESPEXTRAS = !ESPEXTRAS;
		Sleep(200);
	}

	if (KeyDown(VK_OEM_MINUS))
	{
		if (fovrange <= 3) {
			fovrange = 2;
		}
		else { 
			fovrange = fovrange - 4; 
		}
		Sleep(100);
	}

	if (!KeyDown(VK_END))
	{
		return true;
	}
	ExitThread();
}

void ConvertCharToWChar(const char* source, LPWSTR& destination)
{
	ANSI_STRING ansiString;
	UNICODE_STRING unicodeString;

	RtlInitAnsiString(&ansiString, source);
	RtlAnsiStringToUnicodeString(&unicodeString, &ansiString, TRUE);

	destination = unicodeString.Buffer;
}

void CsgoMain()
{
    Print("Starting Main");

    while (SaveWhileLoop())
    {
        if (!SpoofWin32Thread())
            continue;

        if (!IsWindowFocused("csgo.exe"))
        {
            Sleep(100);
            UnspoofWin32Thread();
            continue;
        }

        hdc = NtUserGetDC(0);
        if (!hdc)
        {
            Print("failed to get userdc");
            UnspoofWin32Thread();
            continue;
        }

        if (AIMBOT)
        {
            const wchar_t* aimbotText = L"Aimbot: On";
            WriteText(hdc, LPWSTR(aimbotText), -10, 1, 13);
        }
        else
        {
            const wchar_t* aimbotText = L"Aimbot: Off";
            WriteText(hdc, LPWSTR(aimbotText), -10, 1, 13);
        }

        if (TriggerBot)
        {
            const wchar_t* triggerbotText = L"Trigger Bot: On";
            WriteText(hdc, LPWSTR(triggerbotText), -10, 1, 26);
        }
        else
        {
            const wchar_t* triggerbotText = L"Trigger Bot: Off";
            WriteText(hdc, LPWSTR(triggerbotText), -10, 1, 26);
        }

        wchar_t fov[50];
        swprintf(fov, L"Current FOV: %d", (int)fovrange);
        WriteText(hdc, fov, -10, 1, 39);

        brush = NtGdiCreateSolidBrush(RGB(255, 255, 0), NULL);
        if (!brush)
        {
            Print("failed create brush");
            NtUserReleaseDC(hdc);
            UnspoofWin32Thread();
            continue;
        }

        DWORD localplayer = ReadMemory<DWORD>(clientBase + dwLocalPlayer);
        DWORD localTeam = ReadMemory<DWORD>(localplayer + m_iTeamNum);

        Vector2 bestpoint = Vector2{};
        float bestdistance = 3.402823E+38f;

        for (size_t i = 0; i < 32; i++)
        {
            DWORD currEnt = ReadMemory<DWORD>(clientBase + dwEntityList + (i * 0x10));
            if (!currEnt)
                continue;

            int entHealth = ReadMemory<int>(currEnt + m_iHealth);
            if (entHealth <= 0)
                continue;

            DWORD dormant = ReadMemory<DWORD>(currEnt + m_bDormant);
            if (dormant)
                continue;

            DWORD teamNum = ReadMemory<DWORD>(currEnt + m_iTeamNum);
            if (teamNum == localTeam)
                continue;

            int crosshairId = ReadMemory<int>(localplayer + m_iCrosshairId);
            DWORD bonePtr = ReadMemory<DWORD>(currEnt + m_dwBoneMatrix);
            MAT3X4 boneMatrix = ReadMemory<MAT3X4>(bonePtr + 0x30 * 8); //head
            Vector3 headPos = { boneMatrix.c[0][3], boneMatrix.c[1][3], boneMatrix.c[2][3] };
            headPos.z += 8.75;
            MAT4X4 viewMatrix = ReadMemory<MAT4X4>(clientBase + dwViewMatrix);
            Vector2 headScreen = WorldToScreen(headPos, viewMatrix);
            Vector3 feetPos = ReadMemory<Vector3>(currEnt + m_vecOrigin);
            Vector2 feetPosScreen = WorldToScreen(feetPos, viewMatrix);
                int height = headScreen.y - feetPosScreen.y;
            int width = height / 4;

            float Entity_x = feetPosScreen.x - width;
            float Entity_y = feetPosScreen.y;
            float Entity_w = height / 2;

            int boxSize = 2;
            int boxX = static_cast<int>(headScreen.x) - boxSize / 2;
            int boxY = static_cast<int>(headScreen.y) - boxSize + 20 / 2;

            RECT boxEsp = { static_cast<LONG>(Entity_x + Entity_w), static_cast<LONG>(Entity_y + height), static_cast<LONG>(Entity_x), static_cast<LONG>(Entity_y) };
            RECT headEsp = { boxX, boxY, boxX + boxSize, boxY + boxSize };

            if (boxEsp.bottom >= 5 && boxEsp.left >= 5)
            {
                //WEAPONS
                DWORD PlayerBase = ReadMemory<DWORD>(clientBase + dwLocalPlayer);
                DWORD getHandleWeap = ReadMemory<DWORD>(currEnt + m_hActiveWeapon);
                int getWeapEnt = getHandleWeap & 0xFFF;
                DWORD getCurrentWeapon = ReadMemory<DWORD>(clientBase + dwEntityList + (getWeapEnt - 1) * 0x10);
                int getWeaponId = ReadMemory<int>(getCurrentWeapon + m_iItemDefinitionIndex);
                const char* weaponName = getWeaponName((enum WeaponIndex)getWeaponId);
                LPWSTR weapon_name_wide;
                ConvertCharToWChar(weaponName, weapon_name_wide);

                // ESP
                FrameRect(hdc, &boxEsp, brush, 1, entHealth, true, weapon_name_wide);
                if (ReadMemory<DWORD>(currEnt + m_bHasHelmet))
                    FrameRect(hdc, &headEsp, brush, 2, 100, false);
                else
                    FrameRect(hdc, &headEsp, brush, 2, 1, false);
                ExFreePool(weapon_name_wide);
            }

            // TRIGGERBOT
            if (TriggerBot)
            {
                int localHealth = ReadMemory<int>(localplayer + m_iHealth);
                if (localHealth >= 1 && crosshairId != 0 && crosshairId < 64)
                {
                    DWORD player = ReadMemory<DWORD>(clientBase + dwEntityList + (crosshairId - 1) * 0x10);
                    if (ReadMemory<int>(player + m_iHealth) >= 1 && ReadMemory<DWORD>(player + m_iTeamNum) != localTeam && !ReadMemory<DWORD>(player + m_bDormant))
                    {
                        KeStallExecutionProcessor(3000);
                        lclick();
                    }
                }
            }

            // AIMBOT
            Vector3 myPos = ReadMemory<Vector3>(localplayer + m_vecOrigin);
            Vector3 enemyPos = ReadMemory<Vector3>(currEnt + m_vecOrigin);
            float distance = getdistance(myPos, enemyPos);

            if (headScreen.x >= 960 - fovrange && headScreen.x <= 960 + fovrange && headScreen.y >= 540 - fovrange && headScreen.y <= 540 + fovrange && headScreen.x >= -1920 && headScreen.x <= 1920 && headScreen.y >= -1080 && headScreen.y <= 1080)
            {
                if (distance < bestdistance)
                {
                    bestdistance = distance;
                    bestpoint = headScreen;
                }
            }

                // Extras
                if (ESPEXTRAS && boxEsp.bottom >= 5 && boxEsp.left >= 5)
                {
                    DWORD hasdefuser = ReadMemory<DWORD>(currEnt + m_bHasDefuser);
                    int armorValue = ReadMemory<int>(currEnt + m_ArmorValue);

                    const wchar_t* ydefuser = L"Defuser: Yes";
                    const wchar_t* ndefuser = L"Defuser: No";
                    if (hasdefuser)
                        WriteText(hdc, LPWSTR(ydefuser), boxEsp.bottom / 45, boxEsp.left, boxEsp.bottom + (3 * (boxEsp.bottom / 45)));
                    else
                        WriteText(hdc, LPWSTR(ndefuser), boxEsp.bottom / 45, boxEsp.left, boxEsp.bottom + (3 * (boxEsp.bottom / 45)));

                    wchar_t armor[20];
                    swprintf(armor, L"Armor: %d", armorValue);
                    WriteText(hdc, armor, boxEsp.bottom / 45, boxEsp.left, boxEsp.bottom + (2 * (boxEsp.bottom / 45)));

                    const wchar_t* yHelmet = L"Helmet: Yes";
                    const wchar_t* nHelmet = L"Helmet: No";
                    if (ReadMemory<DWORD>(currEnt + m_bHasHelmet))
                        WriteText(hdc, LPWSTR(yHelmet), boxEsp.bottom / 45, boxEsp.left, boxEsp.bottom + (4 * (boxEsp.bottom / 45)));
                    else
                        WriteText(hdc, LPWSTR(nHelmet), boxEsp.bottom / 45, boxEsp.left, boxEsp.bottom + (4 * (boxEsp.bottom / 45)));
                }
        }

        // AIMBOT
        if (AIMBOT)
        {
            if (ShowFOV)
            {
                RECT rect = { 960 - fovrange, 540 - fovrange, 960 + fovrange, 540 + fovrange };
                FrameRect(hdc, &rect, brush, 2, 100, false);
            }

            if (KeyDown(VK_XBUTTON2) && !iszero(bestpoint))
            {
                if (bestpoint.x >= 960 - fovrange && bestpoint.x <= 960 + fovrange && bestpoint.y >= 540 - fovrange && bestpoint.y <= 540 + fovrange)
                {
                    Vector3 punchAngle = ReadMemory<Vector3>(localplayer + m_aimPunchAngle);
                    punchAngle.x = punchAngle.x * 12;
                    punchAngle.y = punchAngle.y * 12;
                    float x = targetWindowWidth / 2 - punchAngle.y;
                    float y = targetWindowHeight / 2 + punchAngle.x;
                    aimbot(bestpoint, static_cast<int>(x), static_cast<int>(y));
                    KeStallExecutionProcessor(600);
                }
            }
        }

        NtGdiDeleteObjectApp(brush);
        NtUserReleaseDC(hdc);
        UnspoofWin32Thread();
        YieldProcessor();
    }
}