#ifndef MAIN_H
#define MAIN_H
#include <Windows.h>

//‰¹ºÄ¶,‚¨‚æ‚Ñ½Ú¯ÄŞ‚Åg—p‚·‚é.
#pragma comment( lib, "winmm.lib" )

//\‘¢‘ÌéŒ¾.
typedef struct _GameWindow
{
	HWND	hWnd;		//³¨İÄŞ³ÊİÄŞÙ.
	HDC		hScreenDC;	//ÊŞ¯¸ÊŞ¯Ì§.
	SIZE	size;		//»²½Ş.
	DWORD	dwFPS;		//FPS(Frame Per Second:ÌÚ°Ñ/•b).
	BOOL	isCloseWnd;
} GameWindow;

#endif	// #ifndef MAIN_H.