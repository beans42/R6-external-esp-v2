#include "Overlay.h"
#include <iostream>
#include "Globals.h"
#include "Vectors.h"
#include <time.h>

using namespace::std;

Overlay::Overlay() {};

unsigned long Blue = D3DCOLOR_RGBA(65, 135, 245, 255);

WPARAM Overlay::Loop() {
	MSG message;
	RECT rect;

	while (TRUE) {
		ZeroMemory(&message, sizeof(MSG));

		if (PeekMessage(&message, NULL, 0, 0, PM_REMOVE)) {
			TranslateMessage(&message);
			DispatchMessage(&message);
		}

		if (message.message == WM_QUIT) {
			exit(0);
		}

		tWindow = NULL;
		tWindow = FindWindow(NULL, L"Rainbow Six");
		if (!tWindow) {
			std::cout << "Application exiting, failed to find the specified window!" << std::endl;
			ExitProcess(0);
		}

		ZeroMemory(&rect, sizeof(RECT));

		GetWindowRect(tWindow, &rect);
		ScreenWidth = rect.right - rect.left;
		ScreenHeight = rect.bottom - rect.top;

		MoveWindow(hWindow, rect.left, rect.top, ScreenWidth, ScreenHeight, true);

		HWND hWindow2 = GetForegroundWindow();
		HWND hWindow3 = GetWindow(hWindow2, GW_HWNDPREV);

		SetWindowPos(hWindow, hWindow3, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);

		UpdateWindow(hWindow);

		Render();

		Sleep(5);
	}
	return message.wParam;
}

void Overlay::Render() {
	d3dev->Clear(0, NULL, D3DCLEAR_TARGET, D3DCOLOR_ARGB(0, 0, 0, 0), 1.0f, 0);
	d3dev->BeginScene();

	MainLoop();

	d3dev->EndScene();
	d3dev->Present(NULL, NULL, NULL, NULL);
}

void Overlay::MainLoop() {
	//Update addresses

	//Loop through first 12 entities
	//Feel free to add an actual entity count
	for (int i = 1/*0 is local player in situations and lw htunt, set to 0 if playing mp*/; i < 12; i++) {

		//Get the current entity
		DWORD_PTR Entity = Mem.GetEntity(i);

		//Get a PlayerInfo struct with all of this entity's info
		PlayerInfo Player = Mem.GetAllEntityInfo(Entity);
		Player.Health = Mem.GetEntityHealth(Mem.GetEntity2(i));
		GetResolution(screenX, screenY);
		if (Player.Health > 0				//Health over 0
			&& Player.Health <= 100		    //Health less than 200
			&& Player.w2sPos.z >= 0.1f      //Player is actually on the screen
			&& Player.w2sHead.z >= 0.1f     //Player is actually on the screen
)		{
			float BoxHeight = Player.w2sPos.y - Player.ScrenTop.y;
			float BoxWidth = BoxHeight / 2.4;

			if (bSkl == 83 || bSkl == 115)
			{
				//Draw bones
				/*Head-Neck*/		  Over.draw_line(Player.w2sHead.x, Player.w2sHead.y, Player.w2sNeck.x, Player.w2sNeck.y, Blue);
				/*Neck-Chest*/	      Over.draw_line(Player.w2sNeck.x, Player.w2sNeck.y, Player.w2sChest.x, Player.w2sChest.y, Blue);
				/*Chest-Stomach*/     Over.draw_line(Player.w2sChest.x, Player.w2sChest.y, Player.w2sStomach.x, Player.w2sStomach.y, Blue);
				/*Stomach-Pelvis*/    Over.draw_line(Player.w2sStomach.x, Player.w2sStomach.y, Player.w2sPelvis.x, Player.w2sPelvis.y, Blue);
				/*Pevlis-Feet*/		  Over.draw_line(Player.w2sPelvis.x, Player.w2sPelvis.y, Player.w2sPos.x, Player.w2sPos.y, Blue);
				/*Chest-RIGHT_HAND*/  Over.draw_line(Player.w2sChest.x, Player.w2sChest.y, Player.w2sRHand.x, Player.w2sRHand.y, Blue);
			}
			else if (bSkl != 83 && bSkl != 115)
			{
				//Draw player box
				Over.draw_outlined_rect(Player.ScrenTop.x - BoxWidth / 2, Player.ScrenTop.y, BoxWidth, BoxHeight, Blue);
			}
			
			//Draw healthbar
			Over.DrawHealthbars((Player.ScrenTop.x - BoxWidth / 2) - 8, Player.ScrenTop.y, BoxHeight, Player.Health, Blue);
	
			//Draw a circle around the players head
			Over.DrawCircle(Player.w2sHead.x, Player.w2sHead.y, BoxHeight / 12.5, 30, Blue);

			//Draw name
			//Over.draw_text("Terrorist", Player.ScrenTop.x, Player.ScrenTop.y - 14, 0xFFFFFFFF, true, true, d3Font);

			//Draw snapline
			//Over.draw_line(screenX / 2, screenY, Player.w2sHead.x, Player.ScrenTop.y + BoxHeight, Blue);
		}
	}
}



