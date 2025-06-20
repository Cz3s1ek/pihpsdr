/* Copyright (C)
* 2016 - John Melton, G0ORX/N6LYT
* 2025 - Christoph van Wüllen, DL1YCF
*
*   This program is free software: you can redistribute it and/or modify
*   it under the terms of the GNU General Public License as published by
*   the Free Software Foundation, either version 3 of the License, or
*   (at your option) any later version.
*
*   This program is distributed in the hope that it will be useful,
*   but WITHOUT ANY WARRANTY; without even the implied warranty of
*   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*   GNU General Public License for more details.
*
*   You should have received a copy of the GNU General Public License
*   along with this program.  If not, see <https://www.gnu.org/licenses/>.
*
*/

#include <gtk/gtk.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "band.h"
#include "bandstack.h"
#include "bandstack_menu.h"
#include "filter.h"
#include "new_menu.h"
#include "radio.h"
#include "receiver.h"
#include "vfo.h"

static GtkWidget *dialog = NULL;

struct _CHOICE {
  int info;
  GtkWidget      *button;
  gulong          signal;
  struct _CHOICE *next;
};

typedef struct _CHOICE CHOICE;

static int myvfo;

static struct _CHOICE *first = NULL;
static struct _CHOICE *current = NULL;

static void cleanup() {
  if (dialog != NULL) {
    GtkWidget *tmp = dialog;
    dialog = NULL;

    while (first != NULL) {
      CHOICE *choice = first;
      first = first->next;
      g_free(choice);
    }

    current = NULL;
    gtk_widget_destroy(tmp);
    sub_menu = NULL;
    active_menu  = NO_MENU;
    radio_save_state();
  }
}

static gboolean close_cb () {
  cleanup();
  return TRUE;
}

static gboolean bandstack_select_cb (GtkWidget *widget, gpointer data) {
  CHOICE *choice = (CHOICE *) data;

  if (current) {
    g_signal_handler_block(G_OBJECT(current->button), current->signal);
    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(current->button), current == choice);
    g_signal_handler_unblock(G_OBJECT(current->button), current->signal);
  }

  if (myvfo == 0 && current) {
    //
    // vfo_bandstack_changed() calls vfo_save_bandstack(), so the frequency/mode
    // of the previous "current" bandstack will be overwritten with the
    // current frequency/mode, which should be reflected by the button text.
    //
    char label[32];
    double f;

    if (vfo[0].ctun) {
      f = (double) vfo[0].ctun_frequency * 1E-6;
    } else {
      f = (double) vfo[0].frequency * 1E-6;
    }

    snprintf(label, sizeof(label), "%8.3f %s", f, mode_string[vfo[0].mode]);
    gtk_button_set_label(GTK_BUTTON(current->button), label);
  }

  current = choice;
  vfo_id_bandstack_changed(myvfo, choice->info);
  return FALSE;
}

void bandstack_menu(GtkWidget *parent) {
  int i;
  dialog = gtk_dialog_new();
  gtk_window_set_transient_for(GTK_WINDOW(dialog), GTK_WINDOW(parent));
  char title[64];
  myvfo = active_receiver->id;
  snprintf(title, sizeof(title), "piHPSDR - Band Stack (VFO-%s)", myvfo == 0 ? "A" : "B");
  GtkWidget *headerbar = gtk_header_bar_new();
  gtk_window_set_titlebar(GTK_WINDOW(dialog), headerbar);
  gtk_header_bar_set_show_close_button(GTK_HEADER_BAR(headerbar), TRUE);
  gtk_header_bar_set_title(GTK_HEADER_BAR(headerbar), title);
  g_signal_connect (dialog, "delete_event", G_CALLBACK (close_cb), NULL);
  g_signal_connect (dialog, "destroy", G_CALLBACK (close_cb), NULL);
  GtkWidget *content = gtk_dialog_get_content_area(GTK_DIALOG(dialog));
  GtkWidget *grid = gtk_grid_new();
  gtk_grid_set_column_homogeneous(GTK_GRID(grid), TRUE);
  gtk_grid_set_row_homogeneous(GTK_GRID(grid), TRUE);
  gtk_grid_set_column_spacing (GTK_GRID(grid), 5);
  gtk_grid_set_row_spacing (GTK_GRID(grid), 5);
  GtkWidget *close_b = gtk_button_new_with_label("Close");
  gtk_widget_set_name(close_b, "close_button");
  g_signal_connect (close_b, "button-press-event", G_CALLBACK(close_cb), NULL);
  gtk_grid_attach(GTK_GRID(grid), close_b, 0, 0, 1, 1);
  BAND *band = band_get_band(vfo[myvfo].band);
  BANDSTACK *bandstack = band->bandstack;
  char label[32];
  int row = 1;
  int col = 0;

  for (i = 0; i < bandstack->entries; i++) {
    const BANDSTACK_ENTRY *entry = &bandstack->entry[i];
    double f;

    if (entry->ctun) {
      f = (double) entry->ctun_frequency * 1E-6;
    } else {
      f = (double) entry->frequency * 1E-6;
    }

    snprintf(label, sizeof(label), "%8.3f MHz %s", f, mode_string[entry->mode]);
    GtkWidget *w = gtk_toggle_button_new_with_label(label);
    gtk_widget_set_name(w, "small_toggle_button");
    gtk_widget_show(w);
    gtk_grid_attach(GTK_GRID(grid), w, col, row, 1, 1);
    CHOICE *choice = g_new(CHOICE, 1);
    choice->next = first;
    first = choice;
    choice->info = i;
    choice->button = w;

    if (i == vfo[myvfo].bandstack) {
      current = choice;
      gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(current->button), TRUE);
    }

    choice->signal = g_signal_connect(w, "toggled", G_CALLBACK(bandstack_select_cb), choice);
    col++;

    if (col >= 4) {
      col = 0;
      row++;
    }
  }

  gtk_container_add(GTK_CONTAINER(content), grid);
  sub_menu = dialog;
  gtk_widget_show_all(dialog);
}
