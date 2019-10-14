#include "main.h"
#include "..\Global.h"
#include "..\Frame\Frame.h"
#include "..\Image\Image.h"
#include "..\Game\Cells\Cells.h"

HINSTANCE hInst;//ｲﾝｽﾀﾝｽ番号.

void createWindow( int w, int h, HWND hWnd, HINSTANCE hInstance );
void setWindow( int w, int h, HWND hWnd );

//ｽﾚｯﾄﾞ関数.
DWORD WINAPI ThreadFunc( LPVOID vdParam )
{
	//変数宣言.
	GameWindow *gameWnd;//ｹﾞｰﾑｳｨﾝﾄﾞｳ構造体.
						//ｹﾞｰﾑｳｨﾝﾄﾞｳ構造体を取得.
	gameWnd = (GameWindow*)vdParam;//先頭ｱﾄﾞﾚｽをｷｬｽﾄして移す.

	HDC hMemDC;	//ﾒﾓﾘﾃﾞﾊﾞｲｽｺﾝﾃｷｽﾄ.

	// フレームクラス.
	std::unique_ptr<CFrame> pFrame = std::make_unique<CFrame>();
	std::unique_ptr<CCells> pCells = std::make_unique<CCells>( 9, 9, 10 );
	POINT MousePos;	//ﾏｳｽ座標用.

	//------------------------------------------------
	//	起動処理[WM_CREATE].
	//------------------------------------------------
	//ﾒﾓﾘDCの作成.
	hMemDC = CreateCompatibleDC( nullptr );
	CImage::Create( gameWnd->hScreenDC );

	auto ReCreate = [&]( int x, int y, int mines )
	{
		pCells = std::make_unique<CCells>( x, y, mines );

		gameWnd->size.cx = 32*x;
		gameWnd->size.cy = 32*y;

		setWindow( gameWnd->size.cx, gameWnd->size.cy, gameWnd->hWnd );

		HDC hdc = GetDC( gameWnd->hWnd );//DC(ﾃﾞﾊﾞｲｽｺﾝﾃｷｽﾄ)を取得.
		HBITMAP hBmp = CreateCompatibleBitmap(
			hdc,
			gameWnd->size.cx,
			gameWnd->size.cy );

		//DCと互換性のあるDCを作成.
		gameWnd->hScreenDC = CreateCompatibleDC(hdc);

		//不要になったDCを解放.
		ReleaseDC( gameWnd->hWnd, hdc );

		SelectObject( gameWnd->hScreenDC, hBmp );
		DeleteObject( hBmp );
	};
	

	//ｳｨﾝﾄﾞｳが閉じられるまで無限ﾙｰﾌﾟ.
	while( !gameWnd->isCloseWnd )
	{
		pFrame->Init();
		GetCursorPos( &MousePos );

		ScreenToClient(
			gameWnd->hWnd,//ｳｨﾝﾄﾞｳﾊﾝﾄﾞﾙ.
			&MousePos );//(out)POINT構造体.

		if( GetAsyncKeyState('C') & 0x0001 ){
			ReCreate( 30, 16, 99 );
		}
		if( GetAsyncKeyState('X') & 0x0001 ){
			ReCreate( 16, 16, 40 );
		}
		if( GetAsyncKeyState('Z') & 0x0001 ){
			ReCreate( 9, 9, 10 );
		}

		//------------------------------------------------
		//	ｹﾞｰﾑ処理[WM_TIMER].
		//------------------------------------------------
		pCells->Update( MousePos );

		//------------------------------------------------
		//	描画処理[WM_PAINT].
		//------------------------------------------------
		pCells->Draw( gameWnd->hScreenDC, hMemDC );

		pFrame->Update( gameWnd->hWnd );
	}

	//------------------------------------------------
	//	解放処理[WM_DESTROY].
	//------------------------------------------------
	pFrame.~unique_ptr();
	pCells.~unique_ptr();

	//ﾒﾓﾘDCの解放.
	DeleteDC( hMemDC );

	// スレッド終了通知.
	ExitThread(0);

	return TRUE;
}

