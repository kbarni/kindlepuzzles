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
/* 
 * File:   gtk_utils.c
 * Author: schoen
 * 
 * Created on September 17, 2017, 8:34 PM
 */

#include "gtk_utils.h"
#include <assert.h>
#include <strings.h>
#include <stdbool.h>
#include <stdlib.h>
#include <wchar.h>
#include <stdio.h>
#include <unistd.h>

#ifdef _KINDLE
#include <openlipc.h>

static LIPC * lipcInstance = 0;

void openLipcInstance() {
	if (lipcInstance == 0) {
		fwprintf(stderr, L"---->openLipcInstance()\n");
		lipcInstance = LipcOpen("net.fabiszewski.gargoyle");
	}
}

void closeLipcInstance() {
	if (lipcInstance != 0) {
		fwprintf(stderr, L"---->closeLipcInstance()\n");
		LipcClose(lipcInstance);
	}
}

void openVirtualKeyboard(GtkWidget * widget, gpointer * callback_data) {
    /* lipc-set-prop -s com.lab126.keyboard open net.fabiszewski.gargoyle:abc:0 */
    if (lipcInstance == 0) {
		openLipcInstance();
	}
	int isKeboardVisible = 0;
	LipcGetIntProperty(
                lipcInstance,
                "com.lab126.keyboard",
                "show", &isKeboardVisible);
	
	if (isKeboardVisible == 0) {
		fwprintf(stderr, L"---->openVirtualKeyboard\n");
    	LipcSetStringProperty(lipcInstance,
                          "com.lab126.keyboard",
                          "open",
                          "net.fabiszewski.gargoyle:abc:0");
    } 
    else {
    	fwprintf(stderr, L"---->openVirtualKeyboard - Keyboard already opened, doing noting.\n");
    }
}

#endif /* _KINDLE */
