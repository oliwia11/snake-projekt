#include <SFML/Graphics.hpp>
#include <iostream>
#include <cstdlib>
#include <ctime>
#include <string>
#include <memory>
#include <optional>
#include <fstream>

// --- KONFIGURACJA ---
const int SZEROKOSC = 40;
const int WYSOKOSC = 20;
const int ROZMIAR = 25;
const int MAX_PRZESZKOD = 15;
const std::string KLUCZ_VIGNERE = "SNAKE"

enum StanGry { MENU, GRA_KLASYCZNA, GRA_NIESKONCZONA, GRA_PRZESZKODY, KONIEC_GRY, PERSONALIZACJA };
StanGry aktualnyStan = MENU;

int wazX[2000], wazY[2000];
int dlugoscWaza = 1;
int owocX, owocY, bonusX, bonusY, bonusCzas = 0, wynik = 0, kierunek = 0;
int najlepszyWynik = 0;
bool jestBonus = false, koniecGry = false;

// TĘCZOWE KOLORY DO WYBORU
sf::Color koloryTeczy[] = {
    sf::Color::Green, sf::Color::Red, sf::Color::Blue,
    sf::Color::Yellow, sf::Color::Magenta, sf::Color::Cyan, sf::Color::White
};
int wybranyIndeksKoloru = 0;
sf::Color kolorWaza = sf::Color::Green;

struct Punkt { int x, y; };
Punkt przeszkody[MAX_PRZESZKOD];
int iloscPrzeszkod = 0;

sf::RenderWindow okno;
sf::Texture teksturaTlo, teksturaJablko, teksturaGwiazda, teksturaGlowa, teksturaKamien, teksturaMenuTlo;
sf::Font czcionka;

std::unique_ptr<sf::Sprite> spriteMenuTlo, spriteTlo, spriteJablko, spriteGwiazda, spriteGlowa, spriteKamien;
std::unique_ptr<sf::Text> napisWynik, napisMenu, napisKoniecGry, napisPerso;
std::unique_ptr<sf::RectangleShape> kwadracikWaza;

void ZapiszRekord() {
    std::ofstream plik("rekord.txt");
    if (plik.is_open()) {
        std::string wynikStr = std::to_string(najlepszyWynik);
        for (size_t i = 0; i < wynikStr.length(); i++) {
            // Przesuwamy znak o wartość litery z klucza
            wynikStr[i] = wynikStr[i] + KLUCZ_VIGENERE[i % KLUCZ_VIGENERE.length()];
        }
        plik << wynikStr;
        plik.close();
    }
}

void WczytajRekord() {
    std::ifstream plik("rekord.txt");
    if (plik.is_open()) {
        std::string zaszyfrowany;
        plik >> zaszyfrowany;
        for (size_t i = 0; i < zaszyfrowany.length(); i++) {
            // Cofamy przesunięcie o tę samą literę z klucza
            zaszyfrowany[i] = zaszyfrowany[i] - KLUCZ_VIGENERE[i % KLUCZ_VIGENERE.length()];
        }
        try {
            najlepszyWynik = std::stoi(zaszyfrowany);
        } catch (...) {
            najlepszyWynik = 0;
        }
        plik.close();
    }
}

void GenerujPrzeszkody() {
    iloscPrzeszkod = 0;
    for (int i = 0; i < MAX_PRZESZKOD; i++) {
        int kX = rand() % SZEROKOSC; int kY = rand() % WYSOKOSC;
        if (kX != SZEROKOSC / 2 && kY != WYSOKOSC / 2) {
            przeszkody[iloscPrzeszkod].x = kX; przeszkody[iloscPrzeszkod].y = kY;
            iloscPrzeszkod++;
        }
    }
}

void ResetujGre() {
    wazX[0] = SZEROKOSC / 2; wazY[0] = WYSOKOSC / 2;
    dlugoscWaza = 1; kierunek = 0; wynik = 0;
    jestBonus = false; koniecGry = false; bonusCzas = 0;
    owocX = rand() % SZEROKOSC; owocY = rand() % WYSOKOSC;
}

