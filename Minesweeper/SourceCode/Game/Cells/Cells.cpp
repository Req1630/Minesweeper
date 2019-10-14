#include <time.h>

#include "Cells.h"
#include "..\..\Image\Image.h"

CCells::CCells( int x, int y, int count )
	: FIELD_WIDTH	( x )
	, FIELD_HEIGHT	( y )
	, MINE_MAX_SIZE	( count )
	, m_Cells		( Cells( FIELD_HEIGHT, std::vector<CELL>(FIELD_WIDTH) ) )
	, m_hMineBMP	( nullptr )
	, m_isClear		( false )
{
	CImage::LoadBmp( &m_hMineBMP, "Data\\BMP\\Minesweeper.bmp" );
	PlaceLandmine();
}

CCells::CCells()
	: CCells( 
		DEFAULT_FIELD_WIDTH, 
		DEFAULT_FIELD_HEIGHT, 
		DEFAULT_MINE_MAX_SIZE )
{
}

CCells::~CCells()
{}

void CCells::Update( POINT mousePos )
{
	
	Position2D pos;

	if( ( 0 <= mousePos.x ) && ( mousePos.x <= static_cast<int>(m_Cells[0].size()*IMAGE_SIZE) ) &&
		( 0 <= mousePos.y ) && ( mousePos.y <= static_cast<int>(m_Cells.size()*IMAGE_SIZE) ) ){

		pos.x = mousePos.x / IMAGE_SIZE;
		pos.y = mousePos.y / IMAGE_SIZE;

		if( GetAsyncKeyState(VK_LBUTTON) & 0x0001 ){
			if( m_Cells[pos.y][pos.x].isMine == true ){
				AllOpen();
			} else {
				MinesOpen( pos.x, pos.y );
			}
		}
		if( GetAsyncKeyState(VK_RBUTTON) & 0x0001 ){
			m_Cells[pos.y][pos.x].isFlag = !m_Cells[pos.y][pos.x].isFlag;
		}
	}
	ClearOpen();
}

void CCells::Draw( HDC hScreenDC, HDC hMemDC )
{
	if( m_hMineBMP == nullptr ) return;
	SelectObject( hMemDC, m_hMineBMP );

	for( auto v : m_Cells ){
		for( auto cell : v ){
			MineDraw( hScreenDC, hMemDC, cell );
			MineCountDraw( hScreenDC, hMemDC, cell );
			NotOpenedDraw( hScreenDC, hMemDC, cell );
			FlagDraw( hScreenDC, hMemDC, cell );
		}
	}
	for( auto v : m_Cells ){
		for( auto cell : v ){
			ClearDraw( hScreenDC, hMemDC, cell );
		}
	}
}

void CCells::MinesOpen( int x, int y )
{
	if( x < 0 || x >= FIELD_WIDTH || y < 0 || y >= FIELD_HEIGHT ) return;
	if( m_Cells[y][x].isOpen == true ) return;
	if( m_Cells[y][x].isMine == true ) return;

	m_Cells[y][x].isOpen = true;

	if( ( m_Cells[y][x].isMine == false ) && ( m_Cells[y][x].BombCount == 0 ) ) {
		for( int _y = -1; _y <= 1; _y++ ){
			for( int _x = -1; _x <= 1; _x++ ){
				if(( _y == 0 ) && ( _x == 0 )) continue;

				int x2 = x + _x;
				int y2 = y + _y;
				MinesOpen( x2, y2 );
			}
		}
	}
}

void CCells::PlaceLandmine()
{
	for( size_t y = 0; y < m_Cells.size(); y++ ){
		for( size_t x = 0; x < m_Cells[y].size(); x++ ){
			m_Cells[y][x].Pos.x = x;
			m_Cells[y][x].Pos.y = y;
		}
	}
	MineInit();
	MineCountInit();
}

