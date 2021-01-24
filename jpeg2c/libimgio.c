/**
 * Copyright 2018 Maksymilian Mruszczak <u at one u x dot o r g>
 *
 * You may do anything with this work that copyright law would normally
 * restrict, so long as you retain the above notice(s) and this license
 * in all redistributed copies and derived works.  There is no warranty.
 *
 *
 * Utility for loading [and saving] jpeg images and converting them
 * to float vector for use as a texture in GLSL or whatever
 */

#include <stdlib.h>
#include <stdio.h>
#include <jpeglib.h>
#include "imgiolib.h"

Image *
create_image(void)
{
	Image *img = (Image *)malloc(sizeof(Image));
	img->w = 0;
	img->h = 0;
	img->d = NULL;
	return img;
}

void
destroy_image(Image *img)
{
	free(img->d);
	free(img);
}

float *
image2fv(Image *img)
{
	size_t imgs, veci; /* image size, vector index */
	imgs = img->w*img->h;
	float *vec = (float *)malloc(sizeof(float)*imgs*4);
	veci = 0;
	for (int i = 0; i < imgs; ++i) {
		vec[veci++] = (float)img->d[i].r/255.f;
		vec[veci++] = (float)img->d[i].g/255.f;
		vec[veci++] = (float)img->d[i].b/255.f;
		if (img->d[i].r > 200 && img->d[i].g > 200 && img->d[i].b > 200)
			vec[veci++] = 0.f;
		else
			vec[veci++] = 1.f;
	}
	return vec;
}

int
load_JPEG(Image *img, char *filename)
{
	unsigned char *raw;
	size_t loc = 0;
	JSAMPROW *row_pointer;

	struct jpeg_decompress_struct pict;
	struct jpeg_error_mgr jerr;
	FILE *file = fopen(filename, "rb");
	if (!file) {
		fprintf(stderr, "Error while opening the file.\n");
		return -1;
	}

	/* init libjpeg struct */
	pict.err = jpeg_std_error(&jerr);
	jpeg_create_decompress(&pict);
	jpeg_stdio_src(&pict, file);
	jpeg_read_header(&pict, TRUE);

	/* read header */
	img->w = (size_t)pict.image_width;
	img->h = (size_t)pict.image_height;
	size_t imgSize = (img->w)*(img->h);
	jpeg_start_decompress(&pict);

	row_pointer = (*pict.mem->alloc_sarray)
	    ((j_common_ptr) &pict, JPOOL_IMAGE, img->w*3, 1);

	raw = (unsigned char *)malloc(pict.output_width*pict.output_height*
	    pict.num_components*sizeof(unsigned char));

	/* allocate row_pointer */
	row_pointer = (*pict.mem->alloc_sarray)((j_common_ptr)&pict,
	    JPOOL_IMAGE, pict.image_width*pict.num_components, 1);

	/* use row pointer to load bytes into buffer */
	while (pict.output_scanline < pict.image_height) {
		jpeg_read_scanlines(&pict, row_pointer, 1);
		for(int i=0; i < pict.image_width*pict.num_components; ++i)
			raw[loc++] = row_pointer[0][i];
	}
	jpeg_finish_decompress(&pict);
	jpeg_destroy_decompress(&pict);
	//free(row_pointer);
	fclose(file);

	/* wipe old buffer */
	if (img->d)
		free(img->d);

	img->d = (Rgba *)malloc(sizeof(Rgba)*imgSize*3);

	/* assign bytes to their respective channel */
	size_t raw_index = 0;
	for (size_t i=0; i < imgSize; ++i)
	{
		img->d[i].r = raw[raw_index++];
		img->d[i].g = raw[raw_index++];
		img->d[i].b = raw[raw_index++];
		img->d[i].a = 255;
	}
	free(raw);
	return 0;
}


int
save_JPEG(Image *img, char *filename, int quality)
{
	/* buffer size, row length, data_ index, buffer index */
	size_t bsize, rlength, di, bi;
	bsize = img->w * img->h * 3;
	rlength = img->w * 3;
	unsigned char *buf = (unsigned char *)malloc(bsize);
	di = 0;
	bi = 0;

	/* put data_ into simple byte array */
	while (bsize > bi) {
		buf[bi++] = img->d[di].r;
		buf[bi++] = img->d[di].g;
		buf[bi++] = img->d[di++].b;
	}

	/* initialize compression */
	struct jpeg_compress_struct pict;
	struct jpeg_error_mgr error_s;
	pict.err = jpeg_std_error(&error_s);
	jpeg_create_compress(&pict);

	/* initialize output file */
	FILE *outp;
	if (!(outp = fopen(filename, "wb")))
		return -1;
	jpeg_stdio_dest(&pict, outp);

	/* give info to image compressor */
	pict.input_components = 3;
	pict.image_width = img->w;
	pict.image_height = img->h;
	pict.in_color_space = JCS_RGB;
	jpeg_set_defaults(&pict);

	/* set quality */
	if (quality > 0)
		jpeg_set_quality(&pict, quality, TRUE);

	/* pointer to given row of pixels used by libjpeg library */
	JSAMPROW *rptr = (JSAMPROW *)malloc(sizeof(JSAMPROW));

	jpeg_start_compress(&pict, TRUE);

	/* compress and write data to file */
	while (pict.next_scanline < img->h) {
		rptr[0] = &buf[rlength*pict.next_scanline];
		jpeg_write_scanlines(&pict, rptr, 1);
	}

	/* cleanup */
	jpeg_finish_compress(&pict);
	fclose(outp);
	jpeg_destroy_compress(&pict);
	free(rptr);
	free(buf);

	return 0;
}
