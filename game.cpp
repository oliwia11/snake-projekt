#include <SFML/Graphics.hpp>
#include <iostream>
#include <cstdlib>
#include <ctime>
#include <string>
#include <memory>
#include <optional>
#include <vector>
#include <fstream>

// --- KONFIGURACJA ---
const int SZEROKOSC = 40;
const int WYSOKOSC = 20;
const int ROZMIAR = 25;

enum StanGry { MENU, GRA_KLASYCZNA, GRA_NIESKONCZONA, GRA_PRZESZKODY};
StanGry aktualnyStan = MENU;

int wazX[2000], wazY[2000];
int dlugoscWaza = 1;
int owocX, owocY, bonusX, bonusY, bonusCzas = 0, wynik = 0, kierunek = 0;
int najlepszyWynik = 0;
bool jestBonus = false, koniecGry = false;

struct Punkt { int x, y; }; //zmienne dla przeszkod
std::vector<Punkt> przeszkody;

sf::RenderWindow okno;
sf::Texture teksturaTlo, teksturaJablko, teksturaGwiazda, teksturaGlowa, teksturaKamien;
sf::Font czcionka;
sf::Texture teksturaMenuTlo;

std::unique_ptr<sf::Sprite> spriteMenuTlo;
std::unique_ptr<sf::Sprite> spriteTlo, spriteJablko, spriteGwiazda, spriteGlowa, spriteKamien;
std::unique_ptr<sf::Text> napisWynik, napisMenu;
std::unique_ptr<sf::RectangleShape> kwadracikWaza;

void WczytajRekord() {
    std::ifstream plik("rekord.txt"); 
    if (plik.is_open()) {
        plik >> najlepszyWynik; 
        plik.close();
    }
}

void ZapiszRekord() {
    std::ofstream plik("rekord.txt"); 
    if (plik.is_open()) {
        plik << najlepszyWynik;
        plik.close();
    }
}

//Funkcja do generowania przeszkod
void GenerujPrzeszkody() {
    przeszkody.clear();
    for(int i=0; i<15; i++) { // Generujemy 15 kamieni
        int kX = rand() % SZEROKOSC;
        int kY = rand() % WYSOKOSC;
        // Upewniamy sie ze kamien nie jest na srodku (gdzie startuje waz)
        if(kX != SZEROKOSC/2 && kY != WYSOKOSC/2) {
            przeszkody.push_back({kX, kY});
        }
    }
}


