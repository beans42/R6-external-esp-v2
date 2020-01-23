#pragma once
#include <Windows.h>
#include <string>
#include "Vectors.h"

//Struct to hold player info
struct PlayerInfo {
	float		Health;
	Vector3		Position;
	Vector3		ScrenTop;
	Vector3		w2sPos;
	Vector3		HeadPos;
	Vector3		w2sHead;
	Vector3		w2sNeck;
	Vector3		w2sRHand;
	Vector3		w2sChest;
	Vector3		w2sStomach;
	Vector3		w2sPelvis;
};

class Memory {

private:
	uintptr_t pGameManager;
	uintptr_t pEntityList;

	uintptr_t pRender;
	uintptr_t pGameRender;
	uintptr_t pEngineLink;
	uintptr_t pEngine;
	uintptr_t pCamera;

public:
	template <typename Type, typename Base, typename Offset>
	static inline Type Ptr(Base base, Offset offset)
	{
		static_assert(std::is_pointer<Type>::value || std::is_integral<Type>::value, "Type must be a pointer or address");
		static_assert(std::is_pointer<Base>::value || std::is_integral<Base>::value, "Base must be a pointer or address");
		static_assert(std::is_pointer<Offset>::value || std::is_integral<Offset>::value, "Offset must be a pointer or address");

		return base ? reinterpret_cast<Type>((reinterpret_cast<uint64_t>(base) + static_cast<uint64_t>(offset))) : nullptr;
	}

	template <typename Type>
	static bool IsValidPtr(Type* ptr)
	{
		return (ptr && sizeof(ptr)) ? true : false;
	}

	static bool IsValidPtr(void* ptr)
	{
		return (ptr && sizeof(ptr)) ? true : false;
	}

	int displayWidth = GetSystemMetrics(SM_CXSCREEN);
	int displayHeight = GetSystemMetrics(SM_CYSCREEN);

	Memory();

	//Method to set the base address of the game to our global variable
	//Mostly copy pasted from stack overflow tbh
	//Returns true if we got it
	//False if we didn't
	BOOL SetBaseAddress();

	//Just casts base address to a uintptr_t
	uintptr_t GetBaseAddress();

	//Method we use to ReadProcessMemory
	//Templated so we can read any type with the same function
	//Only param is the address to read
	//Returns whatever is read from that address
	template<typename T> T RPM(SIZE_T address);

	//Method we use to WriteProcessMemory
	//Templated so we can write any type with the same function
	//2 params, the address to write to and a buffer to write from
	template<typename T> void WPM(SIZE_T address, T buffer);

	//Simple method to update all top-level addresses
	//Just in case something changes
	void UpdateAddresses();

	//Method to get i entity from the entity list
	//Just doest EntityList] + i * 0x0008
	uintptr_t GetEntity(int i);
	uintptr_t Memory::GetEntity2(int i);

	//Method to get an entity's health
	//Takes pointer to an entity as param
	//Returns health value as a DWORD (120 max health for pvp cause downed state)
	float GetEntityHealth(uintptr_t entity);

	//Method to get entity's feet position
	//Takes pointer to an entity as param
	//Returns a Vector3 of the entity's 3D coordinates at their feet
	Vector3 GetEntityFeetPosition(uintptr_t entity);

	//Method to get an entity's head position
	//Takes pointer to an entity as param
	//Retunrs Vector3 of the entity's 3D coordiantes at their head
	Vector3 GetEntityHeadPosition(uintptr_t entity);
	
	Vector3 GetEntityNeckPosition(uintptr_t entity);
	Vector3 GetEntityHandPosition(uintptr_t entity);
	Vector3 GetEntityChestPosition(uintptr_t entity);
	Vector3 GetEntityStomachPosition(uintptr_t entity);
	Vector3 GetEntityPelvisPosition(uintptr_t entity);

	//Method that gathers all the possible info about a player
	//Returns a PlayerInfo struct
	//Makes life easier
	//If you think this is a performance issues then fix it, but it really isn't
	PlayerInfo GetAllEntityInfo(uintptr_t entity);

	struct ViewMatrix_t {
		Vector3 ViewRight;
		UINT32 filler0;
		Vector3 ViewUp;
		UINT32 filler1;
		Vector3 ViewForward;
		UINT32 filler2;
		Vector3 ViewTranslation;
	};

	//Method to get your view translation
	//Returns Vector3 of the view translation
	//Used for World2Screen stuff
	ViewMatrix_t GetViewMatrix();

	//Method to get your current FOV
	//Returns float of your FOV on the X axis and float of your FOV on the Y axis
	//Used for Wolrd2Screen stuff
	Vector2 GetFOV();

	//World 2 screen function
	//Translates 3d coordinates to 2d screen positions
	//Use that screen position to draw stuff
	Vector3 WorldToScreen(Vector3 position);
};
