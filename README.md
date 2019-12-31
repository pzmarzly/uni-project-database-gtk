---
lang: pl
papersize: a4
---

# Projekt końcowy WdPwjC: WeźMnie, autor: Paweł Zmarzły

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
Podział "1 semestr = 1 plik" pozwoli na łatwe przygotowanie następnego semestru (kopiując poprzedni
lub kopiując szablon) oraz równoczesną pracę na wielu semestrach. Program będzie korzystał z GTK+
w wersji co najmniej 3.18 (zawarte w Ubuntu 16.04) oraz oczekiwał środowiska uniksowego (`/`
rozdziela katalogi, `/proc/self/exe` prowadzi do programu, itd.).

Po uruchomieniu programu pojawi się okno pozwalające na stworzenie nowej bazy danych
(nowego semestru) lub wczytanie już istniejącej. Podając ścieżkę do pliku jako parametr
wywołania programu, użytkownik będzie mógł pominąć to okno i od razu przejść do edytora bazy.

Edytor będzie zawierał kilka kart:

- Sprzęt - edycja sprzętu dostępnego na uczelni (nazwa, typ, opis),
- Cykliczne - edycja rezerwacji cotygodniowych, powodująca usunięcie wszystkich rezerwacji
jednorazowych (z powodu potencjalnych konfliktów),
- Wypożyczenia - widok tygodniowy, z opcją dodania nowej rezerwacji lub pominięcia wybranej
rezerwacji cyklicznej w danym tygodniu,
- Raporty - zawiera przyciski do generowania raportów (plan danego tygodnia, wolny sprzęt
w danym terminie, ranking sprzętu według dostępności).

Format bazy danych:

- nagłówek - początek semestru, rozmiary sekcji,
- rosnąca tablica TableEquipment - informacje o sprzęcie,
- rosnąca tablica TablePeriodicReservation - informacje o rezerwacjach cyklicznych,
- rosnąca tablica TableOneTimeReservation - informacje o rezerwacjach jednorazowych,
- rosnąca tablica TableStringFragment - sekcja przeznaczona na tekst o nieznanej, nieograniczonej
długości (będzie on dzielony na fragmenty po maksymalnie 255 znaków).

Nagłówek będzie trzymany w pamięci, z pozostałych części bazy dane będą wczytywane fragmentami
na żądanie. Zmiany będą zapisywane na dysk na bieżąco.