//ｳｨﾝﾄﾞｳ関数.
LRESULT CALLBACK WindowProc(
	HWND hWnd,		//ｳｨﾝﾄﾞｳﾊﾝﾄﾞﾙ.
	UINT uMsg,		//ﾒｯｾｰｼﾞ.
	WPARAM wParam,	//ﾒｯｾｰｼﾞに対する情報.
	LPARAM lParam )	//ﾒｯｾｰｼﾞに対する情報.
{
	HDC hdc;	//ﾃﾞﾊﾞｲｽｺﾝﾃｷｽﾄ.

	PAINTSTRUCT ps;//描画構造体.

	//-------------------------------------
	//	ｽﾚｯﾄﾞ関連.
	//-------------------------------------
	static GameWindow gameWnd;//ｹﾞｰﾑｳｨﾝﾄﾞｳ構造体.
	static HANDLE th;
	DWORD dwID;		//ｽﾚｯﾄﾞID.
	HBITMAP hBmp;	//ﾋﾞｯﾄﾏｯﾌﾟﾊﾝﾄﾞﾙ.

					//ﾒｯｾｰｼﾞ別の処理.
	switch( uMsg ){
	case WM_DESTROY://ｳｨﾝﾄﾞｳが破棄された時.
					//ｱﾌﾟﾘｹｰｼｮﾝの終了をWindowsに通知する.
		PostQuitMessage(0);
		return 0;

	case WM_CREATE:	//ｳｨﾝﾄﾞｳが生成された時.

		//------------------------------------
		//	ｽﾚｯﾄﾞの設定.
		//------------------------------------
		//ｽﾚｯﾄﾞに渡す値を設定.
		gameWnd.hWnd = hWnd;	//ｳｨﾝﾄﾞｳﾊﾝﾄﾞﾙ.
		gameWnd.size.cx = WND_W;//ｳｨﾝﾄﾞｳ幅.
		gameWnd.size.cy = WND_H;//ｳｨﾝﾄﾞｳ高さ.
		gameWnd.dwFPS = FPS;	//FPS値(基準).

		//HDCの作成.
		hdc = GetDC( hWnd );//DC(ﾃﾞﾊﾞｲｽｺﾝﾃｷｽﾄ)を取得.

		//ﾋﾞｯﾄﾏｯﾌﾟﾊﾝﾄﾞﾙを作成.
		hBmp = CreateCompatibleBitmap(
			hdc,
			gameWnd.size.cx,
			gameWnd.size.cy );

		//DCと互換性のあるDCを作成.
		gameWnd.hScreenDC = CreateCompatibleDC(hdc);

		//hScreenDCをﾋﾞｯﾄﾏｯﾌﾟ用として定義.
		SelectObject( gameWnd.hScreenDC, hBmp );

		//不要になったDCを解放.
		ReleaseDC( hWnd, hdc );

		//ｽﾚｯﾄﾞの作成と実行.
		th = CreateThread(
			nullptr,//ﾊﾝﾄﾞﾙを他のﾌﾟﾛｾｽと共有する場合は指定.
			0,					//ｽﾀｯｸｻｲｽﾞ(ﾃﾞﾌｫﾙﾄ:0).
			ThreadFunc,			//ｽﾚｯﾄﾞ関数名.
			(LPVOID)&gameWnd,	//ｽﾚｯﾄﾞに渡す構造体.
			0,					//0:作成と同時に実行.
			&dwID);				//(out)ｽﾚｯﾄﾞID.

		return 0;


	case WM_PAINT:	//ｳｨﾝﾄﾞｳが更新された時.
					//描画開始.
		hdc = BeginPaint( hWnd, &ps );

		BitBlt(
			hdc,	//表示DC.
			0, 0,	//表示先x,y座標.
			gameWnd.size.cx,//表示する幅.
			gameWnd.size.cy,//表示する高さ.
			gameWnd.hScreenDC,//ﾊﾞｯｸﾊﾞｯﾌｧ(ｽﾚｯﾄﾞ内で作成).
			0, 0,	//元画像のx,y座標.
			SRCCOPY);//ｺﾋﾟｰする.

					 //描画終了.
		EndPaint( hWnd, &ps );
		return 0;
	case WM_KEYDOWN://ｷｰが押された.
					//ｷｰ別の処理.
		switch (wParam) {
		case VK_ESCAPE:	//Esc.
			if (MessageBox(nullptr,
				"ｹﾞｰﾑを終了しますか？", "警告",
				MB_YESNO) == IDYES)
			{
				gameWnd.isCloseWnd = TRUE;
			}
			break;
		}
		if (!gameWnd.isCloseWnd) break;
	case WM_CLOSE:
		DWORD result;
		gameWnd.isCloseWnd = TRUE;
		while (1) {
			//スレッドが終わったかチェック
			GetExitCodeThread(th, &result);
			//終わったらハンドルを閉じる。
			if (STILL_ACTIVE != result) {
				//closehandleで閉じる。
				CloseHandle(th);
				//ループを抜ける。
				break;
			}
		}
		//ｳｨﾝﾄﾞｳを破棄する.
		DestroyWindow(hWnd);
		return 0;
	}

	//ﾒｲﾝに返す情報.
	return DefWindowProc( hWnd, uMsg, wParam, lParam );
}

