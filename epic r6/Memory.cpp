#include <Windows.h>
#include <Psapi.h>
#include "Memory.h"
#include "Globals.h"
#include "Offsets.h"
#include <cmath>
#include <iostream>

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

uintptr_t Memory::GetBaseAddress() {
	return (uintptr_t)BaseAddress;
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
	pGameManager = RPM<uintptr_t>(GetBaseAddress() + ADDRESS_GAMEMANAGER);
	pEntityList = RPM<uintptr_t>(pGameManager + OFFSET_GAMEMANAGER_ENTITYLIST);
	pCamera = RPM<uintptr_t>(GetBaseAddress() + ADDRESS_GAMEPROFILE);

	pCamera = RPM<uintptr_t>(pCamera + GAMEPROFILE_CHAIN1);
	pCamera = RPM<uintptr_t>(pCamera + GAMEPROFILE_CHAIN2);
	pCamera = RPM<uintptr_t>(pCamera + GAMEPROFILE_CHAIN3);
}

uintptr_t Memory::GetEntity(int i) {
	uintptr_t entityBase = RPM<uintptr_t>(pEntityList + (i * OFFSET_GAMEMANAGER_ENTITY));
	return RPM<uintptr_t>(entityBase + 0x20);
}

uintptr_t Memory::GetEntity2(int i) {
	uintptr_t entityBase = RPM<uintptr_t>(pEntityList + (i * OFFSET_GAMEMANAGER_ENTITY));
	return entityBase;
}

float Memory::GetEntityHealth(uintptr_t entity) 
{
	//Entity info pointer from the Entity
	uintptr_t EntityInfo = RPM<uintptr_t>(entity + OFFSET_ENTITY_ENTITYINFO);
	//Main component pointer from the entity info
	uintptr_t MainComponent = RPM<uintptr_t>(EntityInfo + OFFSET_ENTITYINFO_MAINCOMPONENT);
	//Child component pointer form the main component
	uintptr_t ChildComponent = RPM<uintptr_t>(MainComponent + OFFSET_MAINCOMPONENT_CHILDCOMPONENT);

	//Finally health from the child component
	float kek = RPM<float>(ChildComponent + OFFSET_CHILDCOMPONENT_HEALTH_FLOAT);
	kek *= 100;
	kek = round(kek);
	return kek;
}

Vector3 Memory::GetEntityFeetPosition(uintptr_t entity) {
	//We get the feet position straight from the entity
	return RPM<Vector3>(entity + OFFSET_ENTITY_FEET);
}

Vector3 Memory::GetEntityHeadPosition(uintptr_t entity) {
	//We get the head position straight from the entity
	return RPM<Vector3>(entity + OFFSET_ENTITY_HEAD);
}

Vector3 Memory::GetEntityNeckPosition(uintptr_t entity) {
	//We get the neck position straight from the entity
	return RPM<Vector3>(entity + OFFSET_ENTITY_NECK);
}

Vector3 Memory::GetEntityHandPosition(uintptr_t entity) {
	//We get the hand position straight from the entity
	return RPM<Vector3>(entity + OFFSET_ENTITY_RIGHT_HAND);
}

Vector3 Memory::GetEntityChestPosition(uintptr_t entity) {
	//We get the chest position straight from the entity
	return RPM<Vector3>(entity + OFFSET_ENTITY_CHEST);
}

Vector3 Memory::GetEntityStomachPosition(uintptr_t entity) {
	//We get the stomach position straight from the entity
	return RPM<Vector3>(entity + OFFSET_ENTITY_STOMACH);
}

Vector3 Memory::GetEntityPelvisPosition(uintptr_t entity) {
	//We get the pelvis position straight from the entity
	return RPM<Vector3>(entity + OFFSET_ENTITY_PELVIS);
}

PlayerInfo Memory::GetAllEntityInfo(uintptr_t entity) {
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

Memory::ViewMatrix_t Memory::GetViewMatrix() {
	//View translation comes straight from the camera
	return RPM<ViewMatrix_t>(pCamera + OFFSET_CAMERA_VIEWRIGHT);
}

Vector2 Memory::GetFOV() {
	//FOV comes directly from the camera
	Vector2 fov = RPM<Vector2>(pCamera + OFFSET_CAMERA_VIEWFOVX);
	fov.x = std::abs(fov.x);
	fov.y = std::abs(fov.y);
	return fov;
}

Vector3 Memory::WorldToScreen(Vector3 position) {
	ViewMatrix_t VM = GetViewMatrix();
	Vector3 temp = position - VM.ViewTranslation;

	float x = temp.Dot(VM.ViewRight);
	float y = temp.Dot(VM.ViewUp);
	float z = temp.Dot(VM.ViewForward * -1);

	return Vector3((displayWidth / 2) * (1 + x / GetFOV().x / z), (displayHeight / 2) * (1 - y / GetFOV().y / z), z);
}
