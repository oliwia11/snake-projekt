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
            else { 				//rysowanie ogona
                bool print = false;
                for (int k = 0; k < nTail; k++) {
                    if (tailX[k] == j && tailY[k] == i) {
                        cout << "o";
                        print = true;
                    }
                }
                if (!print) cout << " ";
            }

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
void Logic() { // logika gry - ruch ogona, kolizje, owoce
    int prevX = tailX[0];
    int prevY = tailY[0];
    int prev2X, prev2Y;
    tailX[0] = x; // pierwszy element ogona to pozycja glowy
    tailY[0] = y;
    for (int i = 1; i < nTail; i++) {
        prev2X = tailX[i];
        prev2Y = tailY[i];
        tailX[i] = prevX;
        tailY[i] = prevY;
        prevX = prev2X;
        prevY = prev2Y;
    } // ruch glowy w zaleznosci od kierunku
    switch (dir) {
    case LEFT:  x--; break;
    case RIGHT: x++; break;
    case UP:    y--; break;
    case DOWN:  y++; break;
    default:    break;
    }
    if (x >= WIDTH || x < 0 || y >= HEIGHT || y < 0) gameOver = true; // kolizje ze scianami
    for (int i = 0; i < nTail; i++) if (tailX[i] == x && tailY[i] == y) gameOver = true; //kolizje z ogonem
    if (x == fruitX && y == fruitY) { // jedzenie owoca
        score += 10;
        nTail++; // wydluzenie owoca
        fruitX = rand() % WIDTH; // nowy owoc
        fruitY = rand() % HEIGHT;
    }
}

int main() {
    Setup();
    while (!gameOver) {
        Draw();
        Input();
        Logic();
        Sleep(50); //kontrola predkosci weza
    }
    return 0;
}


