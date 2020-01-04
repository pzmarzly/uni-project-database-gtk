// Paweł Zmarzły 314569
// Lista 10 zadanie 1, grupa MPi śr 12-14

#include <gtk/gtk.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
  GtkWidget ***labels;
  GtkWidget *button;
  bool restore;

  int n, size, expected;
  int *tab, *restore_tab;
  bool *used;

  int cur_row_sum;
  int *cur_col_sums;
} state;

state state_default() {
  state s = {
      NULL,           // labels
      NULL,           // button
      false,          // restore
      0,     0,    0, // n, size, expected
      NULL,  NULL,    // tab, restore_tab
      NULL,           // used
      0,              // cur_row_sum
      NULL,           // cur_col_sums
  };
  return s;
}

bool test_diagonals(state *s) {
  int sum = 0;
  for (int d = 0; d < s->n; d++) {
    sum += s->tab[d * s->n + d];
  }
  if (sum != s->expected)
    return false;
  sum = 0;
  for (int d = 0; d < s->n; d++) {
    sum += s->tab[(s->n - d - 1) * s->n + d];
  }
  if (sum != s->expected)
    return false;
  return true;
}

int test(state *s, int field, int value) {
  // używam wolniejszej strategii, która pozwala
  // jednak na łatwiejszy powrót do poprzedniej
  // pozycji
  if (s->restore) {
    if (s->tab[field] > value)
      return 1;
  }

  // szybkie odrzucanie
  if (value <= 0)
    return 1;
  if (value > s->size)
    return 1;
  if (s->used[value - 1])
    return 1;

  // próbujemy użyć liczby
  s->tab[field] = value;
  s->used[value - 1] = true;

  int row = field / s->n;
  int col = field - row * s->n;

  int orig_cur_row_sum = s->cur_row_sum;
  if (col == 0)
    s->cur_row_sum = value;
  else
    s->cur_row_sum += value;
  s->cur_col_sums[col] += value;

  // odrzucanie
  if (s->cur_row_sum > s->expected)
    goto end;
  if (s->cur_col_sums[col] > s->expected)
    goto end;

  // tablica jest pełna
  if (field == s->size - 1) {
    if (test_diagonals(s)) {
      if (s->restore) {
        // jesteśmy tam gdzie ostatnio
        s->restore = false;
      } else {
        s->cur_row_sum = 0;
        memset(s->cur_col_sums, 0, sizeof(int) * s->n);
        memset(s->used, 0, sizeof(bool) * s->size);
        memcpy(s->restore_tab, s->tab, sizeof(int) * s->size);
        s->restore = true;
        return 0;
      }
    }
    goto end;
  }

  // niepełna tablica - szukamy następnego elementu
  // obliczamy jego współrzędne i sumę pozostałych elementów
  int row_next = (field + 1) / s->n;
  int col_next = field + 1 - row_next * s->n;
  int remaining = s->expected - s->cur_row_sum;
  if (col_next == 0)
    remaining = s->expected;
  // nie musimy zgadywać - ostatni wiersz
  if (row_next == s->n - 1) {
    if (test(s, field + 1, s->expected - s->cur_col_sums[col_next]) == 0)
      return 0;
    goto end;
  }
  // nie musimy zgadywać - ostatnia kolumna
  if (col_next == s->n - 1) {
    if (test(s, field + 1, remaining) == 0)
      return 0;
    goto end;
  }
  // zgadujemy
  int max = remaining - 1; // -1 bo zostały jeszcze 2 el. lub więcej
  for (int i = 0; i < max; i++) {
    if (test(s, field + 1, i + 1) == 0)
      return 0;
  }
end:
  // element został odrzucony, cofamy zmiany z sekcji "próbujemy użyć liczby"
  s->tab[field] = 0;
  s->used[value - 1] = false;
  s->cur_row_sum = orig_cur_row_sum;
  s->cur_col_sums[col] -= value;
  return 1;
}