void Overlay::InitD3D(HWND RIGHT_HANDleWindow) {
	//Create our direct3d object
	d3 = Direct3DCreate9(D3D_SDK_VERSION);

	D3DPRESENT_PARAMETERS direct3DPresetParams;

	ZeroMemory(&direct3DPresetParams, sizeof(direct3DPresetParams));

	direct3DPresetParams.Windowed = true;
	direct3DPresetParams.SwapEffect = D3DSWAPEFFECT_DISCARD;
	direct3DPresetParams.hDeviceWindow = RIGHT_HANDleWindow;
	direct3DPresetParams.BackBufferFormat = D3DFMT_A8R8G8B8;
	direct3DPresetParams.BackBufferWidth = ScreenWidth;
	direct3DPresetParams.BackBufferHeight = ScreenHeight;

	direct3DPresetParams.EnableAutoDepthStencil = TRUE;
	direct3DPresetParams.AutoDepthStencilFormat = D3DFMT_D16;

	d3->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, RIGHT_HANDleWindow, D3DCREATE_SOFTWARE_VERTEXPROCESSING, &direct3DPresetParams, &d3dev);
	D3DXCreateFontA(d3dev, 12, NULL, FW_BOLD, 1, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, ANTIALIASED_QUALITY, DEFAULT_PITCH | FF_DONTCARE, "Tahoma", &d3Font);
	D3DXCreateLine(d3dev, &d3Line);
}

void Overlay::draw_outlined_rect(float x, float y, float w, float h, unsigned long color) 
{
	draw_rect(x - 1, y - 1, w + 2, h + 2, D3DCOLOR_RGBA(1, 1, 1, 255));
	draw_rect(x + 1, y + 1, w - 2, h - 2, D3DCOLOR_RGBA(1, 1, 1, 255));
	draw_rect(x, y, w, h, color);
}

void Overlay::draw_line(float x0, float y0, float x1, float y1, unsigned long color) 
{
	/*D3DTLVERTEX qV[ 2 ] = {
		{ float(x0), float(y0), 0.f, 1.f, color },
		{ float(x1), float(y1), 0.f, 1.f, color },
	};

	m_device->SetRenderState(D3DRS_ALPHABLENDENABLE, true);
	m_device->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
	m_device->SetFVF(D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_TEX1);
	m_device->SetTexture(0, nullptr);
	m_device->DrawPrimitiveUP(D3DPT_LINELIST, 2, qV, sizeof(D3DTLVERTEX));*/

	D3DXVECTOR2 lines[2] = {
		D3DXVECTOR2(x0, y0),
		D3DXVECTOR2(x1, y1)
	};

	d3Line->Begin();
	d3Line->Draw(lines, 2, color);
	d3Line->End();
}

void Overlay::draw_rect(float x, float y, float w, float h, unsigned long color) 
{
	draw_line(x, y, x + w, y, color);
	draw_line(x, y, x, y + h, color);
	draw_line(x + w, y, x + w, y + h, color);
	draw_line(x, y + h, x + w + 1, y + h, color);
}

void Overlay::draw_filled_rect(float x, float y, float w, float h, unsigned long color) 
{
	D3DTLVERTEX qV[4] = {
		{ x, y + h, 0.f, 1.f, color },
		{ x, y, 0.f, 1.f, color },
		{ x + w, y + h, 0.f, 1.f, color },
		{ x + w, y , 0.f, 1.f, color }
	};

	d3dev->SetRenderState(D3DRS_ALPHABLENDENABLE, true);
	d3dev->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
	d3dev->SetFVF(D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_TEX1);
	d3dev->SetTexture(0, nullptr);
	d3dev->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, qV, sizeof(D3DTLVERTEX));
}

void Overlay::DrawHealthbars(float PosX, float PosY, float height, float Value1, unsigned long color)
{
	int HealthR = 0, HealthG = 0, HealthB = 0; // Lets make some integers so we can use them for the healthbar. What we will be using this for is to change the color of the healthbar according to the damage done to the enemy.

	float Value2 = static_cast<float>(Value1) / 100.f * height;
	int Value = static_cast<int>(Value2);

	if (Value1 > 0 && Value1 < 101) // if Value is greater then 75 and not greater then 90 draw the health has a darker shade of green.
	{
		draw_rect(PosX - 1, PosY - 1, 5, height, D3DCOLOR_RGBA(0, 0, 0, 255));
		draw_filled_rect(PosX, PosY, 3, Value - 1, color);
	}	
}

void Overlay::DrawCircle(int X, int Y, int radius, int numSides, DWORD color) {
	D3DXVECTOR2 Line[128];
	float Step = M_PI * 2.0 / numSides;
	int Count = 0;
	for (float a = 0; a < M_PI*2.0; a += Step)
	{
		float X1 = radius * cos(a) + X;
		float Y1 = radius * sin(a) + Y;
		float X2 = radius * cos(a + Step) + X;
		float Y2 = radius * sin(a + Step) + Y;
		Line[Count].x = X1;
		Line[Count].y = Y1;
		Line[Count + 1].x = X2;
		Line[Count + 1].y = Y2;
		Count += 2;
	}
	d3Line->Draw(Line, Count, color);
}

