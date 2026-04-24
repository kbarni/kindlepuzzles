/*
 * launcher.c — GTK2 game launcher for Simon Tatham's Puzzle Collection.
 *
 * Usage: launcher <bindir>
 *   <bindir>  absolute path to the directory containing game binaries
 *
 * On game selection, the launcher replaces itself via execl() with the
 * chosen game binary. No temp files, no shell post-processing needed.
 *
 * Kindle window title follows the Awesome WM key-value format:
 *   L:A  — APP layer
 *   N:application — window role
 *   PC:N — no chrome bars (full screen)
 *   ID:  — unique app identifier
 */

#include <gtk/gtk.h>
#include <gdk-pixbuf/gdk-pixbuf.h>
#include <gdk-pixbuf/gdk-pixdata.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "launcher-pixbufs.h"

/* ------------------------------------------------------------------ */
/* Layout constants                                                     */
/* ------------------------------------------------------------------ */
#define COLS       3
#define THUMB_SIZE 120

/* ------------------------------------------------------------------ */
/* Game table                                                           */
/* ------------------------------------------------------------------ */
typedef struct {
    const char      *binary;
    const char      *name;
    const char      *description;
    const GdkPixdata *pixdata;
} Game;

static const Game games[] = {
    { "blackbox",  "Black Box",      "Find the hidden balls in the box by bouncing laser beams off them.",          &pixbuf_blackbox  },
    { "bridges",   "Bridges",        "Connect all the islands with a network of bridges.",                          &pixbuf_bridges   },
    { "cube",      "Cube",           "Pick up all the blue squares by rolling the cube over them.",                 &pixbuf_cube      },
    { "dominosa",  "Dominosa",       "Tile the rectangle with a full set of dominoes.",                             &pixbuf_dominosa  },
    { "fifteen",   "Fifteen",        "Slide the tiles around to arrange them into order.",                          &pixbuf_fifteen   },
    { "flip",      "Flip",           "Flip groups of squares to light them all up at once.",                        &pixbuf_flip      },
    { "galaxies",  "Galaxies",       "Divide the grid into rotationally symmetric regions each centred on a dot.",  &pixbuf_galaxies  },
    { "inertia",   "Inertia",        "Collect all the gems without running into any of the mines.",                 &pixbuf_inertia   },
    { "lightup",   "Light Up",       "Place bulbs to light up all the squares.",                                    &pixbuf_lightup   },
    { "loopy",     "Loopy",          "Draw a single closed loop, given clues about number of adjacent edges.",      &pixbuf_loopy     },
    { "map",       "Map (4 color)",  "Colour the map so that adjacent regions are never the same colour.",          &pixbuf_map       },
    { "net",       "Net",            "Rotate each tile to reassemble the network.",                                 &pixbuf_net       },
    { "netslide",  "Netslide",       "Slide a row at a time to reassemble the network.",                            &pixbuf_netslide  },
    { "pegs",      "Pegs",           "Jump pegs over each other to remove all but one.",                            &pixbuf_pegs      },
    { "signpost",  "Signpost",       "Connect the squares into a path following the arrows.",                       &pixbuf_signpost  },
    { "singles",   "Singles",        "Black out the right set of duplicate numbers.",                               &pixbuf_singles   },
    { "sixteen",   "Sixteen",        "Slide a row at a time to arrange the tiles into order.",                      &pixbuf_sixteen   },
    { "slant",     "Slant",          "Draw a maze of slanting lines that matches the clues.",                       &pixbuf_slant     },
    { "tents",     "Tents",          "Place a tent next to each tree.",                                             &pixbuf_tents     },
    { "twiddle",   "Twiddle",        "Rotate the tiles around themselves to arrange them into order.",              &pixbuf_twiddle   },
    { "unruly",    "Unruly",         "Fill in the black and white grid to avoid runs of three.",                    &pixbuf_unruly    },
    { "untangle",  "Untangle",       "Reposition the points so that the lines do not cross.",                       &pixbuf_untangle  },
};

#define NUM_GAMES ((int)(sizeof(games) / sizeof(games[0])))

/* ------------------------------------------------------------------ */
/* State                                                                */
/* ------------------------------------------------------------------ */
static const char *selected_binary = NULL;
static char        bindir[512];

/* ------------------------------------------------------------------ */
/* Callbacks                                                            */
/* ------------------------------------------------------------------ */
static void on_game_clicked(GtkWidget *widget, gpointer data)
{
    (void)widget;
    selected_binary = (const char *)data;
    gtk_main_quit();
}

