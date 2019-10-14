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

// ��̃}�X�̏��.
struct CELL
{
	bool isOpen;	// �J���Ă��邩.
	bool isMine;	// �n�������邩.
	bool isFlag;	// �������邩.
	Position2D Pos;	// ���W.
	int BombCount;	// ����̃{���̐�.
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
	const int FIELD_WIDTH;		// ��.
	const int FIELD_HEIGHT;		// ����.
	const int MINE_MAX_SIZE;	// �n���̐�,
	
	const int DEFAULT_FIELD_WIDTH	= 9;	// ��.
	const int DEFAULT_FIELD_HEIGHT	= 9;	// ����.
	const int DEFAULT_MINE_MAX_SIZE = 10;	// �n���̐�,

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