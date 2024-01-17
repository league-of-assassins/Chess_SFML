
#include "main.hpp"



void Chess::setWindow() {
	window.create(VideoMode(width, height), "Chess Game", Style::None);
	window.setFramerateLimit(60);

	windowGap.x = window.getPosition().x;
	windowGap.y = window.getPosition().y;
}

void Chess::fonts() {
	if (!font.loadFromFile("resources\\fonts\\font-style.ttf")) {
		cout << "\n\t Font not found\n";
		return;
	}
}

void Chess::textures() {

	string temp;
	for (int i = 0; i < 12; i++) {
		temp = "resources\\Pieces\\" + to_string(i) + ".png";
		if (!pieceTexture[i].loadFromFile(temp)) {
			return;
		}
	}

	for (int i = 0; i < 2; i++) {
		for (int j = 0; j < 8; j++) {
			piece[i][j].setTexture(pieceTexture[i]);
		}
	}

	for (int i = 0; i < 2; i++) {
		for (int j = 0; j < 8; j++) {
			piece[i][j + 8].setTexture(pieceTexture[ptOrder[j] + i * 5]);
		}
	}

}

void Chess::events() {
	while (window.pollEvent(event))
	{
		if (event.type == Event::Closed) window.close();


		else if (event.type == Event::KeyPressed) {

			if (event.key.code == Keyboard::Escape) {
				window.close();
			}
		}

		else if (event.type == Event::MouseButtonPressed) {

			if (event.mouseButton.button == Mouse::Left) {
				pressed = true;
			}
		}
	}
}

void Chess::displays() {

	window.clear();



	for (int i = 0; i < 8; i++) {
		for (int j = 0; j < 8; j++) {
			window.draw(cube[i][j]);
		}
	}

	for (int i = 0; i < 2; i++) {
		for (int j = 0; j < 8; j++) {
			window.draw(textNo[i][j]);
		}
	}

	for (int i = 0; i < 2; i++) {
		for (int j = 0; j < 16; j++) {
			if (!pieceEaten[i][j]) window.draw(piece[i][j]);
		}
	}



	window.display();
}

Chess::Chess() {
	fonts();
	textures();
	objects();
	setWindow();


	while (window.isOpen())
	{
		events();

		mouseMain();

		displays();
	}
}



void Chess::objects() {

	//BOARD

	int row = 0, column = 0, n = 1;

	for (int i = 0; i < 8; i++) {
		for (int j = 0; j < 8; j++) {
			cubePos[i][j].x = j * 100;
			cubePos[i][j].y = i * 100;
			cube[i][j].setPosition(cubePos[i][j]);
			cube[i][j].setSize(Vector2f(cubeSize, cubeSize));

			if (n == 1) {
				cube[i][j].setFillColor(Color::White);
			}
			else {
				cube[i][j].setFillColor(Color::Yellow);
			}

			n *= -1;
		}
		n *= -1;
	}


	//BOARD NUMBERS

	for (int i = 0; i < 2; i++) {
		for (int j = 0; j < 8; j++) {
			textNo[i][j].setOrigin(-40, -30);
			textNo[i][j].setFont(font);
			textNo[i][j].setFillColor(Color::White);
			textNo[i][j].setString(to_string(j));
			textNo[i][j].setPosition(Vector2f(800 * i + j * 100 * (1 - i), 800 * (1 - i) + j * 100 * i));
		}
	}


	//PIECES

	column = 1; row = 0;
	for (int j = 0; j < 2; j++) {
		for (int i = 0; i < 16; i++) {
			piece[j][i].setScale(Vector2f(5.0 / 3, 5.0 / 3));
			piecePos[j][i].x = cubePos[column][row].x;
			piecePos[j][i].y = cubePos[column][row].y;
			piece[j][i].setPosition(piecePos[j][i]);

			row++;
			if (i == 7) {
				row = 0;
				if (j == 0) { column = 0; }
				else { column = 7; }
			}
		}
		row = 0;
		column = 6;
	}


	//OTHERS

	for (int i = 0; i < 2; i++) {
		for (int j = 0; j < 16; j++) {

			pieceEaten[i][j] = false;
		}
	}

	pawnDoubleMovedPos.y = -1;
}



void Chess::mouseMain() {
	if (pressed && !gameEnded) {
		pressed = false;

		mousePos.x = Mouse::getPosition().x - windowGap.x;
		mousePos.y = Mouse::getPosition().y - windowGap.y;

		logic();
	}
}


void Chess::resetColor() {
	canResetColor = false;

	int n = 1;
	for (int i = 0; i < 8; i++) {
		for (int j = 0; j < 8; j++) {
			if (n == 1) {
				cube[i][j].setFillColor(Color::White);
			}
			else {
				cube[i][j].setFillColor(Color::Yellow);
			} n *= -1;
		} n *= -1;
	}

	if (kingCheck) {
		cube[kingCheckPos.x][kingCheckPos.y].setFillColor(Color::Red);
	}
}