static void next(GtkWidget *widget, gpointer user_data) {
  state *s = user_data;
  bool found = false;
  for (int i = 0; i < s->size; i++) {
    if (test(s, 0, i + 1) == 0) {
      found = true;
      break;
    }
  }

  if (found) {
    gtk_button_set_label(GTK_BUTTON(widget), "next");
  } else {
    gtk_button_set_label(GTK_BUTTON(widget), "restart");
  }

  char o[256];
  for (int i = 0; i < s->n; i++) {
    for (int j = 0; j < s->n; j++) {
      if (found) {
        sprintf(o, "%d", s->tab[i * s->n + j]);
      } else {
        o[0] = '-';
        o[1] = '\0';
      }
      gtk_label_set_text(GTK_LABEL(s->labels[i][j]), o);
    }
  }
}

int main(int argc, char *argv[]) {
  gtk_init(&argc, &argv);

  GtkWidget *window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
  gtk_window_set_title(GTK_WINDOW(window), "Kwadraty magiczne");
  gtk_window_set_position(GTK_WINDOW(window), GTK_WIN_POS_CENTER);
  gtk_container_set_border_width(GTK_CONTAINER(window), 10);
  g_signal_connect(G_OBJECT(window), "destroy", G_CALLBACK(gtk_main_quit),
                   NULL);

  if (argc != 2) {
    char str[] = "Błąd: nie podano parametru N (wielkość boku).";
    GtkWidget *dialog = gtk_message_dialog_new(
        GTK_WINDOW(window), GTK_DIALOG_DESTROY_WITH_PARENT, GTK_MESSAGE_ERROR,
        GTK_BUTTONS_CLOSE, "%s", str);
    gtk_dialog_run(GTK_DIALOG(dialog));
    gtk_widget_destroy(dialog);
    return 1;
  }

  state s = state_default();
  s.n = atoi(argv[1]);
  s.size = s.n * s.n;
  s.expected = s.n * (s.n * s.n + 1) / 2;
  s.cur_col_sums = calloc(sizeof(int), s.n);
  s.used = calloc(sizeof(bool), s.size);
  s.tab = calloc(sizeof(int), s.size);
  s.restore_tab = calloc(sizeof(int), s.size);

  GtkCssProvider *css = gtk_css_provider_new();
  char css_data[] =
      // starsze wersje GTK+ (np. ta używana w Ubuntu 16.04 oczekują
      // selektora .frame, nowsze frame
      "frame, .frame { border: none; background: #000; color: #fff; }";
  gtk_css_provider_load_from_data(css, css_data, strlen(css_data), NULL);
  GdkScreen *screen = gtk_widget_get_screen(window);
  gtk_style_context_add_provider_for_screen(
      screen, GTK_STYLE_PROVIDER(css), GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);

  GtkWidget *box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
  gtk_container_add(GTK_CONTAINER(window), box);

  s.button = gtk_button_new_with_label("start");
  g_signal_connect(G_OBJECT(s.button), "clicked", G_CALLBACK(next), &s);

  gtk_box_pack_end(GTK_BOX(box), s.button, TRUE, TRUE, 0);

  GtkWidget *grid = gtk_grid_new();
  gtk_grid_set_row_spacing(GTK_GRID(grid), 5);
  gtk_grid_set_row_homogeneous(GTK_GRID(grid), TRUE);
  gtk_grid_set_column_spacing(GTK_GRID(grid), 5);
  gtk_grid_set_column_homogeneous(GTK_GRID(grid), TRUE);
  gtk_box_pack_start(GTK_BOX(box), grid, TRUE, TRUE, 0);

  s.labels = malloc(s.n * sizeof(GtkWidget **));
  for (int i = 0; i < s.n; i++)
    s.labels[i] = malloc(s.n * sizeof(GtkWidget *));

  for (int i = 0; i < s.n; i++) {
    for (int j = 0; j < s.n; j++) {
      s.labels[i][j] = gtk_label_new("-");
      gtk_widget_set_size_request(s.labels[i][j], 35, 35);
      GtkWidget *frame = gtk_frame_new(NULL);
      gtk_container_add(GTK_CONTAINER(frame), s.labels[i][j]);
      gtk_grid_attach(GTK_GRID(grid), frame, i, j, 1, 1);
    }
  }

  gtk_widget_show_all(window);
  gtk_main();

  return 0;
}
