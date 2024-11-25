#include <iostream>
#include <windows.h>
#include <WinUser.h>
#include <vector>
#include <thread>
#include "Cheats\Esp.h"
#include "Cheats\Aimbot.h"
#include "Cheats\mem.h"
#include "include/imgui_hook.h"
#include "include/imgui/imgui.h"
#include "include/minhook/include/MinHook.h"

BYTE* ModuleBaseAddress;

struct OCurrentWeapon {
	int CurrentWeaponAmmo;			//0x0 - 0x4
	char buffer[80];				//0x0 - 0x54
	int KickBack;					//0x54 - 0x58
	char buffer2[6];				//0x58 - 0x5E
	uint16_t RecoilOne;				//0x5E
	uint16_t RecoilTwo;				//0x60
};

struct OInventory {
	char Name[12];							// 0x0 - 0xC
	OCurrentWeapon* CurrentWeaponRecoil;	// 0xC - 0x10
	OCurrentWeapon* CurrentWeapon;			//0x10 - 0x14
	OCurrentWeapon* CurrentWeaponMain;		//0x14 - 0x20
};

class Entity
{
public: 
	char pad_0000[4];					//0x0000 - 0x04
	Vector3 HeadPos;					//0x0004 - 0x10
	Vector3 Velocity;					//0x0010 - 0x1C
	char pad_001C[12];					//0x001C - 0x28
	Vector3 PlayerPos;					//0x0028 - 0x34
	Vector3 ViewAngles;					//0x0034 - 0x40
	char pad_idk[25];					//0x40 - 0x5D
	int32_t OnGround;					//0x5D - 0x61
	char pad_idk2[19];					//0x61 -0x74
	uint16_t SpeedHacks;				//0x74 - 0x76
	uint16_t NoClip;					//0x76 - 0x78
	char pad_0040[116];					//0x78 - 0xEC
	int32_t PlayerHealth;				//0xEC - 0xF0
	int32_t Armor;						//0x00F0
	char pad_00F4[20];					//0x00F4
	int32_t PistolAmmo2;				//0x0108
	char pad_010C[16];					//0x010C
	int32_t AssaultRifleAmmo2;			//0x011C
	char pad_0120[12];					//0x0120
	int32_t PistolAmmo1;				//0x012C
	char pad_0130[16];					//0x0130
	int32_t AssaultRifleAmmo1;			//0x0140
	char pad_0144[12];					//0x0144
	int32_t PistolReloadDelay;			//0x0150
	char pad_0154[16];					//0x0154
	int32_t AssaultRifleReloadDelay;	//0x0164
	char pad_0168[12];					//0x0168
	int32_t AmountOfShotsFired;			//0x0174
	char pad_0178[100];					//0x0178
	int32_t BlueTeamScore;				//0x01DC
	char pad_01E0[37];					//0x01E0
	char Name[16];						//0x0205
	char pad_0215[247];					//0x0215
	int32_t Team;						//0x030C
	char pad_030C[8];					//0x314
	int32_t IsAlive;					//0x318 - 0x31C
	char pad_0302[76];					//0x368
	OInventory* Inventory;				//0x368 - 0x37C

	void SetCurrentWeaponAmmo(int ammo) {
		Inventory->CurrentWeaponMain->CurrentWeaponAmmo = ammo;
	}

	void SetCurrentWeaponAmmo2(int ammo){
		Inventory->CurrentWeapon->CurrentWeaponAmmo = ammo;
	}
};

//bools
bool ToggleMenu{ false };
bool ToggleAntiRecoil{ false };
bool ToggleRapidFire{ false };
bool ToggleAntiRecoilCheck{ false };
bool ToggleAimBot{ false };
bool ToggleEsp{ false };
bool ToggleUnlimitedAmmo{ false };
bool ToggleUnlimitedHealth{ false };
bool ToggleNoClip{ false };
bool ToggleSpeedBoost{ false };
bool ToggleBulletTPToLocation{ false };
bool ToggleActualBulletTP{ false };

//Vectors
Vector3 BulletTP;

//Key bind
int SpeedBoostActivationKey = VK_LSHIFT;
int NoClipActivationKey = 0x48;

//For ESP
DWORD viewMatrix = 0x0057DFD0;
float Matrix[16];

//Teleport Values
static char buf1[64] = "";
static char buf2[64] = "";
static char buf3[64] = "";

Entity* GlobalEntity = nullptr;