void Overlay::FillArea(float x, float y, float width, float height, D3DCOLOR color) {
	D3DXVECTOR2 vectorLine[2];
	d3Line->SetWidth(width);

	vectorLine[0].x = x + width / 2;
	vectorLine[0].y = y;
	vectorLine[1].x = x + width / 2;
	vectorLine[1].y = y + height;

	d3Line->Draw(vectorLine, 2, color);
}

void Overlay::FillRectangle(double x, double y, double w, double h, D3DCOLOR color)
{
	struct Vertex
	{
		float x, y, z, h;
		D3DCOLOR color;
	};

	Vertex vertices[4] =
	{
		x,  y, 0.0f, 1.0f, color,
		x + w, y, 0.0f, 1.0f, color,

		x + w, y + h, 0.0f, 1.0f, color,
		x,  y + h, 0.0f, 1.0f, color,
	};

	d3dev->SetRenderState(D3DRS_ALPHABLENDENABLE, true);
	d3dev->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
	d3dev->SetFVF(D3DFVF_XYZRHW | D3DFVF_DIFFUSE);
	d3dev->DrawPrimitiveUP(D3DPT_TRIANGLEFAN, 2, vertices, sizeof(Vertex));
}

void Overlay::DrawBox(float x, float y, float width, float height, D3DCOLOR color) {
	D3DXVECTOR2 points[5];
	points[0] = D3DXVECTOR2(x, y);
	points[1] = D3DXVECTOR2(x + width, y);
	points[2] = D3DXVECTOR2(x + width, y + height);
	points[3] = D3DXVECTOR2(x, y + height);
	points[4] = D3DXVECTOR2(x, y);
	d3Line->SetWidth(1);
	d3Line->Draw(points, 5, color);
}

void Overlay::draw_text(std::string_view text, float x, float y, unsigned long color, bool center, bool outline, LPD3DXFONT font) {
	if (center) {
		RECT dimensions = get_text_dimensions(text, font);
		x -= (dimensions.right - dimensions.left) / 2;
	}

	auto _text = [&](std::string_view _text, int _x, int _y, unsigned long _color) {
		RECT r{ _x, _y, _x, _y };
		font->DrawTextA(NULL, _text.data(), -1, &r, DT_NOCLIP, _color);
	};

	if (outline) {
		_text(text, x - 1, y, D3DCOLOR_RGBA(1, 1, 1, 255));
		_text(text, x + 1, y, D3DCOLOR_RGBA(1, 1, 1, 255));
		_text(text, x, y - 1, D3DCOLOR_RGBA(1, 1, 1, 255));
		_text(text, x, y + 1, D3DCOLOR_RGBA(1, 1, 1, 255));
	}

	_text(text, x, y, color);
}

RECT Overlay::get_text_dimensions(std::string_view text, LPD3DXFONT font) {
	RECT r;
	font->DrawTextA(NULL, text.data(), -1, &r, DT_CALCRECT, 0xFFFFFFFF);
	return r;
}

LRESULT CALLBACK WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_PAINT:
		Over.Render();
		break;
	case WM_CREATE:
		DwmExtendFrameIntoClientArea(hWnd, &Over.margins);
		break;

	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	}

	return DefWindowProc(hWnd, message, wParam, lParam);
}

void Overlay::SetupWindow() {

	RECT rect;

	while (!tWindow) {
		tWindow = FindWindow(NULL, L"Rainbow Six");
	}

	if (tWindow != NULL) {
		GetWindowRect(tWindow, &rect);
	}
	else {
		std::cout << "Couldn't create overlay. Error code: " << GetLastError() << std::endl;
		Sleep(3000);
		ExitProcess(0);
	}

	WNDCLASSEX windowClass;

	ZeroMemory(&windowClass, sizeof(WNDCLASSEX));

	windowClass.cbSize = sizeof(WNDCLASSEX);
	windowClass.style = CS_HREDRAW | CS_VREDRAW;
	windowClass.lpfnWndProc = WindowProc;
	windowClass.hInstance = GetModuleHandle(0);
	windowClass.hCursor = LoadCursor(NULL, IDC_ARROW);
	windowClass.hbrBackground = (HBRUSH)RGB(0, 0, 0);

	LPCWSTR Str = L"epic overlay";

	windowClass.lpszClassName = Str;

	RegisterClassEx(&windowClass);

	hWindow = CreateWindowExW(WS_EX_LAYERED | WS_EX_TRANSPARENT, windowClass.lpszClassName, Str, WS_POPUP, rect.left, rect.top, ScreenWidth, ScreenHeight, NULL, NULL, windowClass.hInstance, NULL);

	SetLayeredWindowAttributes(hWindow, RGB(0, 0, 0), 0, ULW_COLORKEY);
	SetLayeredWindowAttributes(hWindow, 0, 255, LWA_ALPHA);

	ShowWindow(hWindow, SW_SHOW);
	InitD3D(hWindow);
}