//メイン関数.
int WINAPI WinMain(
	HINSTANCE hInstance,	//インスタンス番号(ｳｨﾝﾄﾞｳの番号).
	HINSTANCE hPrevInstance,
	PSTR lpCmdLine,
	int nCmdshow )
{
#ifdef _DEBUG
	// メモリリーク検出
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif	// #ifdef _DEBUG.
	WNDCLASS wc;	//ウィンドウクラス構造体.
	HWND hWnd = nullptr;		//ウィンドウハンドル.
	MSG msg;		//メッセージ.
	// アイコンの作成
//	HICON res_icon = LoadIcon( hInstance, MAKEINTRESOURCE(IDI_ICON1) );
	hInst = hInstance;
	//------------------------------------------------
	//	複数CPUに対応させる▼.
	//------------------------------------------------
	DWORD	ProcessAffinitMask;	//CPUを指定.
	HANDLE	hProcess;	//ﾌﾟﾛｾｽﾊﾝﾄﾞﾙ.
	DWORD	errnum;		//ｴﾗｰNo.
	BOOL	ret;		//関数復帰値.



	//自身のﾌﾟﾛｾｽのﾊﾝﾄﾞﾙを取得.
	hProcess = GetCurrentProcess();

	//自身を動作させるCPUを指定する.
	ProcessAffinitMask = 1;	//CPU0を使う(2進数で指定する).
	ret = SetProcessAffinityMask(
		hProcess,
		ProcessAffinitMask );
	if( ret == FALSE ){
		errnum = GetLastError();	//ｴﾗｰ情報を取得.
		return 0;
	}

	//------------------------------------------------
	//	複数CPUに対応させる▲.
	//------------------------------------------------

	//ウィンドウクラスの登録.
	wc.style = CS_HREDRAW | CS_VREDRAW;	//スタイルの登録.
	wc.lpfnWndProc = WindowProc;		//ウィンドウ関数の登録(DefWindowProc:ﾃﾞﾌｫﾙﾄ).
	wc.hInstance = hInstance;			//インスタンス番号の登録.
	wc.hIcon = nullptr;					//アイコンの登録.
	wc.hCursor = nullptr;					//カーソルの登録.
	wc.hbrBackground = (HBRUSH)COLOR_BACKGROUND + 1;	//背景色.
	wc.lpszMenuName = nullptr;				//メニューの登録.
	wc.cbClsExtra = 0;					//拡張用メンバ(使わないので０で固定).
	wc.cbWndExtra = 0;					//拡張用メンバ(使わないので０で固定).
	wc.lpszClassName = APP_NAME;		//アプリケーション名.

	//ウィンドウクラスをWindowsに登録.
	if ( !RegisterClass ( &wc ) ){
		MessageBox ( nullptr, "ウィドウクラスの登録に失敗",
			"ｴﾗｰ", MB_OK );
		return 0;
	}

	createWindow( WND_W, WND_H, hWnd, hInstance );

	//ウィンドウの表示.
//	ShowWindow ( hWnd, SW_SHOW ); 
	//SW_SHOW:表示、SW_HIDE:非表示.

	//メッセージループ.
	while ( GetMessage ( &msg, nullptr, 0, 0 ) > 0 ){
		//ウィンドウ関数にメッセージを送る,
		DispatchMessage ( &msg );
	}

	return 0;
}

