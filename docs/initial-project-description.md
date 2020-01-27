---
lang: pl
papersize: a4
pagestyle: empty
header-includes: |
  \usepackage{wrapfig}
  \usepackage{graphicx}
---

# Projekt końcowy WdPwjC: WeźMnie, autor: Paweł Zmarzły 314569

## Polecenie

S9. Prosty system rezerwacji sprzętu projekcyjnego (rzutniki, laptopy, itp.)
w instytucie w czasie jednego semestru. Sprzęt można rezerwować na cały semestr lub
na pojedynczy termin. System powinien umożliwić wprowadzanie, usuwanie i edytowanie opisów
sprzętów i rezerwacji oraz drukować tygodniowy plan rezerwacji dla podanego tygodnia,
a także informować, który sprzęt jest wolny w podanym terminie. Można zaproponować
inne sensowne zapytania do systemu.

System rejestracji danych powinien zapisywać wszystkie wprowadzone dane do plików,
odczytywać je przy kolejnym uruchomieniu programu oraz umożliwiać korektę i usuwanie
wcześniej wprowadzonych informacji.

## Opis interakcji użytkownika z programem oraz realizowanych przez program funkcji

Program będzie operował na bazach danych będących reprezentacją rezerwacji w danym semestrze.
Podział „1 semestr = 1 plik” pozwoli na łatwe przygotowanie następnego semestru (kopiując poprzedni
lub kopiując szablon) oraz równoczesną pracę na wielu semestrach. Program będzie wykonany z użyciem
biblioteki GTK 3 i przeznaczony dla Ubuntu 16.04.

Po uruchomieniu programu pojawi się okno pozwalające na stworzenie nowej bazy danych
(nowego semestru) lub wczytanie już istniejącej. Podając ścieżkę do pliku jako parametr
wywołania programu, użytkownik będzie mógł pominąć to okno i od razu przejść do edytora bazy.

Edytor będzie zawierał kilka kart:

- **Semestr** - odczyt daty startu i końca semestru, tworzenie kopii semestru,
- **Sprzęt** - podgląd i edycja (operacje CRUD) sprzętu dostępnego na uczelni (nazwa, wybór
jednej z ikon reprezentujących typ, oraz opis, wszystko przedstawione w formie tabeli),
- **Cykliczne** - podgląd i edycja tabeli rezerwacji cotygodniowych,
- **Wypożyczenia** - widok tabeli rezerwacji w danym tygodniowy, z opcją dodania nowej rezerwacji
lub pominięcia wybranej rezerwacji cyklicznej w tym tygodniu,
- **Raporty** - zawiera przyciski do generowania raportów w formacie tekstowym (plan danego
tygodnia, wolny sprzęt w danym terminie, ranking sprzętu według dostępności).

Funkcja tworzenia kopii semestru będzie ułatwiać przygotowanie planu na następny semestr (umożliwi
skopiowanie całości danych lub samej karty Sprzęt). Podczas dodawania rezerwacji będą wyszukiwane
konflikty (zapobiegając wielu rezerwacjom tego samego sprzętu w tym samym terminie).
Dane będą wprowadzane przez okna dialogowe zawierające pola tekstowe, pola wyboru („combo box”)
oraz widżety wyboru daty.

\begin{wrapfigure}{r}{0.3\textwidth}
  \begin{center}
    \includegraphics[width=0.28\textwidth]{window.png}
  \end{center}
  \caption{Prototyp programu}
\end{wrapfigure}

Pliki bazy danych podzielone będą na sekcje:

1. nagłówek - początek semestru, rozmiary sekcji,
2. tablica `TableEquipment` - informacje o sprzęcie,
3. tablica `TablePeriodicReservation` - informacje o rezerwacjach cyklicznych,
4. tablica `TableOneTimeReservation` - informacje o rezerwacjach jednorazowych oraz pominięciach
rezerwacji cyklicznych,
5. tablica `TableStringMetadata` - sekcja przeznaczona na metadane opisów (tekstu o nieznanej,
nieograniczonej długości),
6. tablica `TableStringFragment` - sekcja przeznaczona na tekst o nieznanej, nieograniczonej
długości (będzie on dzielony na fragmenty po maksymalnie 255 znaków).

Nagłówek będzie trzymany w pamięci, z pozostałych części bazy dane będą wczytywane fragmentami
na żądanie. Zmiany będą zapisywane na dysk na bieżąco. Identyfikatorami elementów będą indeksy
tablic, w których te elementy się znajdują. Elementy tablic 2. - 4. będą zawierały odniesienia do
elementów tablicy 5., więc usuwanie tekstu będzie pozostawiać
w tablicy 5. metadane usuwanego elemencie (aby indeksy pozostałych elementów nie uległy
zmianie), usuwając jednak jego zawartość z tablicy 6. Daty będą reprezentowane przez 64-bitowy
znacznik czasu uniksowego, dni przez typ wyliczeniowy (`enum`), a pora dnia przez liczbę
minut, jakie upłynęły od godziny 0:00 tego dnia.

Projekt będzie podzielony na moduły (pliki `.glade`, `.h` i `.c`), każdy odpowiedzialny za
konkretną funkcjonalność (np. odczyt i zapis bazy), okno programu, lub fragment okna.
