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

Zawarty w pliku [DESIGN.md](./DESIGN.md).

## Budowanie

Wymagania: GTK+ 3.18 wzwyż, GNU `make`, GNU `gcc`, `pkg-config`.

Dla budowania plików `.pdf` także: `pandoc`, `xelatex`.

Do utrzymania stylu kodu użyty został `clang-format`.

```bash
make # buduje główny program `wez-mnie-gtk` i bazę `demo.db`
./wez-mnie-gtk # uruchamia program
make fmt # uruchamia `clang-format`
make all # równoważne `make wez-mnie-gtk test docs`
make clean # usuwa pliki utworzone przez `make all`

make test # buduje program test
./test # próbuje wykonać wszystkie testy

# buduje pliki `.pdf` na podstawie plików `.md`, używając `pandoc` i `xelatex`
make docs

# kopiuje `wez-mnie-gtk` i używane przez niego pliki
# do `DESTDIR` (domyślnie `./_install`)
make install DESTDIR=./_install
_install/wez-mnie-gtk # uruchamia program
```

## Struktura projektu

```text
programy:
- wez-mnie-gtk - główny program
- test - program używany do szukania błędów w implementacji bazy
- gen-demo - program tworzący bazę z przykładowymi danymi `demo.db`
okna GTK+:
- AboutDialog
- Editor (i jego zakładki w EditorSemester, EditorEquipment, itd.)
- EditorEditDialog
- EditorRemovalDialog
- Greeter
pozostałe:
- Repo - implementacja wczytania/zapisania bazy, definicje tabel
- RepoString - konwersja między tekstem zapisanym w bazie a `char *`
- RepoData - funkcje operujące na typach zdefiniowanych w Repo
- Utils - funkcje pomocnicze
ikony:
- question-mark.png image-projector.png laptop.png:
  Icon made by Freepik from www.flaticon.com
- whiteboard.png:
  Icons made by Kiranshastry from www.flaticon.com
Licencja ikon znajduje się również w AboutDialog.
```