void Ustawienia() {
    srand(static_cast<unsigned int>(time(nullptr)));

    okno.create(sf::VideoMode({ (unsigned int)SZEROKOSC * ROZMIAR, (unsigned int)WYSOKOSC * ROZMIAR + 60 }), "Wonsz SFML - Projekt");
    okno.setFramerateLimit(10);

    wazX[0] = SZEROKOSC / 2;
    wazY[0] = WYSOKOSC / 2;

    // Ladowanie plikow
    if (!teksturaTlo.loadFromFile("background.png")) std::cerr << "brak background.png\n";
    if (!teksturaJablko.loadFromFile("apple.png")) std::cerr << "brak apple.png\n";
    if (!teksturaGwiazda.loadFromFile("star.png")) std::cerr << "brak star.png\n";
    if (!teksturaGlowa.loadFromFile("head.png")) std::cerr << "brak head.png\n";
    if (!czcionka.openFromFile("arial.ttf")) std::cerr << "brak arial.ttf\n";
    if (!teksturaMenuTlo.loadFromFile("menu_bg.png")) std::cerr << "brak menu_bg.png\n";

    bool maKamien = teksturaKamien.loadFromFile("rock.png");

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
    if(maKamien) spriteKamien->setScale({ (float)ROZMIAR / teksturaKamien.getSize().x, (float)ROZMIAR / teksturaKamien.getSize().y });

    spriteMenuTlo = std::make_unique<sf::Sprite>(teksturaMenuTlo);
    spriteMenuTlo->setScale({
        (float)(SZEROKOSC * ROZMIAR) / teksturaMenuTlo.getSize().x,
        (float)(WYSOKOSC * ROZMIAR + 60) / teksturaMenuTlo.getSize().y
        });

    napisWynik = std::make_unique<sf::Text>(czcionka);
    napisWynik->setCharacterSize(20);
    napisWynik->setPosition({ 10.f, (float)WYSOKOSC * ROZMIAR + 5.f });

    napisMenu = std::make_unique<sf::Text>(czcionka);
    napisMenu->setCharacterSize(25);
    napisMenu->setFillColor(sf::Color::Yellow);
    napisMenu->setString(
        "SNAKE - MENU GLOWNE\n"
        "---------------------------\n"
        "Rekord: " + std::to_string(najlepszyWynik) + "\n\n"
        "1. TRYB KLASYCZNY Z BONUSAMI\n"
        "2. TRYB NIESKONCZONA PLANSZA\n"
        "3. TRYB Z PRZESZKODAMI\n\n"
        "LEGENDA:\n"
        "[ Glowa ] - Twoj waz\n"
        "[ Jablko ] - +10 punktow\n"
        "[ Gwiazda] - +50 punktow (widoczne 5s)\n"
        "---------------------------\n"
        "Sterowanie: W, A, S, D"
    );

    sf::FloatRect bounds = napisMenu->getLocalBounds();
    // Naprawa SFML 3.0 - uzywamy bounds.position i bounds.size
    napisMenu->setOrigin({ bounds.position.x + bounds.size.x / 2.0f, bounds.position.y + bounds.size.y / 2.0f });

    float srodekX = (SZEROKOSC * ROZMIAR) / 2.0f;
    float srodekY = (WYSOKOSC * ROZMIAR + 60) / 2.0f;
    napisMenu->setPosition({ srodekX, srodekY });

    kwadracikWaza = std::make_unique<sf::RectangleShape>(sf::Vector2f{ (float)ROZMIAR - 1.f, (float)ROZMIAR - 1.f });
    kwadracikWaza->setFillColor(sf::Color::Green);

    owocX = rand() % SZEROKOSC; owocY = rand() % WYSOKOSC;
}

void Logika() {
    if (kierunek == 0 || koniecGry) return;

    // Ruch ogona
    for (int i = dlugoscWaza - 1; i > 0; i--) {
        wazX[i] = wazX[i - 1]; wazY[i] = wazY[i - 1];
    }

    // Ruch glowy
    if (kierunek == 1) wazX[0]--;
    if (kierunek == 2) wazX[0]++;
    if (kierunek == 3) wazY[0]--;
    if (kierunek == 4) wazY[0]++;

    // NIESKONCZONA MAPA LUB KLASYK 
    if (aktualnyStan == GRA_NIESKONCZONA) {
        if (wazX[0] < 0) wazX[0] = SZEROKOSC - 1;
        else if (wazX[0] >= SZEROKOSC) wazX[0] = 0;
        if (wazY[0] < 0) wazY[0] = WYSOKOSC - 1;
        else if (wazY[0] >= WYSOKOSC) wazY[0] = 0;
    }
    else {
        if (wazX[0] < 0 || wazX[0] >= SZEROKOSC || wazY[0] < 0 || wazY[0] >= WYSOKOSC) koniecGry = true;
    }

    // Kolizja z samym soba
    for (int i = 1; i < dlugoscWaza; i++) {
        if (wazX[i] == wazX[0] && wazY[i] == wazY[0]) koniecGry = true;
    }
    //Kolizja z kamieniami (tylko w trybie 3)
    if (aktualnyStan == GRA_PRZESZKODY) {
        for (const auto& kamien : przeszkody) {
            if (wazX[0] == kamien.x && wazY[0] == kamien.y) {
                koniecGry = true;
            }
        }
    }
    // Jedzenie owocow
    if (wazX[0] == owocX && wazY[0] == owocY) {
        wynik += 10;
        if (wynik > najlepszyWynik) { //zmiana
            najlepszyWynik = wynik;
            ZapiszRekord(); 
        }
        if (dlugoscWaza < 1999) dlugoscWaza++;
        
        //jablko nie zrespi sie na kamieniu
        do {
            owocX = rand() % SZEROKOSC; 
            owocY = rand() % WYSOKOSC;
            bool kolizjaZKamieniem = false;
            if(aktualnyStan == GRA_PRZESZKODY) {
                for(auto& k : przeszkody) if(k.x == owocX && k.y == owocY) kolizjaZKamieniem = true;
            }
            if(!kolizjaZKamieniem) break;
        } while(true);
   

        // Szansa na bonus (tylko w klasyku)
        if (aktualnyStan == GRA_KLASYCZNA && !jestBonus && rand() % 10 < 3) {
            jestBonus = true; bonusCzas = 50;
            bonusX = rand() % SZEROKOSC; bonusY = rand() % WYSOKOSC;
        }
    }

    if (jestBonus) {
        bonusCzas--;
        if (wazX[0] == bonusX && wazY[0] == bonusY) { 
        wynik += 50;
        if (wynik > najlepszyWynik) { //sprawdzenie rekordu przy bonusie
                najlepszyWynik = wynik;
                ZapiszRekord();
            }
        jestBonus = false; }
        if (bonusCzas <= 0) jestBonus = false;
    }
}