void Ustawienia() {
    srand(static_cast<unsigned int>(time(nullptr)));
    WczytajRekord();
    okno.create(sf::VideoMode({ (unsigned int)SZEROKOSC * ROZMIAR, (unsigned int)WYSOKOSC * ROZMIAR + 60 }), "Wonsz SFML - Final");
    okno.setFramerateLimit(10);

    // Ladowanie plikow
    teksturaTlo.loadFromFile("background.png");
    teksturaJablko.loadFromFile("apple.png");
    teksturaGwiazda.loadFromFile("star.png");
    teksturaGlowa.loadFromFile("head.png");
    teksturaKamien.loadFromFile("rock.png");
    teksturaMenuTlo.loadFromFile("menu_bg.png");
    czcionka.openFromFile("arial.ttf");

    if (!czcionka.openFromFile("Play-Bold.ttf")) {
        std::cerr << "Blad: Nie znaleziono pliku Play-Bold.ttf!\n";
    }

    spriteTlo = std::make_unique<sf::Sprite>(teksturaTlo);
    spriteTlo->setScale({ (float)(SZEROKOSC * ROZMIAR) / teksturaTlo.getSize().x, (float)(WYSOKOSC * ROZMIAR) / teksturaTlo.getSize().y });

    spriteJablko = std::make_unique<sf::Sprite>(teksturaJablko);
    spriteJablko->setScale({ (float)ROZMIAR / teksturaJablko.getSize().x, (float)ROZMIAR / teksturaJablko.getSize().y });

    spriteGwiazda = std::make_unique<sf::Sprite>(teksturaGwiazda);
    spriteGwiazda->setScale({ (float)ROZMIAR / teksturaGwiazda.getSize().x, (float)ROZMIAR / teksturaGwiazda.getSize().y });

    spriteGlowa = std::make_unique<sf::Sprite>(teksturaGlowa);
    spriteGlowa->setOrigin({ (float)teksturaGlowa.getSize().x / 2.f, (float)teksturaGlowa.getSize().y / 2.f });
    spriteGlowa->setScale({ ((float)ROZMIAR / teksturaGlowa.getSize().x) * 1.1f, ((float)ROZMIAR / teksturaGlowa.getSize().x) * 1.1f });

    spriteKamien = std::make_unique<sf::Sprite>(teksturaKamien);
    spriteKamien->setScale({ (float)ROZMIAR / teksturaKamien.getSize().x, (float)ROZMIAR / teksturaKamien.getSize().y });

    spriteMenuTlo = std::make_unique<sf::Sprite>(teksturaMenuTlo);
    spriteMenuTlo->setScale({ (float)(SZEROKOSC * ROZMIAR) / teksturaMenuTlo.getSize().x, (float)(WYSOKOSC * ROZMIAR + 60) / teksturaMenuTlo.getSize().y });

    napisWynik = std::make_unique<sf::Text>(czcionka);
    napisWynik->setCharacterSize(20); napisWynik->setPosition({ 10.f, (float)WYSOKOSC * ROZMIAR + 5.f });

    napisMenu = std::make_unique<sf::Text>(czcionka);
    napisMenu->setCharacterSize(25); napisMenu->setFillColor(sf::Color(34, 139, 34));

    napisKoniecGry = std::make_unique<sf::Text>(czcionka);
    napisKoniecGry->setCharacterSize(25); napisKoniecGry->setFillColor(sf::Color(34, 139, 34));

    napisPerso = std::make_unique<sf::Text>(czcionka);
    napisPerso->setCharacterSize(25); napisPerso->setFillColor(sf::Color(34, 139, 34));

    kwadracikWaza = std::make_unique<sf::RectangleShape>(sf::Vector2f{ (float)ROZMIAR - 1.f, (float)ROZMIAR - 1.f });

    ResetujGre();
}

