/**
 * 2021, Maksymilian Mruszczak <u at one u x dot o r g>
 *
 * Convert jpeg to C array declaration suitable for embedding img
 * for usage with external LED display
 */

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include "imgiolib.h"


/**
 * prints img to stdin in SSD1331 format
 */
static void
img2u16(const Image *img, const char *name)
{
	Rgba cp;
	int i, j, val;
	uint16_t pix;
	printf("\n/* %ldx%ld */\nstatic const uint16_t sprite_%s_data[] = {\n", img->w, img->h, name);
	for (i = 0; i < img->h; ++i) {
		printf("\t");
		for (j = 0; j < img->w; ++j) {
			cp = img->d[i*img->w+j];
			val = (cp.r * 0x1f) / 0xff;
			pix = val << 11;
			val = (cp.g * 0x1f) / 0xff;
			pix |= val << 9;
			val = (cp.b * 0x1f) / 0xff;
			pix |= val;
			printf("0x%04x", pix);
			if (j+1 != img->w)
				printf(", ");
		}
		if (i+1 == img->h)
			printf("\n};\n");
		else
			printf(",\n");
	}
}

int
main(int argc, char *argv[])
{
	Image *img;

	if (argc < 3) {
		fprintf(stderr, "usage: %s image.jpg my_sprite > image.c\n", argv[0]);
		return 1;
	}

	img = create_image();
	load_JPEG(img, argv[1]);
	img2u16(img, argv[2]);
	return 0;
}