/* ------------------------------------------------------------------ */
/* Widget builders                                                      */
/* ------------------------------------------------------------------ */
static GtkWidget *make_game_cell(const Game *g)
{
    GtkWidget *cell = gtk_vbox_new(FALSE, 2);

    /* image-only button */
    GtkWidget *btn = gtk_button_new();
    gtk_button_set_relief(GTK_BUTTON(btn), GTK_RELIEF_NONE);
    g_signal_connect(btn, "clicked", G_CALLBACK(on_game_clicked),
                     (gpointer)g->binary);

    GdkPixbuf *raw    = gdk_pixbuf_from_pixdata(g->pixdata, FALSE, NULL);
    GdkPixbuf *scaled = gdk_pixbuf_scale_simple(raw, THUMB_SIZE, THUMB_SIZE,
                                                 GDK_INTERP_BILINEAR);
    g_object_unref(raw);
    GtkWidget *img = gtk_image_new_from_pixbuf(scaled);
    g_object_unref(scaled);
    gtk_container_add(GTK_CONTAINER(btn), img);
    gtk_box_pack_start(GTK_BOX(cell), btn, FALSE, FALSE, 0);

    /* bold game name */
    GtkWidget *name_lbl = gtk_label_new(NULL);
    char *markup = g_markup_printf_escaped("<b>%s</b>", g->name);
    gtk_label_set_markup(GTK_LABEL(name_lbl), markup);
    g_free(markup);
    gtk_box_pack_start(GTK_BOX(cell), name_lbl, FALSE, FALSE, 0);

    /* description — small, word-wrapped, fills the full cell width */
    GtkWidget *desc_lbl = gtk_label_new(g->description);
    gtk_label_set_line_wrap(GTK_LABEL(desc_lbl), TRUE);
    gtk_label_set_line_wrap_mode(GTK_LABEL(desc_lbl), PANGO_WRAP_WORD_CHAR);
    gtk_misc_set_alignment(GTK_MISC(desc_lbl), 0.0f, 0.0f);
    /* GTK2 won't infer wrap width from the table cell; compute it explicitly */
    int col_w = gdk_screen_get_width(gdk_screen_get_default()) / COLS - 20;
    gtk_widget_set_size_request(desc_lbl, col_w, -1);
    PangoAttrList *al = pango_attr_list_new();
    pango_attr_list_insert(al, pango_attr_scale_new(PANGO_SCALE_SMALL));
    gtk_label_set_attributes(GTK_LABEL(desc_lbl), al);
    pango_attr_list_unref(al);
    gtk_box_pack_start(GTK_BOX(cell), desc_lbl, TRUE, TRUE, 0);

    return cell;
}

static GtkWidget *make_grid(void)
{
    int rows = (NUM_GAMES + COLS - 1) / COLS;
    GtkWidget *table = gtk_table_new(rows, COLS, TRUE);
    gtk_table_set_row_spacings(GTK_TABLE(table), 4);
    gtk_table_set_col_spacings(GTK_TABLE(table), 4);
    gtk_container_set_border_width(GTK_CONTAINER(table), 6);

    for (int i = 0; i < NUM_GAMES; i++) {
        GtkWidget *btn = make_game_cell(&games[i]);
        gtk_table_attach_defaults(GTK_TABLE(table), btn,
                                  i % COLS, i % COLS + 1,
                                  i / COLS, i / COLS + 1);
    }
    return table;
}

/* ------------------------------------------------------------------ */
/* Main                                                                 */
/* ------------------------------------------------------------------ */
int main(int argc, char *argv[])
{
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <bindir>\n", argv[0]);
        return 1;
    }
    snprintf(bindir, sizeof(bindir), "%s", argv[1]);

    gtk_init(&argc, &argv);

    /* window */
    GtkWidget *window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(window),
        "L:A_N:application_ID:com.kbarni.puzzles_PC:N");
    gtk_window_fullscreen(GTK_WINDOW(window));
    g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);

    /* white background */
    GdkColor white;
    gdk_color_parse("white", &white);
    gtk_widget_modify_bg(window, GTK_STATE_NORMAL, &white);

    /* root layout */
    GtkWidget *vbox = gtk_vbox_new(FALSE, 0);
    gtk_container_add(GTK_CONTAINER(window), vbox);

    /* header: title centred, close button on the right */
    GtkWidget *header_hbox = gtk_hbox_new(FALSE, 0);

    GtkWidget *close_btn = gtk_button_new_with_label("  X  ");
    g_signal_connect(close_btn, "clicked", G_CALLBACK(gtk_main_quit), NULL);
    GtkWidget *left_pad = gtk_label_new("       "); /* symmetric spacer */

    GtkWidget *header = gtk_label_new(NULL);
    gtk_label_set_markup(GTK_LABEL(header),
        "<span size=\"x-large\"><b>Simon Tatham's\nPuzzle Collection for <i>Kindle</i></b></span>");
    gtk_misc_set_padding(GTK_MISC(header), 0, 12);

    gtk_box_pack_start(GTK_BOX(header_hbox), left_pad,   FALSE, FALSE, 4);
    gtk_box_pack_start(GTK_BOX(header_hbox), header,     TRUE,  TRUE,  0);
    gtk_box_pack_end  (GTK_BOX(header_hbox), close_btn,  FALSE, FALSE, 6);

    gtk_box_pack_start(GTK_BOX(vbox), header_hbox, FALSE, FALSE, 0);

    /* scrolled game grid */
    GtkWidget *scroll = gtk_scrolled_window_new(NULL, NULL);
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scroll),
                                   GTK_POLICY_NEVER, GTK_POLICY_AUTOMATIC);
    gtk_box_pack_start(GTK_BOX(vbox), scroll, TRUE, TRUE, 0);
    gtk_scrolled_window_add_with_viewport(GTK_SCROLLED_WINDOW(scroll),
                                          make_grid());

    gtk_widget_show_all(window);

    /* apply white background to the viewport inside the scrolled window */
    GtkWidget *viewport = gtk_bin_get_child(GTK_BIN(scroll));
    gtk_widget_modify_bg(viewport, GTK_STATE_NORMAL, &white);

    gtk_main();

    /* replace this process with the selected game */
    if (selected_binary) {
        char path[512];
        snprintf(path, sizeof(path), "%s/%s", bindir, selected_binary);
        execl(path, path, NULL);
        perror("execl");
        return 1;
    }
    return 0;
}