void Logika() {
    if (kierunek == 0 || koniecGry) return;
    for (int i = dlugoscWaza - 1; i > 0; i--) { wazX[i] = wazX[i - 1]; wazY[i] = wazY[i - 1]; }
    if (kierunek == 1) wazX[0]--; if (kierunek == 2) wazX[0]++; if (kierunek == 3) wazY[0]--; if (kierunek == 4) wazY[0]++;

    if (aktualnyStan == GRA_NIESKONCZONA) {
        if (wazX[0] < 0) wazX[0] = SZEROKOSC - 1; else if (wazX[0] >= SZEROKOSC) wazX[0] = 0;
        if (wazY[0] < 0) wazY[0] = WYSOKOSC - 1; else if (wazY[0] >= WYSOKOSC) wazY[0] = 0;
    }
    else if (wazX[0] < 0 || wazX[0] >= SZEROKOSC || wazY[0] < 0 || wazY[0] >= WYSOKOSC) koniecGry = true;

    for (int i = 1; i < dlugoscWaza; i++) if (wazX[i] == wazX[0] && wazY[i] == wazY[0]) koniecGry = true;
    if (aktualnyStan == GRA_PRZESZKODY) {
        for (int i = 0; i < iloscPrzeszkod; i++) if (wazX[0] == przeszkody[i].x && wazY[0] == przeszkody[i].y) koniecGry = true;
    }

    if (wazX[0] == owocX && wazY[0] == owocY) {
        wynik += 10; if (wynik > najlepszyWynik) { najlepszyWynik = wynik; ZapiszRekord(); }
        if (dlugoscWaza < 1999) dlugoscWaza++;
        owocX = rand() % SZEROKOSC; owocY = rand() % WYSOKOSC;
        if (aktualnyStan == GRA_KLASYCZNA && !jestBonus && rand() % 10 < 3) { jestBonus = true; bonusCzas = 50; bonusX = rand() % SZEROKOSC; bonusY = rand() % WYSOKOSC; }
    }
    if (jestBonus) {
        bonusCzas--;
        if (wazX[0] == bonusX && wazY[0] == bonusY) { wynik += 50; if (wynik > najlepszyWynik) { najlepszyWynik = wynik; ZapiszRekord(); } jestBonus = false; }
        if (bonusCzas <= 0) jestBonus = false;
    }
}

void Rysowanie() {
    okno.clear(sf::Color(30, 30, 30));

    if (aktualnyStan == MENU || aktualnyStan == PERSONALIZACJA || aktualnyStan == KONIEC_GRY) {
        if (spriteMenuTlo) okno.draw(*spriteMenuTlo);
        sf::RectangleShape nakladka(sf::Vector2f((float)SZEROKOSC * ROZMIAR, (float)WYSOKOSC * ROZMIAR + 60));
        nakladka.setFillColor(sf::Color(0, 0, 0, 50));
        okno.draw(nakladka);

        if (aktualnyStan == MENU) {
            napisMenu->setString("SNAKE - MENU GLOWNE\n---------------------------\nRekord: " + std::to_string(najlepszyWynik) + "\n\n1. TRYB KLASYCZNY\n2. TRYB NIESKONCZONY\n3. TRYB Z PRZESZKODAMI\n4. PERSONALIZACJA WEZA\n\nSterowanie: W, A, S, D");
            sf::FloatRect b = napisMenu->getLocalBounds();
            napisMenu->setOrigin({ b.position.x + b.size.x / 2.0f, b.position.y + b.size.y / 2.0f });
            napisMenu->setPosition({ (SZEROKOSC * ROZMIAR) / 2.0f, (WYSOKOSC * ROZMIAR + 60) / 2.0f });
            okno.draw(*napisMenu);
        }
        else if (aktualnyStan == PERSONALIZACJA) {
            napisPerso->setString("WYBIERZ KOLOR CIALA:\n\n   <  KOLOR " + std::to_string(wybranyIndeksKoloru + 1) + "  >\n\n[ENTER] - Zatwierdz");
            sf::FloatRect b = napisPerso->getLocalBounds();
            napisPerso->setOrigin({ b.position.x + b.size.x / 2.0f, b.position.y + b.size.y / 2.0f });
            napisPerso->setPosition({ (SZEROKOSC * ROZMIAR) / 2.0f, (WYSOKOSC * ROZMIAR) / 2.0f - 40 });
            okno.draw(*napisPerso);
            kwadracikWaza->setFillColor(koloryTeczy[wybranyIndeksKoloru]);
            kwadracikWaza->setPosition({ (SZEROKOSC * ROZMIAR) / 2.0f - 12, (WYSOKOSC * ROZMIAR) / 2.0f + 60 });
            okno.draw(*kwadracikWaza);
        }
        else if (aktualnyStan == KONIEC_GRY) {
            napisKoniecGry->setString("KONIEC GRY!\n\nTwoj wynik: " + std::to_string(wynik) + "\nRekord: " + std::to_string(najlepszyWynik) + "\n\n---------------------------\n[R] - Powrot do MENU\n[ESC] - Wyjscie");
            sf::FloatRect b = napisKoniecGry->getLocalBounds();
            napisKoniecGry->setOrigin({ b.position.x + b.size.x / 2.0f, b.position.y + b.size.y / 2.0f });
            napisKoniecGry->setPosition({ (SZEROKOSC * ROZMIAR) / 2.0f, (WYSOKOSC * ROZMIAR + 60) / 2.0f });
            okno.draw(*napisKoniecGry);
        }
    }
    else {
        if (spriteTlo) okno.draw(*spriteTlo);
        if (aktualnyStan == GRA_PRZESZKODY) {
            for (int i = 0; i < iloscPrzeszkod; i++) {
                spriteKamien->setPosition({ (float)przeszkody[i].x * ROZMIAR, (float)przeszkody[i].y * ROZMIAR });
                okno.draw(*spriteKamien);
            }
        }
        spriteJablko->setPosition({ (float)owocX * ROZMIAR, (float)owocY * ROZMIAR });
        okno.draw(*spriteJablko);
        if (jestBonus) { spriteGwiazda->setPosition({ (float)bonusX * ROZMIAR, (float)bonusY * ROZMIAR }); okno.draw(*spriteGwiazda); }

        for (int i = 0; i < dlugoscWaza; i++) {
            if (i == 0) {
                if (kierunek == 3) spriteGlowa->setRotation(sf::degrees(180.f));
                else if (kierunek == 4) spriteGlowa->setRotation(sf::degrees(0.f));
                else if (kierunek == 1) spriteGlowa->setRotation(sf::degrees(90.f));
                else if (kierunek == 2) spriteGlowa->setRotation(sf::degrees(270.f));
                spriteGlowa->setPosition({ (float)wazX[i] * ROZMIAR + ROZMIAR / 2.f, (float)wazY[i] * ROZMIAR + ROZMIAR / 2.f });
                okno.draw(*spriteGlowa);
            }
            else {
                kwadracikWaza->setFillColor(kolorWaza);
                kwadracikWaza->setPosition({ (float)wazX[i] * ROZMIAR, (float)wazY[i] * ROZMIAR });
                okno.draw(*kwadracikWaza);
            }
        }
        std::string t = (aktualnyStan == GRA_NIESKONCZONA) ? " [NIESKONCZONA]" : (aktualnyStan == GRA_PRZESZKODY ? " [HARDCORE]" : " [KLASYK]");
        napisWynik->setString("Wynik: " + std::to_string(wynik) + " | Rekord: " + std::to_string(najlepszyWynik) + t);
        okno.draw(*napisWynik);
    }
    okno.display();
}