void Chess::changeColor() {

	canChangeColor = false;

	cube[clickedPiecePos.y / 100][clickedPiecePos.x / 100].setFillColor(Color::Cyan);
	for (int n = 0; n < pathPossibleCount[clickedPiece]; n++) {

		cube[pathPossible[clickedPiece][n][1] / 100][pathPossible[clickedPiece][n][0] / 100].setFillColor(Color::Green);
	}

	int tempX, tempY;
	for (int n = 0; n < eatPossibleCount[clickedPiece]; n++) {

		tempX = piecePos[!turn][eatPossible[clickedPiece][n]].y / 100;
		tempY = piecePos[!turn][eatPossible[clickedPiece][n]].x / 100;

		cube[tempX][tempY].setFillColor(Color::Red);
	}

	canResetColor = true;
}


int Chess::findPieceType(int i) {

	for (int m = 0; m < 5; m++) {
		for (int n = 0; n < 10; n++) {
			if (pieceTypeStorage[m][n] == -1) break;
			if (i == pieceTypeStorage[m][n]) return m;
		}
	}

	return 5;
}


void Chess::isGameEnd() {

	if (pathPossibleCountTotal == 0) {
		string endText = "draw";

		if (kingCheck) {
			endText = "checkmate";
		}

		cout << "\n " << endText << "\n";

		gameEnded = true;
	}
}


bool Chess::isKingCheck(int i) {

	Vector2i path;
	bool pawnTurnTemp;

	int pieceTypeTemp = findPieceType(i);

	for (int routes = 0; routes < routeMax[pieceTypeTemp]; routes++) {

		pawnTurnTemp = false;
		if (pieceTypeTemp == 0) {
			pawnTurnTemp = true;
			if (routes == 2) break;
		}

		for (int paths = 0; paths < pathMax[pieceTypeTemp]; paths++) {

			path.x = piecePos[turn][i].x + pathOrigin[pieceTypeTemp][routes][0] * (paths + 1) * 100;
			path.y = piecePos[turn][i].y + (pathOrigin[pieceTypeTemp][routes][1] + pawnTurnTemp * !turn * 2) * (paths + 1) * 100;

			for (int j = 0; j < 16; j++) {

				if (!pieceEaten[turn][j] && piecePos[turn][j].x == path.x && piecePos[turn][j].y == path.y) {

					paths = pathMax[pieceTypeTemp] - 1;
				}

				if (!pieceEaten[!turn][j] && piecePos[!turn][j].x == path.x && piecePos[!turn][j].y == path.y) {
					
					if (j == 12) return true;

					paths = pathMax[pieceTypeTemp] - 1;
					break;
				}
			}
		}
	}

	return false;
}


void Chess::pawnLogic(int i, int routes, Vector2i main) {

	pawnTurn = false;
	pawnNoEat = false;
	pathMax[0] = 1;

	if (pieceType == 0) {

		pawnTurn = true;

		if (routes == 2) pawnNoEat = true;

		if (!turn * 500 + piecePos[turn][i].y == 600) pathMax[pieceType] = 2;

		if (pawnDoubleMovedPos.y >= 0 && routes <= 1) {
			if (main.y == pawnDoubleMovedPos.y && main.x + (2 * routes + (-1)) * 100 == pawnDoubleMovedPos.x) {
				enPassant = true;
			}
		}
	}
}


void Chess::firstClick() {

	for (int i = 0; i < 16; i++) {

		if (!pieceEaten[turn][i]) {

			if (initFirst) {

				if (i == 0) pathPossibleCountTotal = 0;

				pieceType = findPieceType(i);

				checkPath(i);
			}

			if (piecePos[turn][i].x == clickedPiecePos.x && piecePos[turn][i].y == clickedPiecePos.y) {

				clickedPiece = i;

				canChangeColor = true;

				secondActive = true;
			}
		}
	}

	initFirst = false;
}