int Hypotenuse(Entity* LocalPlayer, Entity* EnemyPlayer) {
	return sqrt(pow((LocalPlayer->HeadPos.x - EnemyPlayer->HeadPos.x), 2) + pow(LocalPlayer->HeadPos.y - EnemyPlayer->HeadPos.y, 2));
}

void block_input(bool block) {
	if (block) {
		mem::Nop((BYTE*)0x004F1C0F, 7);
		mem::Nop((BYTE*)0x004F1BF4, 2);
		mem::Patch((BYTE*)0x0593F19, (BYTE*)"\x01", 1);
		mem::Patch((BYTE*)0x004F1B65, (BYTE*)"\xEB", 1);
		mem::Patch((BYTE*)0x00593F12, (BYTE*)"\x00", 1);
		mem::Patch((BYTE*)0x00593F13, (BYTE*)"\x00", 1);
	}
	else {
		mem::Patch((BYTE*)0x004F1C0F, (BYTE*)"\xC6\x05\x19\x3F\x59\x00\x00", 7);//
		mem::Patch((BYTE*)0x004F1BF4, (BYTE*)"\x74\x20", 2);//
		mem::Patch((BYTE*)0x0593F19, (BYTE*)"\x00", 1);
		mem::Patch((BYTE*)0x004F1B65, (BYTE*)"\x74", 1);
		mem::Patch((BYTE*)0x00593F12, (BYTE*)"\x01", 1);
		mem::Patch((BYTE*)0x00593F13, (BYTE*)"\x01", 1);
	}
}

typedef int(__thiscall* tgunAttackFXOriginal)(void* thisPtr, float* from, float* to, int a4);
tgunAttackFXOriginal ogunAttackFXOriginal;

int __fastcall gunAttackFXHook(void* thisPtr, void* edx, float* from, float* to, int a4) {
	if (ToggleBulletTPToLocation){
		BulletTP.x = to[0];
		BulletTP.y = to[1];
		BulletTP.z = to[2];
		std::cout << "Teleported" << "\n";
		std::cout << "X: " << BulletTP.x << "\n";
		std::cout << "Y: " << BulletTP.y << "\n";
		std::cout << "Z: " << BulletTP.z << "\n\n";
	}
	return ogunAttackFXOriginal(thisPtr, from, to, a4);
}

double __fastcall hRECOIL(VOID* Vthis, void* Unknown) {
	return 0;}
int __fastcall hSPREAD(VOID* Vthis, void* Unknown) {
	return 0;}


typedef int(__fastcall* tRayDamage)(float* from, float* to, int d);
tRayDamage oRayDamage;

int __fastcall RayDamageHook(float* from, float* to, int d) {

	if (ToggleActualBulletTP) {
		to[0] = GlobalEntity->PlayerPos.x;
		to[1] = GlobalEntity->PlayerPos.y;
		to[2] = GlobalEntity->PlayerPos.z;
	}

	return oRayDamage(from, to, d);
}