void Rysowanie() {
    okno.clear(sf::Color(30, 30, 30));

    if (aktualnyStan == MENU) {
        if (spriteMenuTlo) okno.draw(*spriteMenuTlo);
        sf::RectangleShape nakladka(sf::Vector2f((float)SZEROKOSC * ROZMIAR, (float)WYSOKOSC * ROZMIAR + 60));
        nakladka.setFillColor(sf::Color(0, 0, 0, 100));
        okno.draw(nakladka);
        std::string menuStr = "SNAKE - MENU GLOWNE\n---------------------------\nRekord: " + std::to_string(najlepszyWynik) + "\n\n1. TRYB KLASYCZNY Z BONUSAMI\n2. TRYB NIESKONCZONA PLANSZA\n3. TRYB Z PRZESZKODAMI\n\nSterowanie: W, A, S, D";
        napisMenu->setString(menuStr);
        okno.draw(*napisMenu);
    }
    else {
        if (spriteTlo) okno.draw(*spriteTlo);

        //rysowanie przeszkod
        if (aktualnyStan == GRA_PRZESZKODY) {
            for (const auto& kamien : przeszkody) {
                if (spriteKamien && teksturaKamien.getSize().x > 0) {
                    spriteKamien->setPosition({ (float)kamien.x * ROZMIAR, (float)kamien.y * ROZMIAR });
                    okno.draw(*spriteKamien);
                }
                }
            }
        }
        spriteJablko->setPosition({ (float)owocX * ROZMIAR, (float)owocY * ROZMIAR });
        okno.draw(*spriteJablko);

        if (jestBonus) {
            spriteGwiazda->setPosition({ (float)bonusX * ROZMIAR, (float)bonusY * ROZMIAR });
            okno.draw(*spriteGwiazda);
        }

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
                kwadracikWaza->setPosition({ (float)wazX[i] * ROZMIAR, (float)wazY[i] * ROZMIAR });
                okno.draw(*kwadracikWaza);
            }
        }
       // Wyswietlanie rekordu podczas gry
        std::string trybNapis = "";
        if(aktualnyStan == GRA_NIESKONCZONA) trybNapis = " [NIESKONCZONA]";
        else if(aktualnyStan == GRA_PRZESZKODY) trybNapis = " [HARDCORE]";
        else trybNapis = " [KLASYK]";
        
        napisWynik->setString("Wynik: " + std::to_string(wynik) + (aktualnyStan == GRA_NIESKONCZONA ? " [NIESKONCZONA]" : " [KLASYK]"));
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
                     if (klawisz->code == sf::Keyboard::Key::Num3) {
                        aktualnyStan = GRA_PRZESZKODY;
                        GenerujPrzeszkody();
                     }
                }
                else {
                    if (klawisz->code == sf::Keyboard::Key::A && kierunek != 2) kierunek = 1;
                    if (klawisz->code == sf::Keyboard::Key::D && kierunek != 1) kierunek = 2;
                    if (klawisz->code == sf::Keyboard::Key::W && kierunek != 4) kierunek = 3;
                    if (klawisz->code == sf::Keyboard::Key::S && kierunek != 3) kierunek = 4;
                }
            }
        }

        if (aktualnyStan != MENU) Logika();
        Rysowanie();

        if (koniecGry) {
            sf::sleep(sf::seconds(2));
            okno.close();
        }
    }
    return 0;
}
