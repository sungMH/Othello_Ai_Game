#include <bangtal>

using namespace bangtal;

ObjectPtr board[8][8];
int numOfCol[8][8]; //흰돌 0으로 저장, 검은돌 1로 저장


void resetCol(void) {
	for (int x = 0; x < 8; x++) {
		for (int y = 0; y < 8; y++) {
			numOfCol[y][x] = -1;
		}
	}
}

enum class State {
	BLANK,
	POSSIBLE,
	BLACK,
	WHITE
};
State board_state[8][8];//보드의 상태 저장할 곳
enum class Turn {
	BLACK,
	WHITE

};
Turn turn = Turn::BLACK;

void setState(int x, int y, State state) {
	switch (state)
	{
	case State::BLANK:board[y][x]->setImage("images/blank.png"); break;
	case State::POSSIBLE:board[y][x]->setImage(turn == Turn::BLACK ? "images/black possible.png" : "images/white possible.png");break;
	case State::BLACK:board[y][x]->setImage("images/black.png"); break;
	case State::WHITE:board[y][x]->setImage("images/white.png"); break;
	}
	board_state[y][x] = state;
	if (state != State::BLANK) {
		if (state == State::BLACK) {
			numOfCol[y][x] = 1;
		}
		else if(state == State::WHITE){
			numOfCol[y][x] = 0;
		}
	}
}



bool checkPossible(int x, int y, int dx, int dy) {

	State self = turn == Turn::BLACK ? State::BLACK : State::WHITE;
	State other = turn == Turn::BLACK ? State::WHITE : State::BLACK;

	bool possible = false;

	//방향을 나아가면 서 체크 other. other ..self 나와야만 나는 돌을 내껄로 만들 수 있으므로 돌을 놓을 수 있다
	for (x += dx, y += dy; x >= 0 && x < 8 && y >= 0 && y < 8; x += dx, y += dy) {
		if (board_state[y][x] == other) {//지금 나와 반대돌이 나옴
			possible = true;
		}
		else if (board_state[y][x] == self) {
			//지금 나와 같은 돌이 나옴
			//이전에 다른돌이 나왔다면 둘수 있으므로 possible 이 true로 변했을 것이고
			//바로 self가 나왔다면 둘 수 없으므로 possible  false로 변했을것이다.
			return possible;
		}
		else {
			return false;
		}
	}

	return false;
}

bool checkPossible(int x, int y) {


	if (board_state[y][x] == State::BLACK)return false;
	if (board_state[y][x] == State::WHITE)return false;
	setState(x, y, State::BLANK);//기존의 possible인 상태가 아니라 블랭크로 바꾸어야 한다.

	int delta[8][2] = {
		{0,1},
		{1,1},
		{1,0},
		{1,-1},
		{0,-1},
		{-1,-1},
		{-1,0},
		{-1,1}
	};

	bool possible = false;
	for (auto d : delta) {
		if (checkPossible(x, y, d[0], d[1])) possible = true;
	}

	return possible;
}

void reverse(int x, int y, int dx, int dy) {
	State self = turn == Turn::BLACK ? State::BLACK : State::WHITE;
	State other = turn == Turn::BLACK ? State::WHITE : State::BLACK;

	bool possible = false;

	//방향을 나아가면 서 체크 other. other ..self 나와야만 나는 돌을 내껄로 만들 수 있으므로 돌을 놓을 수 있다
	for (x += dx, y += dy; x >= 0 && x < 8 && y >= 0 && y < 8; x += dx, y += dy) {
		if (board_state[y][x] == other) {//지금 나와 반대돌이 나옴
			possible = true;
		}
		else if (board_state[y][x] == self) {
			//지금 나와 같은 돌이 나옴
			if (possible) {//뒤집을 수 있다
				//가운데 조건 없어도 무한루프 안됨 왜냐면 이미 self  other other ...으로 이루어진것을 안다!
				for (x -= dx, y -= dy; x >= 0 && x < 8 && y >= 0 && y < 8; x -= dx, y -= dy) {
					if (board_state[y][x] == other) {
						setState(x, y, self);
					}
					else {
						return;
					}
				}
			}
		}
		else {
			return;
		}
	}

	return;
}

void reverse(int x, int y) {
	int delta[8][2] = {
		{0,1},
		{1,1},
		{1,0},
		{1,-1},
		{0,-1},
		{-1,-1},
		{-1,0},
		{-1,1}
	};

	/*
	for (int i = 0; i < 8; i++) {
		checkPossible(x, y, delta[i][0], delta[i][1]);
	}*/

	bool possible = false;
	for (auto d : delta) {
		reverse(x, y, d[0], d[1]);
	}

}

int comPossPoint[8][8];//매턴 컴퓨터가 놓을 수 있는 자리를 기억한다. 1이 가능한자리

