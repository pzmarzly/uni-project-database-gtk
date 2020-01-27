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

Zawarty w pliku [initial-project-description.md](initial-project-description.md).

## Poradnik użytkowania

Zawarty w pliku [user-guide.md](user-guide.md).

## Poradnik rozwoju programu

Zawarty w pliku [contributing.md](contributing.md).

## Budowanie

Wymagania: GTK+ 3.18 wzwyż, GNU `make`, GNU `gcc`, `pkg-config`.

Dla budowania plików `.pdf` także: `pandoc`, `texlive-xelatex` i `texlive-latex-recommended`.

Aby zbudować projekt, uruchom `make`. Program będzie można uruchomić za pomocą `./wez-mnie-gtk`,
lub zainstalować do wybranego folderu używając `make install DESTDIR=folder` (jeśli `DESTDIR`
nie zostanie podany, użyty zostanie katalog `./_install`).
