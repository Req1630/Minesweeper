#ifndef CELLS_H
#define CELLS_H

#include <Windows.h>
#include <vector>

struct Position2D
{
	int x;
	int y;
	Position2D( int _x, int _y )
		: x (_x), y (_y)
	{}
	Position2D() : x (0), y (0)
	{}
};

// 一つのマスの情報.
struct CELL
{
	bool isOpen;	// 開いているか.
	bool isMine;	// 地雷があるか.
	bool isFlag;	// 旗があるか.
	Position2D Pos;	// 座標.
	int BombCount;	// 周りのボムの数.
	CELL()
		: isOpen	( false )
		, isMine	( false )
		, isFlag	( false )
		, Pos		()
		, BombCount	( 0 )
	{}
};

typedef std::vector<std::vector<CELL>> Cells;

class CCells
{
	const int FIELD_WIDTH;		// 幅.
	const int FIELD_HEIGHT;		// 高さ.
	const int MINE_MAX_SIZE;	// 地雷の数,
	
	const int DEFAULT_FIELD_WIDTH	= 9;	// 幅.
	const int DEFAULT_FIELD_HEIGHT	= 9;	// 高さ.
	const int DEFAULT_MINE_MAX_SIZE = 10;	// 地雷の数,

	const int IMAGE_SIZE = 32;
public:
	CCells( int x, int y, int count );
	CCells();
	~CCells();

	void Update( POINT mousePos );
	void Draw( HDC hScreenDC, HDC hMemDC );

	void PlaceLandmine();

	void MinesOpen( int x, int y );

private:
	void MineInit();
	void MineCountInit();
	int getMinesCount( int _x, int _y );

	void AllOpen();
	void ClearOpen();

	void MineDraw( HDC hScreenDC, HDC hMemDC, CELL cell );
	void MineCountDraw( HDC hScreenDC, HDC hMemDC, CELL cell );
	void NotOpenedDraw( HDC hScreenDC, HDC hMemDC, CELL cell );
	void FlagDraw( HDC hScreenDC, HDC hMemDC, CELL cell );
	void ClearDraw( HDC hScreenDC, HDC hMemDC, CELL cell );

	Cells	m_Cells;
	HBITMAP m_hMineBMP;

	bool	m_isClear;
};

#endif	// #ifndef CELLS_H.