#pragma once

void hexconv(const char *hex, unsigned short *r, unsigned short *g, unsigned short *b);
void create_window_x11(int x, int y, int w, int h);
void set_window_x11(void);
void set_prop_x11(void);
void resizeclient_x11(void);
void xinitvisual(void);
