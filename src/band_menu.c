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
#include "band_menu.h"
#include "client_server.h"
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

static struct _CHOICE *first = NULL;
static struct _CHOICE *current = NULL;

static int myvfo;

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

gboolean band_select_cb (GtkWidget *widget, gpointer data) {
  CHOICE *choice = (CHOICE *) data;
  int newband;

  //
  // If the current band has been clicked, this will cycle through the
  // band stack
  //
  if (radio_is_remote) {
    send_band(client_socket, myvfo, choice->info);
    // We have to assume that the band change succeeded, we just cannot know.
    newband = choice->info;
  } else {
    vfo_id_band_changed(myvfo, choice->info);
    newband = vfo[myvfo].band;
  }

  if (newband != choice->info) {
    //
    // Note that a band change may fail. This can happen if the local oscillator
    // of a transverter band has a grossly wrong frequency such that the effective
    // radio frequency falls outside the hardware frequency range of the radio.
    // In this case, the band button to the band that is effective must be highlighted
    // and all others turned off.
    //
    //
    choice = first;
    current = NULL;

    while (choice) {
      g_signal_handler_block(G_OBJECT(choice->button), choice->signal);

      if (choice->info == newband) {
        gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(choice->button), TRUE);
        current = choice;
      } else {
        gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(choice->button), FALSE);
      }

      g_signal_handler_unblock(G_OBJECT(choice->button), choice->signal);
      choice = choice->next;
    }

    return FALSE;
  }

  if (current) {
    g_signal_handler_block(G_OBJECT(current->button), current->signal);
    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(current->button), current == choice);
    g_signal_handler_unblock(G_OBJECT(current->button), current->signal);
  }

  current = choice;
  return FALSE;
}

void band_menu(GtkWidget *parent) {
  int i, j;
  dialog = gtk_dialog_new();
  gtk_window_set_transient_for(GTK_WINDOW(dialog), GTK_WINDOW(parent));
  char title[64];
  myvfo = active_receiver->id;
  snprintf(title, sizeof(title), "piHPSDR - Band (VFO-%s)", myvfo == 0 ? "A" : "B");
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
  gtk_widget_set_size_request(close_b, 150, 0);
  gtk_grid_attach(GTK_GRID(grid), close_b, 0, 0, 2, 1);
  long long frequency_min = radio->frequency_min;
  long long frequency_max = radio->frequency_max;
  j = 0;

  for (i = 0; i < BANDS + XVTRS; i++) {
    const BAND *band;
    band = (BAND*)band_get_band(i);

    if (strlen(band->title) > 0) {
      if (i < BANDS) {
        if (!(band->frequencyMin == 0.0 && band->frequencyMax == 0.0)) {
          if (band->frequencyMin < frequency_min || band->frequencyMax > frequency_max) {
            continue;
          }
        }
      }

      GtkWidget *w = gtk_toggle_button_new_with_label(band->title);
      gtk_widget_set_name(w, "small_toggle_button");
      gtk_widget_show(w);
      gtk_grid_attach(GTK_GRID(grid), w, j % 5, 1 + (j / 5), 1, 1);
      CHOICE *choice = g_new(CHOICE, 1);
      choice->next = first;
      first = choice;
      choice->info = i;
      choice->button = w;

      if (i == vfo[myvfo].band) {
        current = choice;
        gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(w), TRUE);
      }

      choice->signal = g_signal_connect(w, "toggled", G_CALLBACK(band_select_cb), choice);
      j++;
    }
  }

  gtk_container_add(GTK_CONTAINER(content), grid);
  sub_menu = dialog;
  gtk_widget_show_all(dialog);
}
