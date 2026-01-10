#include <SFML/Graphics.hpp>
#include <iostream>
#include <cstdlib>
#include <ctime>
#include <string>
#include <memory>
#include <optional>

const int SZEROKOSC = 40;
const int WYSOKOSC = 20;
const int ROZMIAR = 25;

int wazX[2000], wazY[2000];
int dlugoscWaza = 1;
int owocX, owocY, bonusX, bonusY, bonusCzas = 0, wynik = 0, kierunek = 0;
bool jestBonus = false, koniecGry = false;

sf::RenderWindow okno;
sf::Texture teksturaTlo, teksturaJablko, teksturaGwiazda, teksturaGlowa;
sf::Font czcionka;

std::unique_ptr<sf::Sprite> spriteTlo, spriteJablko, spriteGwiazda, spriteGlowa;
std::unique_ptr<sf::Text> napisWynik;
std::unique_ptr<sf::RectangleShape> kwadracikWaza;

void Ustawienia() {
    srand(static_cast<unsigned int>(time(nullptr)));

    okno.create(sf::VideoMode({ (unsigned int)SZEROKOSC * ROZMIAR, (unsigned int)WYSOKOSC * ROZMIAR + 40 }), "Wonsz SFML 3.0");
    okno.setFramerateLimit(10);

    wazX[0] = SZEROKOSC / 2;
    wazY[0] = WYSOKOSC / 2;

    if (!teksturaTlo.loadFromFile("background.png")) std::cerr << "brak background.png\n";
    if (!teksturaJablko.loadFromFile("apple.png")) std::cerr << "brak apple.png\n";
    if (!teksturaGwiazda.loadFromFile("star.png")) std::cerr << "brak star.png\n";
    if (!teksturaGlowa.loadFromFile("head.png")) std::cerr << "brak head.png\n";
    if (!czcionka.openFromFile("arial.ttf")) std::cerr << "brak arial.ttf\n";

    spriteTlo = std::make_unique<sf::Sprite>(teksturaTlo);
    spriteTlo->setScale({ (float)(SZEROKOSC * ROZMIAR) / teksturaTlo.getSize().x,
                          (float)(WYSOKOSC * ROZMIAR) / teksturaTlo.getSize().y });

    spriteJablko = std::make_unique<sf::Sprite>(teksturaJablko);
    spriteJablko->setScale({ (float)ROZMIAR / teksturaJablko.getSize().x,
                             (float)ROZMIAR / teksturaJablko.getSize().y });

    spriteGwiazda = std::make_unique<sf::Sprite>(teksturaGwiazda);
    spriteGwiazda->setScale({ (float)ROZMIAR / teksturaGwiazda.getSize().x,
                              (float)ROZMIAR / teksturaGwiazda.getSize().y });

    spriteGlowa = std::make_unique<sf::Sprite>(teksturaGlowa);
    float skalaGlowy = ((float)ROZMIAR / teksturaGlowa.getSize().x) * 1.1f;
    spriteGlowa->setScale({ skalaGlowy, skalaGlowy });
    spriteGlowa->setOrigin({ (float)teksturaGlowa.getSize().x / 2.f, (float)teksturaGlowa.getSize().y / 2.f });

    napisWynik = std::make_unique<sf::Text>(czcionka);
    napisWynik->setCharacterSize(20);
    napisWynik->setPosition({ 10.f, (float)WYSOKOSC * ROZMIAR + 5.f });

    kwadracikWaza = std::make_unique<sf::RectangleShape>(sf::Vector2f{ (float)ROZMIAR - 1.f, (float)ROZMIAR - 1.f });
    kwadracikWaza->setFillColor(sf::Color::Green);

    owocX = rand() % SZEROKOSC;
    owocY = rand() % WYSOKOSC;
}

