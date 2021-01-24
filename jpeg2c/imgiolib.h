/**
 * Copyright 2018 Maksymilian Mruszczak <u at one u x dot o r g>
 *
 * You may do anything with this work that copyright law would normally
 * restrict, so long as you retain the above notice(s) and this license
 * in all redistributed copies and derived works.  There is no warranty.
 */

typedef struct {
	unsigned char r, g, b, a;
} Rgba;

typedef struct {
	size_t w, h;
	Rgba *d;
} Image;

Image * create_image(void);
void destroy_image(Image *);
float * image2fv(Image *);
int load_JPEG(Image *, char *);
int save_JPEG(Image *, char *, int);
