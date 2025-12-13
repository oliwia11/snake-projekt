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

int main(){
	Setup();
	Draw();
}