void createWindow( int w, int h, HWND hWnd, HINSTANCE hInstance )
{
	//ウィンドウ領域の調整.
	RECT rect;		//短形構造体.
	RECT deskrect;

	rect.left	= 0;	//左.
	rect.top	= 0;	//上.
	rect.right	= w;//右.
	rect.bottom = h;//下.

	// WS_OVERLAPPEDWINDOW, WS_POPUP.


	if ( !AdjustWindowRect (
		&rect,	//(in)画面サイズが入った短形構造体、(out)計算結果.
		(WS_OVERLAPPEDWINDOW ^ (WS_THICKFRAME|WS_MAXIMIZEBOX)),//ウィンドウスタイル.
		FALSE ))	//メニューを持つかどうか.
	{
		MessageBox (nullptr,
			"ウィンドウ領域の調整に失敗",
			"エラー", MB_OK );
		return;
	}
	SystemParametersInfo(SPI_GETWORKAREA, 0, &deskrect, 0);


	//ｳｨﾝﾄﾞｳの作成.
	hWnd = CreateWindow(
		APP_NAME,	//ｱﾌﾟﾘｹｰｼｮﾝ名(構造体と同じ名前にする).
		WND_TITLE,	//ｳｨﾝﾄﾞｳﾀｲﾄﾙ.
		(WS_OVERLAPPEDWINDOW ^ (WS_THICKFRAME|WS_MAXIMIZEBOX)),	//ｳｨﾝﾄﾞｳ種類.
		(((deskrect.right - deskrect.left) - (rect.right - rect.left)) / 2 + deskrect.left), 
		(((deskrect.bottom - deskrect.top) - (rect.bottom - rect.top)) / 2 + deskrect.top),		//表示位置x,y.
		rect.right - rect.left,	//ウィンドウの幅.
		rect.bottom - rect.top,	//ｳｨﾝﾄﾞｳの高さ.
		nullptr,		//親ｳｨﾝﾄﾞｳのﾊﾝﾄﾞﾙ.
		nullptr,		//ﾒﾆｭｰの設定.
		hInstance,	//ｲﾝｽﾀﾝｽ番号.
		nullptr);//ｳｨﾝﾄﾞｳ作成時に発生するｲﾍﾞﾝﾄに渡すﾃﾞｰﾀ.

	if( hWnd == nullptr ){
		MessageBox( nullptr, "ｳｨﾝﾄﾞｳの作成に失敗",
			"ｴﾗｰ", MB_OK );
		return;
	}

	//ウィンドウの表示.
	ShowWindow ( hWnd, SW_SHOW ); 
}

void setWindow( int w, int h, HWND hWnd )
{
	//ウィンドウ領域の調整.
	RECT rect;		//短形構造体.
	RECT deskrect;

	rect.left	= 0;	//左.
	rect.top	= 0;	//上.
	rect.right	= w;//右.
	rect.bottom = h;//下.

	// WS_OVERLAPPEDWINDOW, WS_POPUP.


	if ( !AdjustWindowRect (
		&rect,	//(in)画面サイズが入った短形構造体、(out)計算結果.
		(WS_OVERLAPPEDWINDOW ^ (WS_THICKFRAME|WS_MAXIMIZEBOX)),//ウィンドウスタイル.
		FALSE ))	//メニューを持つかどうか.
	{
		MessageBox (nullptr,
			"ウィンドウ領域の調整に失敗",
			"エラー", MB_OK );
		return;
	}
	SystemParametersInfo(SPI_GETWORKAREA, 0, &deskrect, 0);

	SetWindowPos( 
		hWnd, NULL, 
		(((deskrect.right - deskrect.left) - (rect.right - rect.left)) / 2 + deskrect.left), 
		(((deskrect.bottom - deskrect.top) - (rect.bottom - rect.top)) / 2 + deskrect.top),		//表示位置x,y.
		rect.right - rect.left,	//ウィンドウの幅.
		rect.bottom - rect.top,	//ｳｨﾝﾄﾞｳの高さ.
		SWP_NOMOVE | SWP_NOZORDER );
}