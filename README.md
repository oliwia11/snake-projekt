# Projekt: Snake SFML 3.0 🐍

Klasyczna gra Snake zaimplementowana w języku C++ przy użyciu najnowszej biblioteki SFML 3.0.

## 👥 Zespół i wkład w projekt:
* **Oliwia**:
    * Implementacja **Menu Głównego** z obsługą graficzną i legendą.
    * Stworzenie autorskiego **Trybu Nieskończonego** (wąż przenika przez ściany i pojawia się po przeciwnej stronie).
    * Optymalizacja kodu pod standard C++20 oraz centrowanie interfejsu UI.
    * Zarządzanie logiką trybów gry i wyłączanie bonusów w trybie nieskończonym.
* **Dominika**: Przygotowanie zasobów graficznych (tekstury, tło) oraz bazowa mechanika ruchu.
   *  Implementacja mechaniki poruszania opartej na pętli wstecznej i kopiowaniu współrzędnych segmentów ogona.
   * Stworzenie systemu bonusów (Złota Gwiazda) z logiką losowego pojawiania się oraz wartościami punktowymi.
   * Opracowanie logiki timera ograniczającego czas trwania bonusu do 5 sekund (50 klatek).
   * Zastosowanie inteligentnych wskaźników std::unique_ptr do zarządzania pamięcią obiektów graficznych.
   * Przygotowanie oprawy wizualnej poprzez ładowanie tekstur PNG oraz ustalenie kolorystyki "Forest Green" dla interfejsu.
* **Zuzia**: 
   * Zabezpieczenie i trwałość danych (Szyfr Vigenère'a): Implementacja systemu zapisu oraz odczytu             najlepszego wyniku (High Score) z wykorzystaniem polialfabetycznego szyfru podstawieniowego.               Zastosowanie słowa-klucza do przesunięcia znaków w pliku rekord.txt zapewnia integralność danych i
     uniemożliwia prostą edycję rekordów przez użytkownika.
   * Stworzenie Trybu z Przeszkodami wraz z algorytmem losowej generacji kamieni i nową logiką kolizji.
   * Projekt graficznego ekranu Game Over wyświetlającego statystyki oraz podsumowanie wyniku.
   * Obsługa stanów końcowych gry oraz funkcjonalności restartu (R) i wyjścia (ESC) z aplikacji.

## 🎮 Tryby gry:
1. **Klasyczny**: Standardowa rozgrywka z bonusami (gwiazdki) i śmiercią po uderzeniu w ścianę.
2. **Nieskończony (Infinite)**: Tryb bez ścian – idealny do bicia rekordów bez ryzyka uderzenia w krawędź planszy.
3. **Hardcore**: Tryb z dodatkowymi przeszkodami.

## Personalizacja: 
* Menu z możliwością doboru koloru węża.

## ⌨️ Sterowanie:
* **W / S / A / D**: Ruch wężem
* **1 / 2 / 3 / 4 (w menu)**: Wybór trybu gry

## 🛠️ Technologie:
* Język: C++20
* Biblioteka: SFML 3.0 (Snapshot)
* Środowisko: Visual Studio 2022# snake-projekt
projekt zaliczoniowy z podstaw informatyki rok 1. gra snake