void Chess::checkPath(int i) {

	pathPossibleCount[i] = 0;
	eatPossibleCount[i] = 0;

	Vector2i main(piecePos[turn][i]);
	Vector2i path;

	bool blockRoute = false, blockPath = false;


	for (int routes = 0; routes < routeMax[pieceType]; routes++) {

		pawnLogic(i, routes, main);

		for (int paths = 0; paths < pathMax[pieceType]; paths++) {

			blockRoute = false;
			blockPath = false;

			path.x = main.x + pathOrigin[pieceType][routes][0] * (paths + 1) * 100;
			path.y = main.y + (pathOrigin[pieceType][routes][1] + pawnTurn * !turn * 2) * (paths + 1) * 100;


			//BORDER BLOCK
			if (path.x < 0 || path.x > 700 || path.y < 0 || path.y > 700) {
				blockRoute = true;
			}

			else {

				for (int j = 0; j < 16; j++) {

					//ALLY PIECE BLOCK
					if (!pieceEaten[turn][j] && piecePos[turn][j].x == path.x && piecePos[turn][j].y == path.y) {
						blockRoute = true;
						
						break;
					}
				}


				//IF KING CHECK BLOCK
				if (!blockRoute) {

					piecePos[turn][i].x = path.x;
					piecePos[turn][i].y = path.y;

					turn = !turn;
					for (int m = 0; m < 16; m++) {

						int temp = findPieceType(m);

						if (!(temp == 0 || temp == 2 || temp == 5) && !pieceEaten[turn][m]) {

							if (isKingCheck(m)) {

								if (path.x == piecePos[turn][m].x && path.y == piecePos[turn][m].y) {
									break;
								}

								blockRoute = true;
								blockPath = true;

								break;
							}
						}
					}
					turn = !turn;

					piecePos[turn][i].x = main.x;
					piecePos[turn][i].y = main.y;
				}


				if (!blockRoute) {
					for (int j = 0; j < 16; j++) {

						//ENEMY PIECE BLOCK
						if (!pieceEaten[!turn][j] && piecePos[!turn][j].x == path.x && piecePos[!turn][j].y == path.y || enPassant) {
							blockRoute = true;

							if (!pawnNoEat) {

								if (enPassant) {
									enPassant = false;
									j = pawnDoubleMovedNo;
									enPassantPos = path;
								}

								canEat = true;

								eatPossible[i][eatPossibleCount[i]] = j;
								eatPossibleCount[i]++;
							}

							break;
						}
					}
				}
			}

			//IF PAWN SIDES BUT NOTHING TO EAT
			if (pawnTurn && !pawnNoEat && !canEat) blockRoute = true;
			

			//ADD TO PATH
			if (!blockRoute || canEat) {

				canEat = false;

				pathPossible[i][pathPossibleCount[i]][0] = path.x;
				pathPossible[i][pathPossibleCount[i]][1] = path.y;

				pathPossibleCount[i]++;
				pathPossibleCountTotal++;
			}


			if (blockRoute && !blockPath) break;
		}
	}
}


void Chess::secondClick() {

	secondActive = false;

	pieceType = findPieceType(clickedPiece);

	for (int i = 0; i < pathPossibleCount[clickedPiece]; i++) {
		if (pathPossible[clickedPiece][i][0] == clickedPiecePos.x && pathPossible[clickedPiece][i][1] == clickedPiecePos.y) {

			//REMOVE EATEN PIECE

			//ENPASSANT EATEN
			if (pieceType == 0 && clickedPiecePos == enPassantPos) {
				pieceEaten[!turn][pawnDoubleMovedNo] = true;
			}
			
			//NORMAL EATEN
			else {
				for (int j = 0; j < eatPossibleCount[clickedPiece]; j++) {
					if (piecePos[!turn][eatPossible[clickedPiece][j]].x == clickedPiecePos.x && piecePos[!turn][eatPossible[clickedPiece][j]].y == clickedPiecePos.y) {

						pieceEaten[!turn][eatPossible[clickedPiece][j]] = true;

						break;
					}
				}
			}

			//PAWN DOUBLE MOVE CHECK
			pawnDoubleMovedPos.y = -1;
			enPassantPos.y = -1;

			if (pieceType == 0 && abs(piecePos[turn][clickedPiece].y - clickedPiecePos.y) == 200) {

				pawnDoubleMovedPos = clickedPiecePos;
				pawnDoubleMovedNo = clickedPiece;
			}


			//MOVE PIECE
			piecePos[turn][clickedPiece].x = clickedPiecePos.x;
			piecePos[turn][clickedPiece].y = clickedPiecePos.y;

			piece[turn][clickedPiece].setPosition(piecePos[turn][clickedPiece]);


			//KING CHECK
			kingCheck = false;
			
			for (int m = 0; m < 16; m++) {

				if (!pieceEaten[turn][m]) {
					if (isKingCheck(m)) {

						kingCheck = true;

						kingCheckPos.x = piecePos[!turn][12].y;
						kingCheckPos.x /= 100;

						kingCheckPos.y = piecePos[!turn][12].x;
						kingCheckPos.y /= 100;


						cube[kingCheckPos.x][kingCheckPos.y].setFillColor(Color::Red);
					}
				}
			}


			turn = !turn;

			initFirst = true;

			break;
		}
	}
}


void Chess::logic() {


	clickedPiecePos.x = mousePos.x / 100;
	clickedPiecePos.x *= 100;

	clickedPiecePos.y = mousePos.y / 100;
	clickedPiecePos.y *= 100;


	if (secondActive) secondClick();


	firstClick();


	isGameEnd();


	if (canResetColor) resetColor();

	if (canChangeColor) changeColor();
}






void main()
{
	Chess chess;
}


// add pawn promotion
// add castle
// fix bug: if double check can only move king