int main() {
    Ustawienia();
    while (okno.isOpen()) {
        while (const std::optional zdarzenie = okno.pollEvent()) {
            if (zdarzenie->is<sf::Event::Closed>()) okno.close();
            if (const auto* klawisz = zdarzenie->getIf<sf::Event::KeyPressed>()) {
                if (aktualnyStan == MENU) {
                    if (klawisz->code == sf::Keyboard::Key::Num1) aktualnyStan = GRA_KLASYCZNA;
                    if (klawisz->code == sf::Keyboard::Key::Num2) aktualnyStan = GRA_NIESKONCZONA;
                    if (klawisz->code == sf::Keyboard::Key::Num3) { aktualnyStan = GRA_PRZESZKODY; GenerujPrzeszkody(); }
                    if (klawisz->code == sf::Keyboard::Key::Num4) aktualnyStan = PERSONALIZACJA;
                }
                else if (aktualnyStan == PERSONALIZACJA) {
                    if (klawisz->code == sf::Keyboard::Key::Left) wybranyIndeksKoloru = (wybranyIndeksKoloru - 1 + 7) % 7;
                    if (klawisz->code == sf::Keyboard::Key::Right) wybranyIndeksKoloru = (wybranyIndeksKoloru + 1) % 7;
                    if (klawisz->code == sf::Keyboard::Key::Enter) { kolorWaza = koloryTeczy[wybranyIndeksKoloru]; aktualnyStan = MENU; }
                }
                else if (aktualnyStan == KONIEC_GRY) {
                    if (klawisz->code == sf::Keyboard::Key::R) { ResetujGre(); aktualnyStan = MENU; }
                    if (klawisz->code == sf::Keyboard::Key::Escape) okno.close();
                }
                else {
                    if (klawisz->code == sf::Keyboard::Key::A && kierunek != 2) kierunek = 1;
                    if (klawisz->code == sf::Keyboard::Key::D && kierunek != 1) kierunek = 2;
                    if (klawisz->code == sf::Keyboard::Key::W && kierunek != 4) kierunek = 3;
                    if (klawisz->code == sf::Keyboard::Key::S && kierunek != 3) kierunek = 4;
                }
            }
        }
        if (aktualnyStan != MENU && aktualnyStan != KONIEC_GRY && aktualnyStan != PERSONALIZACJA) Logika();
        Rysowanie();
        if (koniecGry) { sf::sleep(sf::seconds(1)); aktualnyStan = KONIEC_GRY; koniecGry = false; }
    }
    return 0;
}