void RenderMain(){
	AllocConsole();
	FILE* f;
	freopen_s(&f, "CONOUT$", "w", stdout);

	memcpy(&Matrix, (PBYTE*)(viewMatrix), sizeof(Matrix));
	uintptr_t* LocalPlayerPtr = (uintptr_t*)(ModuleBaseAddress + 0x18AC00);
	Entity* LocalPlayer = *(Entity**)(ModuleBaseAddress + 0x18AC00);				
	Vector<Entity*> EntityList = *(Vector<Entity*>*)(ModuleBaseAddress + 0x18AC04); 

	if (GetAsyncKeyState(VK_INSERT) & 1) {
		ToggleMenu = !ToggleMenu;
		block_input(ToggleMenu);
	}
	if (ToggleMenu) {
		ImGui::SetNextWindowSize(ImVec2(400, 300));
		ImGui::Begin("Assault Cube Cheat Menu ");
		ImGui::Checkbox("Aimbot", &ToggleAimBot);
		ImGui::Checkbox("Esp", &ToggleEsp);
		ImGui::Checkbox("Anti-Recoil", &ToggleAntiRecoil);
		ImGui::Checkbox("Rapid-Fire", &ToggleRapidFire);
		ImGui::Checkbox("Unlimited Ammo", &ToggleUnlimitedAmmo);
		ImGui::Checkbox("Unlimited Health", &ToggleUnlimitedHealth);
		ImGui::Checkbox("GunTP", &ToggleBulletTPToLocation);
		ImGui::Checkbox("BulletTP", &ToggleActualBulletTP);
		ImGui::Checkbox("NoClip", &ToggleNoClip);
		ImGui::SameLine();
		ImGui::Hotkey(&NoClipActivationKey, ImVec2(70, 20));
		ImGui::Checkbox("SpeedBoost", &ToggleSpeedBoost);
		ImGui::SameLine();
		ImGui::Hotkey(&SpeedBoostActivationKey, ImVec2(70, 20));
		ImGui::Text("TELPORT TO -> ");
		ImGui::SetNextItemWidth(50); 
		ImGui::InputText("X", buf1, IM_ARRAYSIZE(buf1)); 
		ImGui::SameLine();
		ImGui::SetNextItemWidth(50); 
		ImGui::InputText("Y", buf2, IM_ARRAYSIZE(buf2));
		ImGui::SameLine();
		ImGui::SetNextItemWidth(50); 
		ImGui::InputText("Z", buf3, IM_ARRAYSIZE(buf3));
		ImGui::SameLine();
		if (ImGui::Button("Teleport Now!", ImVec2(100, 20))) {
			int x = std::atoi(buf1);
			int y = std::atoi(buf2);
			int z = std::atoi(buf3);

			std::cout << "Teleporting..." << std::endl;
			Sleep(100);
			LocalPlayer->PlayerPos.x = x;
			LocalPlayer->PlayerPos.y = y;
			LocalPlayer->PlayerPos.z = z;

		}
		ImGui::End();
	}

	int minDistance = 100000;
	Entity* ClosestEntity = nullptr;
	
	for (int i = 0; i < EntityList.ulen; i++) 
	{
		Entity* entities = EntityList.Buffer[i];
		
		if (entities)
		{
			if ((entities->Team != LocalPlayer->Team && entities->IsAlive == 0) || GlobalEntity == nullptr) {
					GlobalEntity = entities;
			}
		
			Vector2 ScreenHead, ScreenFoot;
			if ((WorldToScreen(entities->PlayerPos, ScreenFoot, Matrix, 600, 500)) && (WorldToScreen(entities->HeadPos, ScreenHead, Matrix, 600, 500)) && entities->IsAlive == 0 && ToggleEsp) {
				float Height = ScreenHead.y - ScreenFoot.y;
				float width = Height / 4;
				float ExtraRoom = Height / 16;
				float ExtraHeadRoom = Height / 6;
				
				if (entities->IsAlive == 0)
				{	
					std::string EnemyName = entities->Name;
					const char* enemyNameCStr = EnemyName.c_str();
					ImFont* font = ImGui::GetIO().Fonts->Fonts[0];
					float fontSize = 12.0f;  

					ImGui::GetBackgroundDrawList()->AddText(font, fontSize, ImVec2(ScreenHead.x + width, ScreenHead.y + (ExtraHeadRoom + ExtraRoom*3)), ImColor(1.f, 1.f, 1.f), enemyNameCStr);
					ImGui::GetBackgroundDrawList()->AddRect(ImVec2{ ScreenHead.x + width, ScreenHead.y + ExtraHeadRoom }, ImVec2{ ScreenFoot.x - width, ScreenFoot.y - ExtraHeadRoom }, ImColor(225, 255, 255));

				float healthPercentage = static_cast<float>(entities->PlayerHealth) / 100.0f;
				float barHeight = ScreenFoot.y - ScreenHead.y - 2 * ExtraHeadRoom;
				float filledHeight = barHeight * healthPercentage;

				ImGui::GetBackgroundDrawList()->AddRectFilled(ImVec2{ ScreenHead.x - width, ScreenHead.y + ExtraHeadRoom }, ImVec2{ ScreenHead.x - (width + ExtraRoom), ScreenFoot.y - ExtraHeadRoom }, ImColor(255, 0, 0));
				ImGui::GetBackgroundDrawList()->AddRectFilled(ImVec2{ ScreenHead.x - width, ScreenFoot.y - ExtraHeadRoom - filledHeight }, ImVec2{ ScreenHead.x - (width + ExtraRoom), ScreenFoot.y - ExtraHeadRoom }, ImColor(0, 255, 0));
				}
			}

			if (Hypotenuse(LocalPlayer, entities) < minDistance) {
				minDistance = Hypotenuse(LocalPlayer, entities);
				ClosestEntity = entities;
			}
		}
	}

	if (GetAsyncKeyState(0x6) && ToggleAimBot && ClosestEntity->IsAlive == 0) {
		std::pair<float, float> AimbotAngles = AimbotAngleYawAndPitch(ClosestEntity->PlayerPos, LocalPlayer->PlayerPos);
		LocalPlayer->ViewAngles.x = AimbotAngles.first;
		LocalPlayer->ViewAngles.y = AimbotAngles.second;
	}

	//Anti Recoil
	if (ToggleAntiRecoil) {
		if (!ToggleAntiRecoilCheck)
		{
			mem::Nop((BYTE*)(0x004C8E3B), 4);
			MH_CreateHook((BYTE*)0x004C8860, hRECOIL, NULL); 
			MH_CreateHook((BYTE*)0x004C66A0, hRECOIL, NULL);
			MH_CreateHook((BYTE*)0x004C66E0, hSPREAD, NULL); 
			MH_CreateHook((BYTE*)0x004C8870, hSPREAD, NULL); 
			MH_EnableHook(MH_ALL_HOOKS);
			ToggleAntiRecoilCheck = true;
		}
	}
	else {
		if (ToggleAntiRecoilCheck) {
			BYTE originalBytes[] = { 0xF3, 0x0F, 0x2A , 0xC0 };
			mem::Patch((BYTE*)(0x004C8E3B), originalBytes, sizeof(originalBytes));
			MH_RemoveHook((BYTE*)0x004C8860);
			MH_RemoveHook((BYTE*)0x004C66A0);
			MH_RemoveHook((BYTE*)0x004C66E0);
			MH_RemoveHook((BYTE*)0x004C8870);
			ToggleAntiRecoilCheck = false;
		}
	}

	//Rapid Fire
	if (ToggleRapidFire) {
		LocalPlayer->AssaultRifleReloadDelay = 0;
		LocalPlayer->PistolReloadDelay = 0;
	}

	//BulletTeleportToLocation
	if (ToggleBulletTPToLocation && GetAsyncKeyState(0x01) & 1) {
		LocalPlayer->PlayerPos.x = BulletTP.x;
		LocalPlayer->PlayerPos.y = BulletTP.y;
		LocalPlayer->PlayerPos.z = BulletTP.z;
	}

	//Unlimited Ammo
	if (ToggleUnlimitedAmmo) {
		LocalPlayer->SetCurrentWeaponAmmo(9999);
		LocalPlayer->SetCurrentWeaponAmmo2(9999);
	}

	//Unlimited Health
	if (ToggleUnlimitedHealth) {
		LocalPlayer->PlayerHealth = 9999;
	}

	//NoClip
	if (ToggleNoClip && GetKeyState(NoClipActivationKey)) {
		LocalPlayer->NoClip = 4;
	}
	else if (ToggleNoClip && (!GetKeyState(NoClipActivationKey))) {
		if (LocalPlayer->NoClip == 4) {
			LocalPlayer->NoClip = 0;
		}
	}

	//Speed Boost
	if (ToggleSpeedBoost && GetKeyState(SpeedBoostActivationKey)) {
		LocalPlayer->SpeedHacks = 2;
	}
	else if (ToggleSpeedBoost && (!GetKeyState(SpeedBoostActivationKey)))
	{
		if (LocalPlayer->SpeedHacks == 2) {
			LocalPlayer->SpeedHacks = 0;
		}
	}

	Sleep(1);
}


BOOL APIENTRY DllMain(HMODULE hModule, DWORD  ul_reason_for_call, LPVOID lpReserved)
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:	
		DisableThreadLibraryCalls(hModule);
		AllocConsole();
		FILE* f;
		freopen_s(&f, "CONOUT$", "w", stdout);
		ModuleBaseAddress = (BYTE*)GetModuleHandleA(0);
		MH_Initialize();
		MH_CreateHook((BYTE*)0x004C7140, gunAttackFXHook, (LPVOID*)&ogunAttackFXOriginal); 
		MH_CreateHook((BYTE*)0x004C9510, RayDamageHook, (LPVOID*)&oRayDamage);
		MH_EnableHook(MH_ALL_HOOKS);
		std::cout << "DLL INJECTED" << std::endl;
		ImGuiHook::Load(RenderMain);
		break;
	case DLL_PROCESS_DETACH:
		break;
	}
	return TRUE;
}

