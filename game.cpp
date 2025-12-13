#include <iostream>
#include <conio.h>
#include <windows.h>

using namespace std;

//wielkosc mapy
const int WIDTH = 40;
const int HEIGHT = 20;

//zmienne globalne 
int x, y;
int fruitX, fruitY;
int score;

int tailX[100], tailY[100];
int nTail;

enum eDirection { STOP = 0, LEFT, RIGHT, UP, DOWN };
eDirection dir;

bool gameOver;

//eliminacja migotania dla estetyki
void GotoXY(int x, int y) {
    COORD coord;
    coord.X = x;
    coord.Y = y;
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);
}

//ustawienia poczatkowe gry
void Setup() {
    gameOver = false;
    dir = STOP;
    x = WIDTH / 2;
    y = HEIGHT / 2;

    fruitX = rand() % WIDTH;
    fruitY = rand() % HEIGHT;
    score = 0;
    nTail = 0;
}

//plansza 
void Draw() {
    GotoXY(0, 0);   //dla plynnosci gry

    for (int i = 0; i < WIDTH + 2; i++) cout << "#";
    cout << endl;

    for (int i = 0; i < HEIGHT; i++) {
        for (int j = 0; j < WIDTH; j++) {
            if (j == 0) cout << "#";

            if (i == y && j == x)
                cout << "O";
            else if (i == fruitY && j == fruitX)
                cout << "@";
            else
                cout << " ";

            if (j == WIDTH - 1) cout << "#";
        }
        cout << endl;
    }

    for (int i = 0; i < WIDTH + 2; i++) cout << "#";
    cout << endl;

    cout << "Wynik: " << score << endl;
}

void Input() {  //kontrala oblugi klawiatury i poruszania sie 
    if (_kbhit()) { 
        switch (_getch()) {
        case 'a':
            if (dir != RIGHT) dir = LEFT; //zeby sie nie cofal
            break;
        case 'd':
            if (dir != LEFT) dir = RIGHT;
            break;
        case 'w':
            if (dir != DOWN) dir = UP;
            break;
        case 's':
            if (dir != UP) dir = DOWN;
            break;
        case 'x':
            gameOver = true;
            break;
        }
    }
}

int main(){
	Setup();
	Draw();
}