void Klawisze() {
    while (const std::optional zdarzenie = okno.pollEvent()) {
        if (zdarzenie->is<sf::Event::Closed>()) {
            okno.close();
        }

        if (const auto* klawisz = zdarzenie->getIf<sf::Event::KeyPressed>()) {
            if (klawisz->code == sf::Keyboard::Key::A && kierunek != 2) kierunek = 1;
            if (klawisz->code == sf::Keyboard::Key::D && kierunek != 1) kierunek = 2;
            if (klawisz->code == sf::Keyboard::Key::W && kierunek != 4) kierunek = 3;
            if (klawisz->code == sf::Keyboard::Key::S && kierunek != 3) kierunek = 4;
        }
    }
}

void Logika() {
    if (kierunek == 0) return;

    // ruch ogona
    for (int i = dlugoscWaza - 1; i > 0; i--) {
        wazX[i] = wazX[i - 1];
        wazY[i] = wazY[i - 1];
    }

    // ruch glowy
    if (kierunek == 1) wazX[0]--;
    if (kierunek == 2) wazX[0]++;
    if (kierunek == 3) wazY[0]--;
    if (kierunek == 4) wazY[0]++;

    // kolizje
    if (wazX[0] < 0 || wazX[0] >= SZEROKOSC || wazY[0] < 0 || wazY[0] >= WYSOKOSC) koniecGry = true;
    for (int i = 1; i < dlugoscWaza; i++) {
        if (wazX[i] == wazX[0] && wazY[i] == wazY[0]) koniecGry = true;
    }

    // zjadanie
    if (wazX[0] == owocX && wazY[0] == owocY) {
        wynik += 10;
        if (dlugoscWaza < 1999) dlugoscWaza++;
        owocX = rand() % SZEROKOSC; owocY = rand() % WYSOKOSC;

        if (!jestBonus && rand() % 10 < 3) {
            jestBonus = true; bonusCzas = 50;
            bonusX = rand() % SZEROKOSC; bonusY = rand() % WYSOKOSC;
        }
    }

    if (jestBonus) {
        bonusCzas--;
        if (wazX[0] == bonusX && wazY[0] == bonusY) { wynik += 50; jestBonus = false; }
        if (bonusCzas <= 0) jestBonus = false;
    }
}

void Rysowanie() {
    okno.clear(sf::Color(30, 30, 30));
    if (spriteTlo) okno.draw(*spriteTlo);

    spriteJablko->setPosition({ (float)owocX * ROZMIAR, (float)owocY * ROZMIAR });
    okno.draw(*spriteJablko);

    if (jestBonus) {
        spriteGwiazda->setPosition({ (float)bonusX * ROZMIAR, (float)bonusY * ROZMIAR });
        okno.draw(*spriteGwiazda);
    }

    for (int i = 0; i < dlugoscWaza; i++) {
        if (i == 0) {
            // obracanie glowy
            if (kierunek == 3) spriteGlowa->setRotation(sf::degrees(180.f));
            else if (kierunek == 4) spriteGlowa->setRotation(sf::degrees(0.f));
            else if (kierunek == 1) spriteGlowa->setRotation(sf::degrees(90.f));
            else if (kierunek == 2) spriteGlowa->setRotation(sf::degrees(270.f));

            spriteGlowa->setPosition({ (float)wazX[i] * ROZMIAR + ROZMIAR / 2.f,
                                       (float)wazY[i] * ROZMIAR + ROZMIAR / 2.f });
            okno.draw(*spriteGlowa);
        }
        else {
            kwadracikWaza->setPosition({ (float)wazX[i] * ROZMIAR, (float)wazY[i] * ROZMIAR });
            okno.draw(*kwadracikWaza);
        }
    }

    napisWynik->setString("wynik: " + std::to_string(wynik));
    okno.draw(*napisWynik);
    okno.display();
}

int main() {
    Ustawienia();
    while (okno.isOpen()) {
        Klawisze();
        if (koniecGry) {
            okno.close();
        }
        Logika();
        Rysowanie();
    }
    return 0;
}