/* See LICENSE file for copyright and license details. */

#pragma once

void drawhighlights(struct item *item, int x, int y, int w, int p, char *itemtext);
char* get_text_n_sgr(struct item *item);
int drawitemtext(struct item *item, int x, int y, int w);
int drawitem(int x, int y, int w);
int drawprompt(int x, int y, int w);
int drawinput(int x, int y, int w);
int drawlarrow(int x, int y, int w);
int drawrarrow(int x, int y, int w);
int drawnumber(int x, int y, int w);
int drawmode(int x, int y, int w);
int draw_caps_indicator(int x, int y, int w);
void draw_menu();
void draw_menu_layer();
#if IMAGE
int draw_icon(struct item *item, int x, int y, int w, int h);
#endif
