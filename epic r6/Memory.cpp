#include <Windows.h>
#include <Psapi.h>
#include "Memory.h"
#include "Globals.h"
#include "Offsets.h"
#include <cmath>

Memory::Memory() {};

BOOL Memory::SetBaseAddress() {
	HMODULE hMods[1024];
	DWORD cbNeeded;
	unsigned int i;

	if (EnumProcessModules(GameHandle, hMods, sizeof(hMods), &cbNeeded))
	{
		for (i = 0; i < (cbNeeded / sizeof(HMODULE)); i++)
		{
			TCHAR szModName[MAX_PATH];
			if (GetModuleFileNameEx(GameHandle, hMods[i], szModName, sizeof(szModName) / sizeof(TCHAR)))
			{
				std::wstring wstrModName = szModName;
				std::wstring wstrModContain = L"RainbowSix.exe";
				if (wstrModName.find(wstrModContain) != std::string::npos)
				{
					BaseAddress = hMods[i];
					return true;
				}
			}
		}
	}
	return false;
}

DWORD_PTR Memory::GetBaseAddress() {
	return (DWORD_PTR)BaseAddress;
}

template<typename T> T Memory::RPM(SIZE_T address) {
	//The buffer for data that is going to be read from memory
	T buffer;

	//The actual RPM
	ReadProcessMemory(GameHandle, (LPCVOID)address, &buffer, sizeof(T), NULL);

	//Return our buffer
	return buffer;
}

template<typename T> void Memory::WPM(SIZE_T address, T buffer) {
	//A couple checks to try and avoid crashing
	if (address == 0 || (LPVOID)address == nullptr || address == NULL) {
		return;
	}
	WriteProcessMemory(GameHandle, (LPVOID)address, &buffer, sizeof(buffer), NULL);
}

void Memory::UpdateAddresses() {
	pGameManager = RPM<DWORD_PTR>(GetBaseAddress() + ADDRESS_GAMEMANAGER);
	pEntityList = RPM<DWORD_PTR>(pGameManager + OFFSET_GAMEMANAGER_ENTITYLIST);
	pRender = RPM<DWORD_PTR>(GetBaseAddress() + ADDRESS_GAMERENDERER);
	pGameRender = RPM<DWORD_PTR>(pRender + OFFSET_GAMERENDERER_DEREF);
	pEngineLink = RPM<DWORD_PTR>(pGameRender + OFFSET_GAMERENDERER_ENGINELINK);
	pEngine = RPM<DWORD_PTR>(pEngineLink + OFFSET_ENGINELINK_ENGINE);
	pCamera = RPM<DWORD_PTR>(pEngine + OFFSET_ENGINE_CAMERA);
}

DWORD_PTR Memory::GetEntity(int i) {
	DWORD_PTR entityBase = RPM<DWORD_PTR>(pEntityList + (i * OFFSET_GAMEMANAGER_ENTITY));
	return RPM<DWORD_PTR>(entityBase + 0x20);
}

DWORD_PTR Memory::GetEntity2(int i) 
{
	DWORD_PTR entityBase = RPM<DWORD_PTR>(pEntityList + (i * OFFSET_GAMEMANAGER_ENTITY));
	return entityBase;
}

float Memory::GetEntityHealth(DWORD_PTR entity) 
{
	//Entity info pointer from the Entity
	DWORD_PTR EntityInfo = RPM<DWORD_PTR>(entity + OFFSET_ENTITY_ENTITYINFO);
	//Main component pointer from the entity info
	DWORD_PTR MainComponent = RPM<DWORD_PTR>(EntityInfo + OFFSET_ENTITYINFO_MAINCOMPONENT);
	//Child component pointer form the main component
	DWORD_PTR ChildComponent = RPM<DWORD_PTR>(MainComponent + OFFSET_MAINCOMPONENT_CHILDCOMPONENT);

	//Finally health from the child component
	float kek = RPM<float>(ChildComponent + OFFSET_CHILDCOMPONENT_HEALTH_FLOAT);
	kek *= 100;
	kek = round(kek);
	return kek;
}