bool setPossible() {

	//8x8의 모든 위치에 대해서
	//놓여질 수 있으면, 상태를 POSSIBLE로 바꾼다
	bool possible = false;
	for (int y = 0; y < 8; y++) {
		for (int x = 0; x < 8; x++) {
			if (checkPossible(x, y)) {
				comPossPoint[y][x] = 1;
				setState(x, y, State::POSSIBLE);
				possible = true;
			}
			else {
				comPossPoint[y][x] = 0;
			}
		}
	}
	return possible;
}



int countPossible(int x, int y, int dx, int dy) {//뒤집을 수 있는 돌의 갯수 반환

	State self = turn == Turn::BLACK ? State::BLACK : State::WHITE;
	State other = turn == Turn::BLACK ? State::WHITE : State::BLACK;

	int possible = 0;

	//방향을 나아가면 서 체크 other. other ..self 나와야만 나는 돌을 내껄로 만들 수 있으므로 돌을 놓을 수 있다
	for (x += dx, y += dy; x >= 0 && x < 8 && y >= 0 && y < 8; x += dx, y += dy) {
		if (board_state[y][x] == other) {//지금 나와 반대돌이 나옴
			possible += 1;
		}
		else if (board_state[y][x] == self) {
			//지금 나와 같은 돌이 나옴
			//이전에 다른돌이 나왔다면 둘수 있으므로 possible 이 true로 변했을 것이고
			//바로 self가 나왔다면 둘 수 없으므로 possible  false로 변했을것이다.
			return possible;
		}
		else {
			return 0;
		}
	}

	return 0;
}


int countPossible(int x, int y) {


	if (board_state[y][x] == State::BLACK)return 0;
	if (board_state[y][x] == State::WHITE)return 0;
	setState(x, y, State::BLANK);//기존의 possible인 상태가 아니라 블랭크로 바꾸어야 한다.

	int delta[8][2] = {
		{0,1},
		{1,1},
		{1,0},
		{1,-1},
		{0,-1},
		{-1,-1},
		{-1,0},
		{-1,1}
	};


	int possible = 0, adder = 0;
	for (auto d : delta) {
		if ((adder=countPossible(x, y, d[0], d[1]))!=0) possible +=adder;
	}

	return possible;
}


bool computerPlay(void) {
	int comX = -1, comY = -1;//가장 많이 검은돌을 뒤집을 수 있는 흰돌 가능한 곳 저장 
	int mostPoss = -1;

	if (setPossible()) { //바꿀 수 있다면

		for (int y = 0; y < 8; y++) {
			for (int x = 0; x < 8; x++) {
				if (comPossPoint[y][x] == 1) {//가능한 자리
					//if (comX < 0 && comY < 0) {
						int tempPoss = countPossible(x, y);
						if (mostPoss < tempPoss) {
							mostPoss = tempPoss;//가장 검은돌 많이 뒤집을 수 있는 곳 저장
							comX = x, comY = y;
						}
					//}
				}
			}
		}
		//여기 반드시 필요
		if (comX >= 0 && comY >= 0) {
			setState(comX, comY, State::WHITE);
			reverse(comX, comY);
		}


		turn = Turn::BLACK;
		return true;
	}
	else {
		turn = Turn::BLACK;

		return false;
	}
}


