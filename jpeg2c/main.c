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


static uint16_t *
img2u16(const Image *img)
{
	Rgba cp;
	int i, j, val;
	uint16_t pix, *buf = (uint16_t *)malloc(sizeof(uint16_t)*img->w*img->h);
	printf("static const uint16_t my_img[%ld][%ld] = {\n", img->h, img->w);
	for (i = 0; i < img->h; ++i) {
		printf("\t{");
		for (j = 0; j < img->w; ++j) {
			cp = img->d[i*img->w+j];
			val = (cp.r * 0x1f) / 0xff;
			pix = val << 11;
			val = (cp.g * 0x1f) / 0xff;
			pix |= val << 9;
			val = (cp.b * 0x1f) / 0xff;
			pix |= val;
			buf[i*img->w+j] = pix;
			printf("0x%04x", pix);
			if (j+1 == img->w)
				printf("}");
			else
				printf(", ");
		}
		if (i+1 == img->h)
			printf("\n}\n");
		else
			printf(",\n");
	}
	return buf;
}

int
main(int argc, char *argv[])
{
	Image *img;
	uint16_t *buf;

	if (argc < 2) {
		fprintf(stderr, "usage: %s image.jpg > image.c\n", argv[0]);
		return 1;
	}

	img = create_image();
	load_JPEG(img, argv[1]);
	buf = img2u16(img);
	free(buf);
	return 0;
}
