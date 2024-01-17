#include <SFML/Graphics.hpp>
#include <iostream>

using namespace std;
using namespace sf;

class Chess {
private:

	int width = 900, height = 900;
	Vector2i windowGap;
	RenderWindow window;

	Event event;

	Font font;


	Text textNo[2][8];


	// ARRAY ORDER RIGHT TO DOWN

	RectangleShape cube[8][8];

	Vector2f cubePos[8][8];
	int cubeSize = 100;


	Texture pieceTexture[12];
	int ptOrder[16] = { 2, 3, 4, 5, 6, 4, 3, 2 };

	Sprite piece[2][16];
	Vector2f piecePos[2][16];

	bool pieceEaten[2][16] = {};


	// 0 pawn, 1 rook, 2 knight, 3 bishop, 4 queen, 5 king
	int pathOrigin[6][8][2] = {

		{ {-1, -1}, {1, -1}, {0, -1} },

		{ {0, -1}, {1, 0}, {0, 1}, {-1, 0} },

		{ {-1, -2}, {1, -2}, {2, -1}, {2, 1}, {1, 2}, {-1, 2}, {-2, 1}, {-2, -1} },

		{ {1, -1}, {1, 1}, {-1, 1}, {-1, -1} },

		{ {0, -1}, {1, 0}, {0, 1}, {-1, 0}, {1, -1}, {1, 1}, {-1, 1}, {-1, -1} },

		{ {0, -1}, {1, -1}, {1, 0}, {1, 1}, {0, 1}, {-1, 1}, {-1, 0}, {-1, -1} }

	};

	int pieceTypeStorage[5][10] = {
		{0, 1, 2, 3, 4, 5, 6, 7, -1, -1},
		{8, 15, -1, -1, -1, -1, -1, -1, -1, -1},
		{9, 14, -1, -1, -1, -1, -1, -1, -1, -1},
		{10, 13, -1, -1, -1, -1, -1, -1, -1, -1},
		{11, -1, -1, -1, -1, -1, -1, -1, -1, -1}
	};

	int routeMax[6] = { 3, 4, 8, 4, 8, 8 };
	int pathMax[6] = { 1, 7, 1, 7, 7, 1 };
	int pieceType;


	int pathPossible[16][30][2] = {};
	int pathPossibleCount[16] = {};
	int pathPossibleCountTotal = 0;

	bool canEat = false;
	int eatPossible[16][8] = {};
	int eatPossibleCount[16] = {};


	bool pawnTurn = false, pawnNoEat = false;

	int pawnDoubleMovedNo = -1;
	Vector2i pawnDoubleMovedPos;
	bool enPassant = false;
	Vector2i enPassantPos;


	Vector2i kingCheckPos;

	bool kingCheck = false, gameEnded = false;


	Vector2f mousePos;

	Vector2i clickedPiecePos;
	int clickedPiece;


	bool pressed = false, initFirst = true, secondActive = false, turn = true;
	bool canChangeColor = false, canResetColor = false;




public:

	void setWindow();

	void fonts();

	void textures();

	void events();

	void displays();

	Chess();

	void objects();

	void mouseMain();



	void resetColor();

	void changeColor();

	int findPieceType(int i);

	void isGameEnd();

	bool isKingCheck(int i);


	void pawnLogic(int i, int routes, Vector2i main);

	void checkPath(int i);

	void firstClick();

	void secondClick();


	void logic();
};
