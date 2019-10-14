#ifndef IMAGE_H
#define IMAGE_H


#include "..\SingletonBase\SingletonBase.h"

//�w��F�𓧉߂��ĕ`�悷�鏈���ŕK�v.
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

	// �摜�ǂݍ��݊֐�.
	static bool LoadBmp( HBITMAP *hBmp, const char* filename );

	//���ߕ\��.
	static void trnsBlt( 
		HDC hScreenDC,	 // DC.
		int x,	int y,	 // �\����x, y���W.
		int w,	int h,	 // �\����, ����.
		HDC hMemDC,		 // �ǂݍ��݌�DC.
		int bx, int by );// ���摜x, y���W.

	//�������\��.
	static void alphaBlt ( 
		HDC hScreenDC,	// �\����DC.
		int x,	int y,	// �\����x, y���W.
		int w,	int h,	// �\�����A����.
		HDC hMemDC,		// �ǂݍ��݌�DC.
		int bx,	int by,	// �ǂݍ��݌� x, y���W.
		int iAlpha );	// ���ߒl(�A���t�@�l).

	// �g�k�\��.
	static void strchBlt( 
		HDC hScreenDC,	 // �\����DC.
		int x, int y,	 // �\����x, y.
		int w, int h,	 // �\����, ����.
		HDC hMemDC,		 // �ǂݍ��݌�DC.
		int bx, int by,	 // ���摜x, y���W.
		int bw, int bh );// ���摜��, ����.

	// ��]�\��.
	static void plgBlt( 
		HDC hScreenDC,		// �\����DC.
		int cx, int cy,		// �\���� ���Sx, y���W.
		HDC hMemDC,			// �ǂݍ��݌�DC.
		int bx, int by,		// ���摜x, y���W.
		int sx, int sy,		// ���摜��, ����.
		float fAngle );		// �p�x.

	// �r�b�g�\��.
	static void bitBlt ( 
		HDC hScreenDC,		// �\����DC.
		int x, int y,		// �\����x, y���W.
		int w, int h,		// �\�����A����.
		HDC hMemDC,			// �ǂݍ��݌�DC.
		int bx, int by );	// ���摜��, ����.

private:
	void CreatMask( HDC hMemDC, int bx, int by, int sx, int sy );

	HDC		m_hWorkDC;
	HDC		m_hmaskBitmapDC;
	HBITMAP m_hWorkBMP;
	HBITMAP m_hmaskBMP;

	COLORREF oldBkColor;
};

#endif	// #ifndef IMAGE_H.