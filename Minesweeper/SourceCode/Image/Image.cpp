#include "Image.h"
#include "..\Global.h"

void CImage::Create( HDC hDctDc )
{
	GetInstance()->m_hWorkDC = CreateCompatibleDC ( hDctDc );//作業用DC作成.
	GetInstance()->m_hWorkBMP = CreateCompatibleBitmap ( hDctDc, WND_W*2, WND_H*2 );//作業用ビットマップ作成.
	SelectObject ( GetInstance()->m_hWorkDC, GetInstance()->m_hWorkBMP );	//DCとビットマップの関連付け.
}

bool CImage::LoadBmp( HBITMAP *hBmp, const char* filename )
{
	//背景の読み込み.
	*hBmp = ( HBITMAP ) LoadImage (
		NULL,				//インスタンス番号(省略).
		filename,			//ファイル名.
		IMAGE_BITMAP,		//ビットマップ形式.
		0, 0,				//読み込む画像のx, y座標.
		LR_LOADFROMFILE );	//ファイルから読み込む.

	if ( *hBmp == NULL ) {
		MessageBox ( NULL,
			"画像ファイルの読み込み失敗",
			filename, MB_OK );
		return false;
	}
	return true;
}

//透過表示.
void CImage::trnsBlt(
	HDC hdc,		
	int x, int y,	
	int w, int h,	
	HDC hMemDC,		
	int bx, int by )
{
	TransparentBlt (
		hdc,					
		x, y,					
		w, h,					
		hMemDC,					
		bx, by,					
		w, h,					
		RGB ( 0x00, 0xFF, 0x00 ));
}

//半透明表示.
void CImage::alphaBlt ( 
	HDC hDctDc,
	int iDx, int iDy,
	int iw, int ih,	
	HDC hSrcDC,		
	int iSx, int iSy,
	int iAlpha )	
{
	//透過構造体.
	BLENDFUNCTION blendfunc = { AC_SRC_OVER, (BYTE)0, (BYTE)iAlpha, (BYTE)0 };

	BitBlt (
		GetInstance()->m_hWorkDC,
		WND_W, WND_H,
		iw, ih,
		hDctDc,
		iDx, iDy,
		SRCCOPY );

	GetInstance()->trnsBlt (
		GetInstance()->m_hWorkDC,
		WND_W, WND_H,
		iw, ih,
		hSrcDC,
		iSx, iSy );

	AlphaBlend ( 
		hDctDc,
		iDx, iDy,
		iw, ih,
		GetInstance()->m_hWorkDC,
		WND_W, WND_H,
		iw, ih,
		blendfunc );
}

void CImage::strchBlt( HDC hdc,
	int x, int y,	
	int w, int h,	
	HDC hMemDC,		
	int bx, int by,	
	int bw, int bh )
{
	StretchBlt(
		GetInstance()->m_hWorkDC,
		WND_W, 0,	
		w, h,		
		hMemDC,		
		bx, by,		
		bw, bh,		
		SRCAND );

	GetInstance()->trnsBlt(
		hdc,
		x, y,
		w, h,
		GetInstance()->m_hWorkDC,
		WND_W, 0 );
}

void CImage::plgBlt( HDC hScreenDC,
	int cx, int cy,
	HDC hMemDC,
	int bx, int by,
	int sx, int sy,
	float fAngle )
{
	LONG	hx = ( sx / 2 );	// 矩形の横半分
	LONG	hy = ( sy / 2 );	// 矩形の縦半分
	LONG	radius = static_cast<LONG>(sqrtl( ( hx * hx ) + ( hy * hy ) ));	// 矩形の回転半径
	POINT	po[3];				// 矩形の回転頂点

	const float M_PI = 3.14f;
	const float M_PI_2 = M_PI/2.0f;
	const float M_PI_4 = M_PI/4.0f;

	// 左上隅
	fAngle -= ( M_PI_4 + M_PI_2 );
	po[0].x = ( cx + static_cast<LONG>(ceill( cosl( fAngle ) * radius )) );
	po[0].y = ( cy + static_cast<LONG>(ceill( sinl( fAngle ) * radius )) );
	// 右上隅
	fAngle += M_PI_2;
	po[1].x = ( cx + static_cast<LONG>(ceill( cosl( fAngle ) * radius )) );
	po[1].y = ( cy + static_cast<LONG>(ceill( sinl( fAngle ) * radius )) );
	// 左下隅
	fAngle += M_PI;
	po[2].x = ( cx + static_cast<LONG>(ceill( cosl( fAngle ) * radius )) );
	po[2].y = ( cy + static_cast<LONG>(ceill( sinl( fAngle ) * radius )) );

	GetInstance()->CreatMask( hMemDC, bx, by, sx, sy );

	PlgBlt(
		hScreenDC,
		po, 
		hMemDC,
		bx, by, 
		sx, sy,
		GetInstance()->m_hmaskBMP,
		0, 0 );

	DeleteObject( GetInstance()->m_hmaskBMP );
	DeleteObject( GetInstance()->m_hmaskBitmapDC );
}

void CImage::bitBlt ( HDC hdc,
	int iDx, int iDy,
	int iw, int ih,	
	HDC hMemDC,		
	int bx, int by )
{
	BitBlt ( hdc,	
		iDx,  iDy,	
		iw, ih,	
		hMemDC,
		bx, by,		
		SRCCOPY );
}

void CImage::CreatMask( HDC hMemDC, int bx, int by, int sx, int sy )
{
	GetInstance()->oldBkColor = SetBkColor(
		hMemDC, RGB( 0x00, 0xFF, 0x00 ) );//背景色設定

	GetInstance()->m_hmaskBitmapDC = CreateCompatibleDC( NULL );

	GetInstance()->m_hmaskBMP = CreateBitmap(
		sx, sy, 1, 1, NULL );

	SelectObject( GetInstance()->m_hmaskBitmapDC, GetInstance()->m_hmaskBMP );

	//マスクビットマップを描画
	BitBlt(
		GetInstance()->m_hmaskBitmapDC,
		0, 0,
		sx, sy,
		hMemDC,
		bx, by,
		NOTSRCCOPY );

	//背景色を元に戻す
	SetBkColor( hMemDC, GetInstance()->oldBkColor );
}