void CCells::MineInit()
{
	srand((unsigned int) time (nullptr));
	int mineCount = 0;
	do {
		int x = rand() % FIELD_WIDTH;
		int y = rand() % FIELD_HEIGHT;

		if( m_Cells[y][x].isMine == false ){
			m_Cells[y][x].isMine = true;
			mineCount++;
		}
	} while( mineCount < MINE_MAX_SIZE );
}

void CCells::MineCountInit()
{
	for( auto& v : m_Cells ){
		for( auto& cell : v ){
			cell.BombCount = getMinesCount( cell.Pos.x, cell.Pos.y );
		}
	}
	
}

int CCells::getMinesCount( int _x, int _y )
{
	int count = 0;
	for( int y = -1; y <= 1; y++ ){
		for( int x = -1; x <= 1; x++ ){
			if(( y == 0 ) && ( x == 0 )) continue;

			int x2 = _x + x;
			int y2 = _y + y;

			if( ( x2 < 0 ) || ( x2 >= FIELD_WIDTH ) || 
				( y2 < 0 ) || ( y2 >= FIELD_HEIGHT ) ) continue;

			if( m_Cells[y2][x2].isMine == true ) count++;
		}
	}
	return count;
}

void CCells::AllOpen()
{
	for( auto& v : m_Cells ){
		for( auto& cell : v ){
			cell.isOpen = true;
		}
	}
}

void CCells::ClearOpen()
{
	int n = 0;
	for( auto& v : m_Cells ){
		for( auto& cell : v ){
			if(( cell.isFlag == true ) && ( cell.isMine == true )){
				n++;
			}
		}
	}
	if( n != MINE_MAX_SIZE ) return;
	m_isClear = true;
}

void CCells::MineDraw( HDC hScreenDC, HDC hMemDC, CELL cell )
{
	if( cell.isMine == false ) return; 
	CImage::bitBlt(
		hScreenDC,
		cell.Pos.x*IMAGE_SIZE,
		cell.Pos.y*IMAGE_SIZE,
		IMAGE_SIZE, IMAGE_SIZE,
		hMemDC,
		IMAGE_SIZE * (12%10), 
		IMAGE_SIZE * (12/10) );
}

void CCells::MineCountDraw( HDC hScreenDC, HDC hMemDC, CELL cell )
{
	if( cell.isMine == true ) return;
	CImage::bitBlt(
		hScreenDC,
		cell.Pos.x*IMAGE_SIZE,
		cell.Pos.y*IMAGE_SIZE,
		IMAGE_SIZE, IMAGE_SIZE,
		hMemDC,
		IMAGE_SIZE * (cell.BombCount+1%10), 
		0 );
}
void CCells::NotOpenedDraw( HDC hScreenDC, HDC hMemDC, CELL cell )
{
	if( cell.isOpen == true ) return;
	CImage::bitBlt(
		hScreenDC,
		cell.Pos.x*IMAGE_SIZE,
		cell.Pos.y*IMAGE_SIZE,
		IMAGE_SIZE, IMAGE_SIZE,
		hMemDC,
		0, 0 );
}

void CCells::FlagDraw( HDC hScreenDC, HDC hMemDC, CELL cell )
{
	if( cell.isFlag == false ) return;
	if( cell.isOpen == true ) return;
	CImage::bitBlt(
		hScreenDC,
		cell.Pos.x*IMAGE_SIZE,
		cell.Pos.y*IMAGE_SIZE,
		IMAGE_SIZE, IMAGE_SIZE,
		hMemDC,
		0, 
		IMAGE_SIZE );
}

void CCells::ClearDraw( HDC hScreenDC, HDC hMemDC, CELL cell )
{
	if( m_isClear == false ) return;
	CImage::bitBlt(
		hScreenDC,
		cell.Pos.x*IMAGE_SIZE,
		cell.Pos.y*IMAGE_SIZE,
		IMAGE_SIZE, IMAGE_SIZE,
		hMemDC,
		IMAGE_SIZE * (11%10), 
		IMAGE_SIZE * (11/10) );
}