Vector3 Memory::GetEntityFeetPosition(DWORD_PTR entity) {
	//We get the feet position straight from the entity
	return RPM<Vector3>(entity + OFFSET_ENTITY_FEET);
}

Vector3 Memory::GetEntityHeadPosition(DWORD_PTR entity) {
	//We get the head position straight from the entity
	return RPM<Vector3>(entity + OFFSET_ENTITY_HEAD);
}

Vector3 Memory::GetEntityNeckPosition(DWORD_PTR entity) {
	//We get the neck position straight from the entity
	return RPM<Vector3>(entity + OFFSET_ENTITY_NECK);
}

Vector3 Memory::GetEntityHandPosition(DWORD_PTR entity) {
	//We get the hand position straight from the entity
	return RPM<Vector3>(entity + OFFSET_ENTITY_RIGHT_HAND);
}

Vector3 Memory::GetEntityChestPosition(DWORD_PTR entity) {
	//We get the chest position straight from the entity
	return RPM<Vector3>(entity + OFFSET_ENTITY_CHEST);
}

Vector3 Memory::GetEntityStomachPosition(DWORD_PTR entity) {
	//We get the stomach position straight from the entity
	return RPM<Vector3>(entity + OFFSET_ENTITY_STOMACH);
}

Vector3 Memory::GetEntityPelvisPosition(DWORD_PTR entity) {
	//We get the pelvis position straight from the entity
	return RPM<Vector3>(entity + OFFSET_ENTITY_PELVIS);
}

PlayerInfo Memory::GetAllEntityInfo(DWORD_PTR entity) {
	PlayerInfo p;

	p.Health = GetEntityHealth(entity);
	p.Position = GetEntityFeetPosition(entity);
	p.w2sPos = WorldToScreen(p.Position);
	p.HeadPos = GetEntityHeadPosition(entity);
	p.w2sHead = WorldToScreen(p.HeadPos);
	p.ScrenTop = WorldToScreen(Vector3(p.HeadPos.x, p.HeadPos.y, p.HeadPos.z + 0.2));

	p.w2sNeck = WorldToScreen(GetEntityNeckPosition(entity));
	p.w2sRHand = WorldToScreen(GetEntityHandPosition(entity));
	p.w2sChest = WorldToScreen(GetEntityChestPosition(entity));
	p.w2sStomach = WorldToScreen(GetEntityStomachPosition(entity));
	p.w2sPelvis = WorldToScreen(GetEntityPelvisPosition(entity));

	return p;
}

Vector3 Memory::GetViewTranslation() {
	//View translation comes straight from the camera
	return RPM<Vector3>(pCamera + OFFSET_CAMERA_VIEWTRANSLATION);
}

Vector3 Memory::GetViewRight() {
	//View right comes directly from the camera
	return RPM<Vector3>(pCamera + OFFSET_CAMERA_VIEWRIGHT);
}

Vector3 Memory::GetViewUp() {
	//View up comes directly from the camera
	return RPM<Vector3>(pCamera + OFFSET_CAMERA_VIEWUP);
}

Vector3 Memory::GetViewForward() {
	//View forward comes directly from the camera
	return RPM<Vector3>(pCamera + OFFSET_CAMERA_VIEWFORWARD);
}

float Memory::GetFOVX() {
	//FOV comes directly from the camera
	return RPM<float>(pCamera + OFFSET_CAMERA_VIEWFOVX);
}

float Memory::GetFOVY() {
	//FOV comes directly from the camera
	return RPM<float>(pCamera + OFFSET_CAMERA_VIEWFOVY);
}

Vector3 Memory::WorldToScreen(Vector3 position) {
	Vector3 temp = position - GetViewTranslation();

	float x = temp.Dot(GetViewRight());
	float y = temp.Dot(GetViewUp());
	float z = temp.Dot(GetViewForward() * -1);

	return Vector3((displayWidth / 2) * (1 + x / GetFOVX() / z), (displayHeight / 2) * (1 - y / GetFOVY() / z), z);
}