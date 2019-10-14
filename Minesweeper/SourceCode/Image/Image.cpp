#include "Image.h"
#include "..\Global.h"

void CImage::Create( HDC hDctDc )
{
	GetInstance()->m_hWorkDC = CreateCompatibleDC ( hDctDc );//��ƗpDC�쐬.
	GetInstance()->m_hWorkBMP = CreateCompatibleBitmap ( hDctDc, WND_W*2, WND_H*2 );//��Ɨp�r�b�g�}�b�v�쐬.
	SelectObject ( GetInstance()->m_hWorkDC, GetInstance()->m_hWorkBMP );	//DC�ƃr�b�g�}�b�v�̊֘A�t��.
}

bool CImage::LoadBmp( HBITMAP *hBmp, const char* filename )
{
	//�w�i�̓ǂݍ���.
	*hBmp = ( HBITMAP ) LoadImage (
		NULL,				//�C���X�^���X�ԍ�(�ȗ�).
		filename,			//�t�@�C����.
		IMAGE_BITMAP,		//�r�b�g�}�b�v�`��.
		0, 0,				//�ǂݍ��މ摜��x, y���W.
		LR_LOADFROMFILE );	//�t�@�C������ǂݍ���.

	if ( *hBmp == NULL ) {
		MessageBox ( NULL,
			"�摜�t�@�C���̓ǂݍ��ݎ��s",
			filename, MB_OK );
		return false;
	}
	return true;
}

//���ߕ\��.
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

//�������\��.
void CImage::alphaBlt ( 
	HDC hDctDc,
	int iDx, int iDy,
	int iw, int ih,	
	HDC hSrcDC,		
	int iSx, int iSy,
	int iAlpha )	
{
	//���ߍ\����.
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
	LONG	hx = ( sx / 2 );	// ��`�̉�����
	LONG	hy = ( sy / 2 );	// ��`�̏c����
	LONG	radius = static_cast<LONG>(sqrtl( ( hx * hx ) + ( hy * hy ) ));	// ��`�̉�]���a
	POINT	po[3];				// ��`�̉�]���_

	const float M_PI = 3.14f;
	const float M_PI_2 = M_PI/2.0f;
	const float M_PI_4 = M_PI/4.0f;

	// �����
	fAngle -= ( M_PI_4 + M_PI_2 );
	po[0].x = ( cx + static_cast<LONG>(ceill( cosl( fAngle ) * radius )) );
	po[0].y = ( cy + static_cast<LONG>(ceill( sinl( fAngle ) * radius )) );
	// �E���
	fAngle += M_PI_2;
	po[1].x = ( cx + static_cast<LONG>(ceill( cosl( fAngle ) * radius )) );
	po[1].y = ( cy + static_cast<LONG>(ceill( sinl( fAngle ) * radius )) );
	// ������
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
		hMemDC, RGB( 0x00, 0xFF, 0x00 ) );//�w�i�F�ݒ�

	GetInstance()->m_hmaskBitmapDC = CreateCompatibleDC( NULL );

	GetInstance()->m_hmaskBMP = CreateBitmap(
		sx, sy, 1, 1, NULL );

	SelectObject( GetInstance()->m_hmaskBitmapDC, GetInstance()->m_hmaskBMP );

	//�}�X�N�r�b�g�}�b�v��`��
	BitBlt(
		GetInstance()->m_hmaskBitmapDC,
		0, 0,
		sx, sy,
		hMemDC,
		bx, by,
		NOTSRCCOPY );

	//�w�i�F�����ɖ߂�
	SetBkColor( hMemDC, GetInstance()->oldBkColor );
}