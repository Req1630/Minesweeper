#include "Frame.h"

DWORD CFrame::s_dwFrames = 0;

CFrame::CFrame()
	: m_dwBeforeTime	( 0 )
	, m_sFPS			( "" )
	, m_dwNowTime		( 0 )
	, m_dwProgress		( 0 )
	, m_dwIdealTime		( 0 )
{
	m_dwBeforeTime = timeGetTime();//現在の時間を取得.
}

CFrame::~CFrame()
{}

void CFrame::Init()
{
	m_dwNowTime = 0;	//現在の時間.
	m_dwProgress = 0;	//経過時間.
	m_dwIdealTime = 0;//理想時間.

	m_dwNowTime = timeGetTime();//現在の時間を取得.

	m_dwProgress = m_dwNowTime - m_dwBeforeTime;//経過時間.

	//理想時間の算出.
	//(現在のﾌﾚｰﾑ数×(1000÷FPS値)).
	m_dwIdealTime = 
		(DWORD)(s_dwFrames*(1000.0F/FPS));
}

void CFrame::Update( HWND hWnd )
{
	//描画の時間待ち.
	if( m_dwIdealTime > m_dwProgress ){
		Sleep( m_dwIdealTime - m_dwProgress );//時間待ち.
	}

	//再描画.
	InvalidateRect( hWnd, nullptr, FALSE );

	//１秒経過.
	if( m_dwProgress >= 1000 ){
		wsprintf( m_sFPS, "FPS[%03d]", s_dwFrames );
		m_dwBeforeTime = m_dwNowTime;//現在の時間に更新.
		s_dwFrames = 0;
	}
	s_dwFrames++;	//ﾌﾚｰﾑ数UP.
}

void CFrame::Draw( HDC hDC )
{
	//FPSの表示.
	TextOut( hDC,
		WND_W-100, 0, m_sFPS, lstrlen(m_sFPS) );
}