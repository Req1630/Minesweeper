#ifndef FRAME_H
#define FRAME_H

#include "..\Global.h"

class CFrame
{
public:
	CFrame();
	~CFrame();

	void Init();
	void Update( HWND hWnd );
	void Draw( HDC hDC );

	DWORD GetFrameNum(){ return s_dwFrames; };

private:
	static DWORD s_dwFrames;		//ÌÚ°Ñ”.
	DWORD m_dwBeforeTime;	//‘O‚ÌŠÔ.
	char m_sFPS[32];

	DWORD m_dwNowTime;	//Œ»İ‚ÌŠÔ.
	DWORD m_dwProgress;	//Œo‰ßŠÔ.
	DWORD m_dwIdealTime;//—‘zŠÔ.

};

#endif	// #ifndef FRAME_H.