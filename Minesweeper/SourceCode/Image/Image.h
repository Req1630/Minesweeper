#ifndef IMAGE_H
#define IMAGE_H


#include "..\SingletonBase\SingletonBase.h"

//指定色を透過して描画する処理で必要.
#pragma comment ( lib, "msimg32.lib" )

class CImage : public CSingletonBase<CImage>
{
	
public:
	CImage()
		: m_hWorkDC			( nullptr )
		, m_hWorkBMP		( nullptr )
		, m_hmaskBitmapDC	( nullptr )
		, m_hmaskBMP		( nullptr )
	{
	};
	~CImage()
	{
		DeleteObject( m_hmaskBMP );
		DeleteObject( m_hmaskBitmapDC );
		DeleteObject( m_hWorkBMP );
		DeleteObject( m_hWorkDC );
	};

	static void Create( HDC hDctDc );

	// 画像読み込み関数.
	static bool LoadBmp( HBITMAP *hBmp, const char* filename );

	//透過表示.
	static void trnsBlt( 
		HDC hScreenDC,	 // DC.
		int x,	int y,	 // 表示先x, y座標.
		int w,	int h,	 // 表示幅, 高さ.
		HDC hMemDC,		 // 読み込み元DC.
		int bx, int by );// 元画像x, y座標.

	//半透明表示.
	static void alphaBlt ( 
		HDC hScreenDC,	// 表示先DC.
		int x,	int y,	// 表示先x, y座標.
		int w,	int h,	// 表示幅、高さ.
		HDC hMemDC,		// 読み込み元DC.
		int bx,	int by,	// 読み込み元 x, y座標.
		int iAlpha );	// 透過値(アルファ値).

	// 拡縮表示.
	static void strchBlt( 
		HDC hScreenDC,	 // 表示先DC.
		int x, int y,	 // 表示先x, y.
		int w, int h,	 // 表示幅, 高さ.
		HDC hMemDC,		 // 読み込み元DC.
		int bx, int by,	 // 元画像x, y座標.
		int bw, int bh );// 元画像幅, 高さ.

	// 回転表示.
	static void plgBlt( 
		HDC hScreenDC,		// 表示先DC.
		int cx, int cy,		// 表示先 中心x, y座標.
		HDC hMemDC,			// 読み込み元DC.
		int bx, int by,		// 元画像x, y座標.
		int sx, int sy,		// 元画像幅, 高さ.
		float fAngle );		// 角度.

	// ビット表示.
	static void bitBlt ( 
		HDC hScreenDC,		// 表示先DC.
		int x, int y,		// 表示先x, y座標.
		int w, int h,		// 表示幅、高さ.
		HDC hMemDC,			// 読み込み元DC.
		int bx, int by );	// 元画像幅, 高さ.

private:
	void CreatMask( HDC hMemDC, int bx, int by, int sx, int sy );

	HDC		m_hWorkDC;
	HDC		m_hmaskBitmapDC;
	HBITMAP m_hWorkBMP;
	HBITMAP m_hmaskBMP;

	COLORREF oldBkColor;
};

#endif	// #ifndef IMAGE_H.