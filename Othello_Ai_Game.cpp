#include <bangtal>

using namespace bangtal;

ObjectPtr board[8][8];

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
	case State::POSSIBLE:board[y][x]->setImage(turn == Turn::BLACK ? "images/black possible.png" : "images/white possible.png"); break;
	case State::BLACK:board[y][x]->setImage("images/black.png"); break;
	case State::WHITE:board[y][x]->setImage("images/white.png"); break;
	}
	board_state[y][x] = state;
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

	/*
	for (int i = 0; i < 8; i++) {
		checkPossible(x, y, delta[i][0], delta[i][1]);
	}*/

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

bool setPossible() {

	//8x8의 모든 위치에 대해서
	//놓여질 수 있으면, 상태를 POSSIBLE로 바꾼다
	bool possible = false;
	for (int y = 0; y < 8; y++) {
		for (int x = 0; x < 8; x++) {
			if (checkPossible(x, y)) {
				setState(x, y, State::POSSIBLE);
				possible = true;
			}
		}
	}
	return possible;
}

int main() {
	setGameOption(GameOption::GAME_OPTION_ROOM_TITLE, false);
	setGameOption(GameOption::GAME_OPTION_MESSAGE_BOX_BUTTON, false);
	setGameOption(GameOption::GAME_OPTION_INVENTORY_BUTTON, false);

	auto scene = Scene::create("Othello", "images/background.png");

	for (int y = 0; y < 8; y++) {
		for (int x = 0; x < 8; x++) {
			board[y][x] = Object::create("images/blank.png", scene, 40 + x * 80, 40 + y * 80);
			board[y][x]->setOnMouseCallback([&, x, y](ObjectPtr, int, int, MouseAction)->bool {//x,y는 값을 카피 레퍼런스로 접근하면 나중에 증가된 x,y값을 참조
				if (board_state[y][x] == State::POSSIBLE) {
					if (turn == Turn::BLACK) {
						setState(x, y, State::BLACK);
						reverse(x, y);
						turn = Turn::WHITE;
					}
					else {
						setState(x, y, State::WHITE);
						reverse(x, y);
						turn = Turn::BLACK;
					}
					if (!setPossible()) {
						turn = turn == Turn::BLACK ? Turn::WHITE : Turn::BLACK; //toggle

						//둘다 안되는 경우
						if (!setPossible()) {
							showMessage("End Game!!!");
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