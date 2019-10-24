#pragma once
#include <Windows.h>
#include <Dwmapi.h> 
#include <TlHelp32.h>
#include "Vectors.h"
#include <string>
#include <thread>
#include <d3d9.h>
#include <d3dx9.h>

struct D3DTLVERTEX {
	float x, y, z, rhw;
	unsigned long color;
};

class Overlay {

public:
	//Constructor
	Overlay();

	int ScreenWidth = GetSystemMetrics(SM_CXSCREEN);

	int ScreenHeight = GetSystemMetrics(SM_CYSCREEN);

	LPDIRECT3D9 d3;
	LPDIRECT3DDEVICE9 d3dev;

	LPD3DXFONT d3Font;

	HWND hWindow;
	HWND tWindow;

	MARGINS margins = { 0, 0, ScreenWidth, ScreenHeight };

	ID3DXLine* d3Line;

	//Initializes our direct3Dx9 instance
	void InitD3D(HWND hWindow);

	//Renders our drawings to the screen
	void Render();

	//Sets up the overlay window
	void SetupWindow();

	//Main loop that calls the esp then renders it
	WPARAM Loop();

	//Esp stuff
	void MainLoop();

	void draw_outlined_rect(float x, float y, float w, float h, unsigned long color);

	void draw_line(float x0, float y0, float x1, float y1, unsigned long color);

	void draw_rect(float x, float y, float w, float h, unsigned long color);

	void draw_filled_rect(float x, float y, float w, float h, unsigned long color);

	void DrawHealthbars(float PosX, float PosY, float height, float Value1, unsigned long color);

	void DrawCircle(int X, int Y, int radius, int numSides, DWORD color);

	void FillArea(float x, float y, float width, float height, D3DCOLOR color);

	void FillRectangle(double x, double y, double w, double h, D3DCOLOR color);

	void DrawBox(float x, float y, float width, float height, D3DCOLOR color);

	void draw_text(std::string_view text, float x, float y, unsigned long color, bool center, bool outline, LPD3DXFONT font);

	RECT get_text_dimensions(std::string_view text, LPD3DXFONT font);
};