int main() {
	setGameOption(GameOption::GAME_OPTION_ROOM_TITLE, false);
	setGameOption(GameOption::GAME_OPTION_MESSAGE_BOX_BUTTON, false);
	setGameOption(GameOption::GAME_OPTION_INVENTORY_BUTTON, false);


	resetCol();

	char numb[10][20] = { "images/L0.png", "images/L1.png", "images/L2.png", "images/L3.png", "images/L4.png", "images/L5.png", "images/L6.png", "images/L7.png", "images/L8.png", "images/L9.png" };

	auto scene = Scene::create("Othello", "images/background.png");
	auto black10 = Object::create("images/L0.png", scene, 750, 200,false);
	auto black1 = Object::create("images/L0.png", scene, 830, 200,false);
	auto white10 = Object::create("images/L0.png", scene, 1070, 200,false);
	auto white1 = Object::create("images/L0.png", scene, 1150, 200,false);

	for (int y = 0; y < 8; y++) {
		for (int x = 0; x < 8; x++) {
			board[y][x] = Object::create("images/blank.png", scene, 40 + x * 80, 40 + y * 80);
			board[y][x]->setOnMouseCallback([&, x, y](ObjectPtr, int, int, MouseAction)->bool {//x,y는 값을 카피 레퍼런스로 접근하면 나중에 증가된 x,y값을 참조
				if (board_state[y][x] == State::POSSIBLE) {
					int numOfW = 0, numOfB = 0;
					

					if (turn == Turn::BLACK) {
						numOfW = 0; numOfB = 0;
						setState(x, y, State::BLACK);
						reverse(x, y);
						for (int i = 0; i < 8; i++) {
							for (int j = 0; j < 8; j++) {
								if (numOfCol[j][i] == 0) {
									numOfW ++;
								}
								else if (numOfCol[j][i] == 1) {
									numOfB ++;
								}
							}
						}
							if (numOfB > 9) {
								int a1 = numOfB / 10;
								black10->setImage(numb[a1]);
								black10->show();
								int a2 = numOfB % 10;
								black1->setImage(numb[a2]);
								black1->show();
							}
							else {
								black10->hide();
								black1->setImage(numb[numOfB]);
								black1->show();
							}

							if (numOfW > 9) {
								int a1 = numOfW / 10;
								white10->setImage(numb[a1]);
								white10->show();
								int a2 = numOfW % 10;
								white1->setImage(numb[a2]);
								white1->show();
							
							}
							else {
								white1->hide();
								white10->setImage(numb[numOfW]);
								white10->show();
							}
						turn = Turn::WHITE;
					//흰돌 컴퓨터 차례
						numOfW = 0; numOfB = 0;
						computerPlay();
						for (int i = 0; i < 8; i++) {
							for (int j = 0; j < 8; j++) {
								if (numOfCol[j][i] == 0) {
									numOfW ++;
								}
								else if (numOfCol[j][i] == 1) {
									numOfB ++;
								}
							}
						}
						if (numOfB > 9) {
							int a1 = numOfB / 10;
							black10->setImage(numb[a1]);
							black10->show();
							int a2 = numOfB % 10;
							black1->setImage(numb[a2]);
							black1->show();
						}
						else {
							black10->hide();
							black1->setImage(numb[numOfB]);
							black1->show();
						}

						if (numOfW > 9) {
							int a1 = numOfW / 10;
							white10->setImage(numb[a1]);
							white10->show();
							int a2 = numOfW % 10;
							white1->setImage(numb[a2]);
							white1->show();

						}
						else {
							white1->hide();
							white10->setImage(numb[numOfW]);
							white10->show();
						}
				
					if (!setPossible()) {
						turn = turn == Turn::BLACK ? Turn::WHITE : Turn::BLACK; //toggle

						numOfW = 0; numOfB = 0;
						//둘다 안되는 경우
						if ((turn == Turn::WHITE&&!computerPlay())){ //|| (turn == Turn::BLACK && !setPossible())) {
							for (int i = 0; i < 8; i++) {
								for (int j = 0; j < 8; j++) {
									if (numOfCol[j][i] == 0) {
										numOfW += 1;
									}
									else if (numOfCol[j][i] == 1) {
										numOfB += 1;
									}
								}
							}
							if (numOfB > 9) {
								int a1 = numOfB / 10;
								black10->setImage(numb[a1]);
								black10->show();
								int a2 = numOfB % 10;
								black1->setImage(numb[a2]);
								black1->show();
							}
							else {
								black10->hide();
								black1->setImage(numb[numOfB]);
								black1->show();
							}

							if (numOfW > 9) {
								int a1 = numOfW / 10;
								white10->setImage(numb[a1]);
								white10->show();
								int a2 = numOfW % 10;
								white1->setImage(numb[a2]);
								white1->show();

							}
							else {
								white1->hide();
								white10->setImage(numb[numOfW]);
								white10->show();
							}
							if (numOfB > numOfW) {
								showMessage("End Game!!!, 인간승리!");
							}
							else if (numOfW > numOfB) {
								showMessage("End Game!!!, 컴퓨터 승리!");
							}
							else {
								showMessage("End Game!!!, 무승부입니다.!");
							}
					

						}
						else {
							for (int i = 0; i < 8; i++) {
								for (int j = 0; j < 8; j++) {
									if (numOfCol[j][i] == 0) {
										numOfW += 1;
									}
									else if (numOfCol[j][i] == 1) {
										numOfB += 1;
									}
								}
							}
							if (numOfB > 9) {
								int a1 = numOfB / 10;
								black10->setImage(numb[a1]);
								black10->show();
								int a2 = numOfB % 10;
								black1->setImage(numb[a2]);
								black1->show();
							}
							else {
								black10->hide();
								black1->setImage(numb[numOfB]);
								black1->show();
							}

							if (numOfW > 9) {
								int a1 = numOfW / 10;
								white10->setImage(numb[a1]);
								white10->show();
								int a2 = numOfW % 10;
								white1->setImage(numb[a2]);
								white1->show();

							}
							else {
								white1->hide();
								white10->setImage(numb[numOfW]);
								white10->show();
							}
						}
						}
					}

				}

				return true;
				});
			board_state[y][x] = State::BLANK;
		}
	}

	setState(3, 3, State::BLACK); setState(4, 4, State::BLACK);
	setState(4, 3, State::WHITE); setState(3, 4, State::WHITE);

	setPossible();

	startGame(scene);
}