# Poradnik rozwoju programu

Wypożyczenia są nazywane w kodzie programu rezerwacjami (`Reservation`).

## Struktura projektu

```text
programy:
- wez-mnie-gtk - główny program
- test - program używany do szukania błędów w implementacji bazy
- gen-demo - program tworzący bazę z przykładowymi danymi `demo.db`
okna GTK+:
- AboutDialog
- Editor (i jego zakładki w EditorSemester, EditorEquipment, itd.)
- dialog/Dialogs, EditorDialogs, EditorReportsDialogs - okna dialogowe
- Greeter
pozostałe:
- Datepicker - widget: przycisk z datą, otwierający kalendarz
- LinkedList - lista łączona jednostronnie i konwersja na tablicę
- RecentList - moduł pomocniczy dla Greeter, zarządzający listą ostatnio otwartych plików
- Repo - implementacja wczytania/zapisania bazy, definicje tabel
- RepoString - konwersja między tekstem zapisanym w bazie a `char *`
- RepoData - funkcje operujące na typach zdefiniowanych w Repo, oraz biblioteka czasu
- RepoGenerate - funkcje pomocnicze dla gen-demo i test
- RepoLogic - funkcje sprawdzające konflikty i generujące wystąpienia cyklicznych wypożyczeń
- Utils - różne funkcje pomocnicze, upraszczające korzystanie z GLib i GTK
ikony:
- question-mark.png image-projector.png laptop.png:
  Icon made by Freepik from www.flaticon.com
- whiteboard.png:
  Icons made by Kiranshastry from www.flaticon.com
Licencja ikon znajduje się również w AboutDialog.
```

## Konwencja nazewnicza

Funkcje prywatne - dowolne nazwy.

Funkcje publiczne związane z modułami posiadają przedrostek (np. `editor_` w `Editor.h`).

Funkcje publiczne pomocnicze (helper functions) nazywane dowolnie, ale jednoznacznie.

Wyjątkiem jest moduł `RepoLogic`, którego funkcje są nazywane tak, aby kod korzystający
z nich formułował zdania w języku angielskim w 1. okresie warunkowym
(np. *if the one time reservation conflicts with the periodic one* ->
`if (one_time_conflicts_with_periodic(...))`).
