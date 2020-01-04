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
w wersji co najmniej 3.18 (zawartej w Ubuntu 16.04) oraz oczekiwał środowiska uniksowego (`/`
rozdziela katalogi, `/proc/self/exe` prowadzi do programu, itd.).

Po uruchomieniu programu pojawi się okno pozwalające na stworzenie nowej bazy danych
(nowego semestru) lub wczytanie już istniejącej. Podając ścieżkę do pliku jako parametr
wywołania programu, użytkownik będzie mógł pominąć to okno i od razu przejść do edytora bazy.

Edytor będzie zawierał kilka kart:

- **Semestr** - wybór daty startu semestru, start i usunięcie semestru, tworzenie kopii bazy danych,
- **Sprzęt** - edycja sprzętu dostępnego na uczelni (nazwa, typ, opis),
- **Cykliczne** - edycja rezerwacji cotygodniowych,
- **Wypożyczenia** - widok tygodniowy, z opcją dodania nowej rezerwacji lub pominięcia wybranej
rezerwacji cyklicznej w danym tygodniu,
- **Raporty** - zawiera przyciski do generowania raportów w formacie tekstowym (plan danego tygodnia,
wolny sprzęt w danym terminie, ranking sprzętu według dostępności).

Przed startem semestru będzie można zmieniać datę startu semestru, dodawać i usuwać sprzęt,
a także dodawać i usuwać rezerwacje cykliczne. Po starcie semestru będzie można dodawać
sprzęt, dodawać i anulować rezerwacje cykliczne (zaczynając od danej daty), pomijać
rezerwacje cykliczne w danych dniach, a także dodawać i usuwać rezerwacje pojedyncze.

W przypadku awarii sprzętu dostępnego na uczelni użytkownik będzie mógł utworzyć rezerwację
na okres naprawy (np. na kilka dni).

Usunięcie semestru spowoduje usunięcie z bazy wszystkich jednorazowych rezerwacji, wszystkich
anulowanych cyklicznych rezerwacji oraz wszystkich pominięć cyklicznych rezerwacji. Ułatwi
to przygotowanie planu na następny semestr (użytkownik będzie mógł skopiować bazę obecnego
semestru, usunąć w niej semestr, zmienić datę rozpoczęcia semestru i rozpocząć nowy semestr,
nie musząc ręcznie przenosić danych z zakładek Sprzęt i Cykliczne między bazami).

Ponieważ polecenie mówi o prostym systemie rezerwacji, dane będą prezentowane w formie
tabelarycznych list. Rozważyłem użycie diagramów Gantta lub kalendarzy podobnych do planu zajęć w
[Systemie Zapisów II UWr](https://zapisy.ii.uni.wroc.pl/), ale odrzuciłem te pomysły ze względu
na złożoność implementacji i/lub ich wątpliwą ergonomiczną przewagę nad prostą listą rezerwacji.

Format bazy danych:

1. nagłówek - początek semestru, rozmiary sekcji,
2. rosnąca tablica TableEquipment - informacje o sprzęcie,
3. rosnąca tablica TablePeriodicReservation - informacje o rezerwacjach cyklicznych,
4. rosnąca tablica TableOneTimeReservation - informacje o rezerwacjach jednorazowych,
5. rosnąca tablica TableStringMetadata - sekcja przeznaczona na metadane tekstu,
6. rosnąca tablica TableStringFragment - sekcja przeznaczona na tekst o nieznanej, nieograniczonej
długości (będzie on dzielony na fragmenty po maksymalnie 255 znaków).

Nagłówek będzie trzymany w pamięci, z pozostałych części bazy dane będą wczytywane fragmentami
na żądanie. Zmiany będą zapisywane na dysk na bieżąco. Format bazy danych pozwoli na odczyt
danych w czasie $O(1)$, ich zapis w czasie $O(n)$ w zależności od całkowitego rozmiaru bazy,
a także ich usuwanie w czasie $O(n)$ w zależności od rozmiaru segmentu, w którym są dane.
Tablice 2. - 4. będą zawierały odniesienia do tablicy 5., więc usuwanie tekstu będzie pozostawiać
w tablicy 5. informacje o usuwanym elemencie (aby indeksy pozostałych elementów nie uległy
zmianie), usuwając jednak jego zawartość z tablicy 6.