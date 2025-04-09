/******************************************************************************
 *                                                                            *
 * This file is part of Gargoyle.                                             *
 *                                                                            *
 * Gargoyle is free software; you can redistribute it and/or modify           *
 * it under the terms of the GNU General Public License as published by       *
 * the Free Software Foundation; either version 2 of the License, or          *
 * (at your option) any later version.                                        *
 *                                                                            *
 * Gargoyle is distributed in the hope that it will be useful,                *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of             *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the              *
 * GNU General Public License for more details.                               *
 *                                                                            *
 * You should have received a copy of the GNU General Public License          *
 * along with Gargoyle; if not, write to the Free Software                    *
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA *
 *                                                                            *
 *****************************************************************************/
#define _KINDLE
/* 
 * File:   gtk_utils.h
 */

#ifndef GTK_UTILS_H
#define GTK_UTILS_H

#include <stdbool.h>
#include <gtk/gtk.h>

#define KBFACTOR 2.909090909090909

#ifdef _KINDLE
/*
 * Returns the value of the first environment variable set or an empty string
 * otherwise. The caller is responsible to free the returned GString object.
 */

void closeLipcInstance();
void openVirtualKeyboard(GtkWidget * widget, gpointer * callback_data);
#endif

#endif /* GTK_UTILS_H */
