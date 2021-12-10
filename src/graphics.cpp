#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <malloc.h>
#include <math.h>
#include <fcntl.h>

#include <jpeglib.h>
#include <time.h>
#include <png.h>

#include "include/graphics.h"

#define DEG2RAD(x) ((x)*0.01745329251)

static const u64 BLACK_RGBAQ   = GS_SETREG_RGBAQ(0x00,0x00,0x00,0x80,0x00);
static const u64 TEXTURE_RGBAQ = GS_SETREG_RGBAQ(0x80,0x80,0x80,0x80,0x00);

GSGLOBAL *gsGlobal = NULL;
GSFONTM *gsFontM = NULL;

#ifdef ftoi4
 #undef ftoi4
 #define ftoi4(F) ((int)(((float)F)*16.0f))
#else
 #define ftoi4(F) ((int)(((float)F)*16.0f))
#endif


//2D drawing functions
GSTEXTURE* luaP_loadpng(const char *path, bool delayed)
{
	GSTEXTURE* tex = (GSTEXTURE*)malloc(sizeof(GSTEXTURE));
	tex->Delayed = delayed;

	FILE* File = fopen(path, "rb");
	if (File == NULL)
	{
		printf("Failed to load PNG file: %s\n", path);
		return NULL;
	}

	png_structp png_ptr;
	png_infop info_ptr;
	png_uint_32 width, height;
	png_bytep *row_pointers;

	u32 sig_read = 0;
        int row, i, k=0, j, bit_depth, color_type, interlace_type;

	png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, (png_voidp) NULL, NULL, NULL);

	if(!png_ptr)
	{
		printf("PNG Read Struct Init Failed\n");
		fclose(File);
		return NULL;
	}

	info_ptr = png_create_info_struct(png_ptr);

	if(!info_ptr)
	{
		printf("PNG Info Struct Init Failed\n");
		fclose(File);
		png_destroy_read_struct(&png_ptr, (png_infopp)NULL, (png_infopp)NULL);
		return NULL;
	}

	if(setjmp(png_jmpbuf(png_ptr)))
	{
		printf("Got PNG Error!\n");
		png_destroy_read_struct(&png_ptr, &info_ptr, (png_infopp)NULL);
		fclose(File);
		return NULL;
	}

	png_init_io(png_ptr, File);

	png_set_sig_bytes(png_ptr, sig_read);

	png_read_info(png_ptr, info_ptr);

	png_get_IHDR(png_ptr, info_ptr, &width, &height, &bit_depth, &color_type,&interlace_type, NULL, NULL);

	png_set_strip_16(png_ptr);

	if (color_type == PNG_COLOR_TYPE_PALETTE)
		png_set_expand(png_ptr);

	if (color_type == PNG_COLOR_TYPE_GRAY && bit_depth < 8)
		png_set_expand(png_ptr);

	if (png_get_valid(png_ptr, info_ptr, PNG_INFO_tRNS))
		png_set_tRNS_to_alpha(png_ptr);

	png_set_filler(png_ptr, 0xff, PNG_FILLER_AFTER);

	png_read_update_info(png_ptr, info_ptr);

	tex->Width = width;
	tex->Height = height;

        tex->VramClut = 0;
        tex->Clut = NULL;

	if(png_get_color_type(png_ptr, info_ptr) == PNG_COLOR_TYPE_RGB_ALPHA)
	{
		int row_bytes = png_get_rowbytes(png_ptr, info_ptr);
		tex->PSM = GS_PSM_CT32;
		tex->Mem = (u32*)memalign(128, gsKit_texture_size_ee(tex->Width, tex->Height, tex->PSM));

		row_pointers = (png_byte**)calloc(height, sizeof(png_bytep));

		for (row = 0; row < height; row++) row_pointers[row] = (png_bytep)malloc(row_bytes);

		png_read_image(png_ptr, row_pointers);

		struct pixel { u8 r,g,b,a; };
		struct pixel *Pixels = (struct pixel *) tex->Mem;

		for (i = 0; i < tex->Height; i++) {
			for (j = 0; j < tex->Width; j++) {
				memcpy(&Pixels[k], &row_pointers[i][4 * j], 3);
				Pixels[k++].a = row_pointers[i][4 * j + 3] >> 1;
			}
		}

		for(row = 0; row < height; row++) free(row_pointers[row]);

		free(row_pointers);
	}
	else if(png_get_color_type(png_ptr, info_ptr) == PNG_COLOR_TYPE_RGB)
	{
		int row_bytes = png_get_rowbytes(png_ptr, info_ptr);
		tex->PSM = GS_PSM_CT24;
		tex->Mem = (u32*)memalign(128, gsKit_texture_size_ee(tex->Width, tex->Height, tex->PSM));

		row_pointers = (png_byte**)calloc(height, sizeof(png_bytep));

		for(row = 0; row < height; row++) row_pointers[row] = (png_bytep)malloc(row_bytes);

		png_read_image(png_ptr, row_pointers);

		struct pixel3 { u8 r,g,b; };
		struct pixel3 *Pixels = (struct pixel3 *) tex->Mem;

		for (i = 0; i < tex->Height; i++) {
			for (j = 0; j < tex->Width; j++) {
				memcpy(&Pixels[k++], &row_pointers[i][4 * j], 3);
			}
		}

		for(row = 0; row < height; row++) free(row_pointers[row]);

		free(row_pointers);
	}
	else
	{
		printf("This texture depth is not supported yet!\n");
		return NULL;
	}

	tex->Filter = GS_FILTER_NEAREST;
	png_read_end(png_ptr, NULL);
	png_destroy_read_struct(&png_ptr, &info_ptr, (png_infopp) NULL);
	fclose(File);

	if(!tex->Delayed)
	{
		tex->Vram = gsKit_vram_alloc(gsGlobal, gsKit_texture_size(tex->Width, tex->Height, tex->PSM), GSKIT_ALLOC_USERBUFFER);
		if(tex->Vram == GSKIT_ALLOC_ERROR)
		{
			printf("VRAM Allocation Failed. Will not upload texture.\n");
			return NULL;
		}

		if(tex->Clut != NULL)
		{
			if(tex->PSM == GS_PSM_T4)
				tex->VramClut = gsKit_vram_alloc(gsGlobal, gsKit_texture_size(8, 2, GS_PSM_CT32), GSKIT_ALLOC_USERBUFFER);
			else
				tex->VramClut = gsKit_vram_alloc(gsGlobal, gsKit_texture_size(16, 16, GS_PSM_CT32), GSKIT_ALLOC_USERBUFFER);

			if(tex->VramClut == GSKIT_ALLOC_ERROR)
			{
				printf("VRAM CLUT Allocation Failed. Will not upload texture.\n");
				return NULL;
			}
		}

		// Upload texture
		gsKit_texture_upload(gsGlobal, tex);
		// Free texture
		free(tex->Mem);
		tex->Mem = NULL;
		// Free texture CLUT
		if(tex->Clut != NULL)
		{
			free(tex->Clut);
			tex->Clut = NULL;
		}
	}
	else
	{
		gsKit_setup_tbw(tex);
	}

	return tex;

}


GSTEXTURE* luaP_loadbmp(const char *Path, bool delayed)
{
	GSBITMAP Bitmap;
	int x, y;
	int cy;
	u32 FTexSize;
	u8  *image;
	u8  *p;

    GSTEXTURE* tex = (GSTEXTURE*)malloc(sizeof(GSTEXTURE));
	tex->Delayed = delayed;

	FILE* File = fopen(Path, "rb");
	if (File == NULL)
	{
		printf("BMP: Failed to load bitmap: %s\n", Path);
		return NULL;
	}
	if (fread(&Bitmap.FileHeader, sizeof(Bitmap.FileHeader), 1, File) <= 0)
	{
		printf("BMP: Could not load bitmap: %s\n", Path);
		fclose(File);
		return NULL;
	}

	if (fread(&Bitmap.InfoHeader, sizeof(Bitmap.InfoHeader), 1, File) <= 0)
	{
		printf("BMP: Could not load bitmap: %s\n", Path);
		fclose(File);
		return NULL;
	}

	tex->Width = Bitmap.InfoHeader.Width;
	tex->Height = Bitmap.InfoHeader.Height;
	tex->Filter = GS_FILTER_NEAREST;

	if(Bitmap.InfoHeader.BitCount == 4)
	{
		tex->PSM = GS_PSM_T4;
		tex->Clut = (u32*)memalign(128, gsKit_texture_size_ee(8, 2, GS_PSM_CT32));
		tex->ClutPSM = GS_PSM_CT32;

		memset(tex->Clut, 0, gsKit_texture_size_ee(8, 2, GS_PSM_CT32));
		fseek(File, 54, SEEK_SET);
		if (fread(tex->Clut, Bitmap.InfoHeader.ColorUsed*sizeof(u32), 1, File) <= 0)
		{
			if (tex->Clut) {
				free(tex->Clut);
				tex->Clut = NULL;
			}
			printf("BMP: Could not load bitmap: %s\n", Path);
			fclose(File);
			return NULL;
		}

		GSBMCLUT *clut = (GSBMCLUT *)tex->Clut;
		int i;
		for (i = Bitmap.InfoHeader.ColorUsed; i < 16; i++)
		{
			memset(&clut[i], 0, sizeof(clut[i]));
		}

		for (i = 0; i < 16; i++)
		{
			u8 tmp = clut[i].Blue;
			clut[i].Blue = clut[i].Red;
			clut[i].Red = tmp;
			clut[i].Alpha = 0x80;
		}

	}
	else if(Bitmap.InfoHeader.BitCount == 8)
	{
		tex->PSM = GS_PSM_T8;
		tex->Clut = (u32*)memalign(128, gsKit_texture_size_ee(16, 16, GS_PSM_CT32));
		tex->ClutPSM = GS_PSM_CT32;

		memset(tex->Clut, 0, gsKit_texture_size_ee(16, 16, GS_PSM_CT32));
		fseek(File, 54, SEEK_SET);
		if (fread(tex->Clut, Bitmap.InfoHeader.ColorUsed*sizeof(u32), 1, File) <= 0)
		{
			if (tex->Clut) {
				free(tex->Clut);
				tex->Clut = NULL;
			}
			printf("BMP: Could not load bitmap: %s\n", Path);
			fclose(File);
			return NULL;
		}

		GSBMCLUT *clut = (GSBMCLUT *)tex->Clut;
		int i;
		for (i = Bitmap.InfoHeader.ColorUsed; i < 256; i++)
		{
			memset(&clut[i], 0, sizeof(clut[i]));
		}

		for (i = 0; i < 256; i++)
		{
			u8 tmp = clut[i].Blue;
			clut[i].Blue = clut[i].Red;
			clut[i].Red = tmp;
			clut[i].Alpha = 0x80;
		}

		// rotate clut
		for (i = 0; i < 256; i++)
		{
			if ((i&0x18) == 8)
			{
				GSBMCLUT tmp = clut[i];
				clut[i] = clut[i+8];
				clut[i+8] = tmp;
			}
		}
	}
	else if(Bitmap.InfoHeader.BitCount == 16)
	{
		tex->PSM = GS_PSM_CT16;
		tex->VramClut = 0;
		tex->Clut = NULL;
	}
	else if(Bitmap.InfoHeader.BitCount == 24)
	{
		tex->PSM = GS_PSM_CT24;
		tex->VramClut = 0;
		tex->Clut = NULL;
	}

	fseek(File, 0, SEEK_END);
	FTexSize = ftell(File);
	FTexSize -= Bitmap.FileHeader.Offset;

	fseek(File, Bitmap.FileHeader.Offset, SEEK_SET);

	u32 TextureSize = gsKit_texture_size_ee(tex->Width, tex->Height, tex->PSM);

	tex->Mem = (u32*)memalign(128,TextureSize);

	if(Bitmap.InfoHeader.BitCount == 24)
	{
		image = (u8*)memalign(128, FTexSize);
		if (image == NULL) {
			printf("BMP: Failed to allocate memory\n");
			if (tex->Mem) {
				free(tex->Mem);
				tex->Mem = NULL;
			}
			if (tex->Clut) {
				free(tex->Clut);
				tex->Clut = NULL;
			}
			fclose(File);
			return NULL;
		}

		fread(image, FTexSize, 1, File);
		p = (u8*)((u32)tex->Mem);
		for (y = tex->Height - 1, cy = 0; y >= 0; y--, cy++) {
			for (x = 0; x < tex->Width; x++) {
				p[(y * tex->Width + x) * 3 + 2] = image[(cy * tex->Width + x) * 3 + 0];
				p[(y * tex->Width + x) * 3 + 1] = image[(cy * tex->Width + x) * 3 + 1];
				p[(y * tex->Width + x) * 3 + 0] = image[(cy * tex->Width + x) * 3 + 2];
			}
		}
		free(image);
		image = NULL;
	}
	else if(Bitmap.InfoHeader.BitCount == 16)
	{
		image = (u8*)memalign(128, FTexSize);
		if (image == NULL) {
			printf("BMP: Failed to allocate memory\n");
			if (tex->Mem) {
				free(tex->Mem);
				tex->Mem = NULL;
			}
			if (tex->Clut) {
				free(tex->Clut);
				tex->Clut = NULL;
			}
			fclose(File);
			return NULL;
		}

		fread(image, FTexSize, 1, File);

		p = (u8*)((u32*)tex->Mem);
		for (y = tex->Height - 1, cy = 0; y >= 0; y--, cy++) {
			for (x = 0; x < tex->Width; x++) {
				u16 value;
				value = *(u16*)&image[(cy * tex->Width + x) * 2];
				value = (value & 0x8000) | value << 10 | (value & 0x3E0) | (value & 0x7C00) >> 10;	//ARGB -> ABGR

				*(u16*)&p[(y * tex->Width + x) * 2] = value;
			}
		}
		free(image);
		image = NULL;
	}
	else if(Bitmap.InfoHeader.BitCount == 8 || Bitmap.InfoHeader.BitCount == 4)
	{
		char *text = (char *)((u32)tex->Mem);
		image = (u8*)memalign(128,FTexSize);
		if (image == NULL) {
			printf("BMP: Failed to allocate memory\n");
			if (tex->Mem) {
				free(tex->Mem);
				tex->Mem = NULL;
			}
			if (tex->Clut) {
				free(tex->Clut);
				tex->Clut = NULL;
			}
			fclose(File);
			return NULL;
		}

		if (fread(image, FTexSize, 1, File) != 1)
		{
			if (tex->Mem) {
				free(tex->Mem);
				tex->Mem = NULL;
			}
			if (tex->Clut) {
				free(tex->Clut);
				tex->Clut = NULL;
			}
			printf("BMP: Read failed!, Size %d\n", FTexSize);
			free(image);
			image = NULL;
			fclose(File);
			return NULL;
		}
		for (y = tex->Height - 1; y >= 0; y--)
		{
			if(Bitmap.InfoHeader.BitCount == 8)
				memcpy(&text[y * tex->Width], &image[(tex->Height - y - 1) * tex->Width], tex->Width);
			else
				memcpy(&text[y * (tex->Width / 2)], &image[(tex->Height - y - 1) * (tex->Width / 2)], tex->Width / 2);
		}
		free(image);
		image = NULL;

		if(Bitmap.InfoHeader.BitCount == 4)
		{
			int byte;
			u8 *tmpdst = (u8 *)((u32)tex->Mem);
			u8 *tmpsrc = (u8 *)text;

			for(byte = 0; byte < FTexSize; byte++)
			{
				tmpdst[byte] = (tmpsrc[byte] << 4) | (tmpsrc[byte] >> 4);
			}
		}
	}
	else
	{
		printf("BMP: Unknown bit depth format %d\n", Bitmap.InfoHeader.BitCount);
	}

	fclose(File);

	if(!tex->Delayed)
	{
		tex->Vram = gsKit_vram_alloc(gsGlobal, gsKit_texture_size(tex->Width, tex->Height, tex->PSM), GSKIT_ALLOC_USERBUFFER);
		if(tex->Vram == GSKIT_ALLOC_ERROR)
		{
			printf("VRAM Allocation Failed. Will not upload texture.\n");
			return NULL;
		}

		if(tex->Clut != NULL)
		{
			if(tex->PSM == GS_PSM_T4)
				tex->VramClut = gsKit_vram_alloc(gsGlobal, gsKit_texture_size(8, 2, GS_PSM_CT32), GSKIT_ALLOC_USERBUFFER);
			else
				tex->VramClut = gsKit_vram_alloc(gsGlobal, gsKit_texture_size(16, 16, GS_PSM_CT32), GSKIT_ALLOC_USERBUFFER);

			if(tex->VramClut == GSKIT_ALLOC_ERROR)
			{
				printf("VRAM CLUT Allocation Failed. Will not upload texture.\n");
				return NULL;
			}
		}

		// Upload texture
		gsKit_texture_upload(gsGlobal, tex);
		// Free texture
		free(tex->Mem);
		tex->Mem = NULL;
		// Free texture CLUT
		if(tex->Clut != NULL)
		{
			free(tex->Clut);
			tex->Clut = NULL;
		}
	}
	else
	{
		gsKit_setup_tbw(tex);
	}

	return tex;

}

struct my_error_mgr {
  struct jpeg_error_mgr pub;    /* "public" fields */

  jmp_buf setjmp_buffer;        /* for return to caller */
};

typedef struct my_error_mgr *my_error_ptr;

METHODDEF(void)
my_error_exit(j_common_ptr cinfo)
{
  /* cinfo->err really points to a my_error_mgr struct, so coerce pointer */
  my_error_ptr myerr = (my_error_ptr)cinfo->err;

  /* Always display the message. */
  /* We could postpone this until after returning, if we chose. */
  (*cinfo->err->output_message) (cinfo);

  /* Return control to the setjmp point */
  longjmp(myerr->setjmp_buffer, 1);
}

// Following official documentation max width or height of the texture is 1024
#define MAX_TEXTURE 1024
static void  _ps2_load_JPEG_generic(GSTEXTURE *Texture, struct jpeg_decompress_struct *cinfo, struct my_error_mgr *jerr, bool scale_down)
{
	int textureSize = 0;
	if (scale_down) {
		unsigned int longer = cinfo->image_width > cinfo->image_height ? cinfo->image_width : cinfo->image_height;
		float downScale = (float)longer / (float)MAX_TEXTURE;
		cinfo->scale_denom = ceil(downScale);
	}

	jpeg_start_decompress(cinfo);

	int psm = cinfo->out_color_components == 3 ? GS_PSM_CT24 : GS_PSM_CT32;

	Texture->Width =  cinfo->output_width;
	Texture->Height = cinfo->output_height;
	Texture->PSM = psm;
	Texture->Filter = GS_FILTER_NEAREST;
	Texture->VramClut = 0;
	Texture->Clut = NULL;

	textureSize = cinfo->output_width*cinfo->output_height*cinfo->out_color_components;
	#ifdef DEBUG
	printf("Texture Size = %i\n",textureSize);
	#endif
	Texture->Mem = (u32*)memalign(128, textureSize);

	unsigned int row_stride = textureSize/Texture->Height;
	unsigned char *row_pointer = (unsigned char *)Texture->Mem;
	while (cinfo->output_scanline < cinfo->output_height) {
		jpeg_read_scanlines(cinfo, (JSAMPARRAY)&row_pointer, 1);
		row_pointer += row_stride;
	}

	jpeg_finish_decompress(cinfo);
}

GSTEXTURE* luaP_loadjpeg(const char *Path, bool scale_down, bool delayed)
{

	
    GSTEXTURE* tex = (GSTEXTURE*)malloc(sizeof(GSTEXTURE));
	tex->Delayed = delayed;

	FILE *fp;
	struct jpeg_decompress_struct cinfo;
	struct my_error_mgr jerr;

	if (tex == NULL) {
		printf("jpeg: error Texture is NULL\n");
		return NULL;
	}

	fp = fopen(Path, "rb");
	if (fp == NULL)
	{
		printf("jpeg: Failed to load file: %s\n", Path);
		return NULL;
	}

	/* We set up the normal JPEG error routines, then override error_exit. */
	cinfo.err = jpeg_std_error(&jerr.pub);
	jerr.pub.error_exit = my_error_exit;
	/* Establish the setjmp return context for my_error_exit to use. */
	if (setjmp(jerr.setjmp_buffer)) {
		/* If we get here, the JPEG code has signaled an error.
		* We need to clean up the JPEG object, close the input file, and return.
		*/
		jpeg_destroy_decompress(&cinfo);
		fclose(fp);
		if (tex->Mem)
			free(tex->Mem);
		printf("jpeg: error during processing file\n");
		return NULL;
	}
	jpeg_create_decompress(&cinfo);
	jpeg_stdio_src(&cinfo, fp);
	jpeg_read_header(&cinfo, TRUE);

	_ps2_load_JPEG_generic(tex, &cinfo, &jerr, scale_down);
	
	jpeg_destroy_decompress(&cinfo);
	fclose(fp);

	
	if(!tex->Delayed)
	{
		tex->Vram = gsKit_vram_alloc(gsGlobal, gsKit_texture_size(tex->Width, tex->Height, tex->PSM), GSKIT_ALLOC_USERBUFFER);
		if(tex->Vram == GSKIT_ALLOC_ERROR)
		{
			printf("VRAM Allocation Failed. Will not upload texture.\n");
			return NULL;
		}

		if(tex->Clut != NULL)
		{
			if(tex->PSM == GS_PSM_T4)
				tex->VramClut = gsKit_vram_alloc(gsGlobal, gsKit_texture_size(8, 2, GS_PSM_CT32), GSKIT_ALLOC_USERBUFFER);
			else
				tex->VramClut = gsKit_vram_alloc(gsGlobal, gsKit_texture_size(16, 16, GS_PSM_CT32), GSKIT_ALLOC_USERBUFFER);

			if(tex->VramClut == GSKIT_ALLOC_ERROR)
			{
				printf("VRAM CLUT Allocation Failed. Will not upload texture.\n");
				return NULL;
			}
		}

		// Upload texture
		gsKit_texture_upload(gsGlobal, tex);
		// Free texture
		free(tex->Mem);
		tex->Mem = NULL;
		// Free texture CLUT
		if(tex->Clut != NULL)
		{
			free(tex->Clut);
			tex->Clut = NULL;
		}
	}
	else
	{
		gsKit_setup_tbw(tex);
	}

	return tex;

}


void gsKit_clear_screens()
{
	int i;

	for (i=0; i<2; i++)
	{
		gsKit_clear(gsGlobal, BLACK_RGBAQ);
		gsKit_queue_exec(gsGlobal);
		gsKit_sync_flip(gsGlobal);
	}
}

void clearScreen(Color color)
{
	gsKit_clear(gsGlobal, color);
	
}

void loadFontM()
{
	gsFontM = gsKit_init_fontm();
	gsKit_fontm_upload(gsGlobal, gsFontM);
	gsFontM->Spacing = 0.70f;
}

void printFontMText(char* text, float x, float y, float scale, Color color)
{
	gsKit_set_test(gsGlobal, GS_ATEST_ON);
	gsKit_fontm_print_scaled(gsGlobal, gsFontM, x, y, 1, scale, color, text);
}

void unloadFontM()
{
	gsKit_free_fontm(gsGlobal, gsFontM);
}

int FPSCounter(clock_t prevtime, clock_t curtime)
{
	float fps = 0.0f;

	if (prevtime != 0) {
	        clock_t diff = curtime - prevtime;
	        float rawfps = ((100 * CLOCKS_PER_SEC) / diff) / 100.0f;

	        if (fps == 0.0f)
	            fps = rawfps;
	        else
	            fps = fps * 0.9f + rawfps / 10.0f;
	    }
	return fps;
}

GSFONT* loadFont(const char* path){
	int file = open(path, O_RDONLY, 0777);
	uint16_t magic;
	read(file, &magic, 2);
	close(file);
	GSFONT* font = NULL;
	if (magic == 0x4D42) {
		font = gsKit_init_font(GSKIT_FTYPE_BMP_DAT, (char*)path);
		gsKit_font_upload(gsGlobal, font);
	} else if (magic == 0x4246) {
		font = gsKit_init_font(GSKIT_FTYPE_FNT, (char*)path);
		gsKit_font_upload(gsGlobal, font);
	} else if (magic == 0x5089) { 
		font = gsKit_init_font(GSKIT_FTYPE_PNG_DAT, (char*)path);
		gsKit_font_upload(gsGlobal, font);
	}

	return font;
}

void printFontText(GSFONT* font, char* text, float x, float y, float scale, Color color)
{
	gsKit_set_test(gsGlobal, GS_ATEST_ON);
	gsKit_font_print_scaled(gsGlobal, font, x-0.5f, y-0.5f, 1, scale, color, text);
}

void unloadFont(GSFONT* font)
{
	gsKit_TexManager_free(gsGlobal, font->Texture);
	// clut was pointing to static memory, so do not free
	font->Texture->Clut = NULL;
	// mem was pointing to 'TexBase', so do not free
	font->Texture->Mem = NULL;
	// free texture
	free(font->Texture);
	font->Texture = NULL;

	if (font->RawData != NULL)
		free(font->RawData);

	free(font);
}

int getFreeVRAM(){
	return (4096 - (gsGlobal->CurrentPointer / 1024));
}


void drawImageCentered(GSTEXTURE* source, float x, float y, float width, float height, float startx, float starty, float endx, float endy, Color color)
{

	if (source->Delayed == true) {
		gsKit_TexManager_bind(gsGlobal, source);
	}
	gsKit_prim_sprite_texture(gsGlobal, source, 
					x-width/2, // X1
					y-height/2, // Y1
					startx,  // U1
					starty,  // V1
					(width/2+x), // X2
					(height/2+y), // Y2
					endx, // U2
					endy, // V2
					1, 
					color);	

}

void drawImage(GSTEXTURE* source, float x, float y, float width, float height, float startx, float starty, float endx, float endy, Color color)
{

	if (source->Delayed == true) {
		gsKit_TexManager_bind(gsGlobal, source);
	}
	gsKit_prim_sprite_texture(gsGlobal, source, 
					x-0.5f, // X1
					y-0.5f, // Y1
					startx,  // U1
					starty,  // V1
					(width+x)-0.5f, // X2
					(height+y)-0.5f, // Y2
					endx, // U2
					endy, // V2
					1, 
					color);	
}


void drawImageRotate(GSTEXTURE* source, float x, float y, float width, float height, float startx, float starty, float endx, float endy, float angle, Color color){

	float c = cosf(angle);
	float s = sinf(angle);

	if (source->Delayed == true) {
		gsKit_TexManager_bind(gsGlobal, source);
	}
	gsKit_prim_quad_texture(gsGlobal, source, 
							(-width/2)*c - (-height/2)*s+x, (-height/2)*c + (-width/2)*s+y, startx, starty, 
							(-width/2)*c - height/2*s+x, height/2*c + (-width/2)*s+y, startx, endy, 
							width/2*c - (-height/2)*s+x, (-height/2)*c + width/2*s+y, endx, starty, 
							width/2*c - height/2*s+x, height/2*c + width/2*s+y, endx, endy, 
							1, color);

}

void drawPixel(float x, float y, Color color)
{
	gsKit_prim_point(gsGlobal, x, y, 1, color);
}

void drawLine(float x, float y, float x2, float y2, Color color)
{
	gsKit_prim_line(gsGlobal, x, y, x2, y2, 1, color);
}


void drawRect(float x, float y, int width, int height, Color color)
{
	gsKit_prim_sprite(gsGlobal, x-0.5f, y-0.5f, (x+width)-0.5f, (y+height)-0.5f, 1, color);
}

void drawRectCentered(float x, float y, int width, int height, Color color)
{
	gsKit_prim_sprite(gsGlobal, x-width/2, y-height/2, (x+width)-width/2, (y+height)-height/2, 1, color);
}

void drawTriangle(float x, float y, float x2, float y2, float x3, float y3, Color color)
{
	gsKit_prim_triangle(gsGlobal, x, y, x2, y2, x3, y3, 1, color);
}

void drawTriangle_gouraud(float x, float y, float x2, float y2, float x3, float y3, Color color, Color color2, Color color3)
{
	gsKit_prim_triangle_gouraud(gsGlobal, x, y, x2, y2, x3, y3, 1, color, color2, color3);
}

void drawQuad(float x, float y, float x2, float y2, float x3, float y3, float x4, float y4, Color color)
{
	gsKit_prim_quad(gsGlobal, x, y, x2, y2, x3, y3, x4, y4, 1, color);
}

void drawQuad_gouraud(float x, float y, float x2, float y2, float x3, float y3, float x4, float y4, Color color, Color color2, Color color3, Color color4)
{
	gsKit_prim_quad_gouraud(gsGlobal, x, y, x2, y2, x3, y3, x4, y4, 1, color, color2, color3, color4);
}

void drawCircle(float x, float y, float radius, u64 color, u8 filled)
{
	float v[37*2];
	int a;
	float ra;

	for (a = 0; a < 36; a++) {
		ra = DEG2RAD(a*10);
		v[a*2] = cos(ra) * radius + x;
		v[a*2+1] = sin(ra) * radius + y;
	}

	if (!filled) {
		v[36*2] = radius + x;
		v[36*2 + 1] = y;
	}
	
	if (filled)
		gsKit_prim_triangle_fan(gsGlobal, v, 36, 1, color);
	else
		gsKit_prim_line_strip(gsGlobal, v, 37, 1, color);
}

void InvalidateTexture(GSTEXTURE *txt)
{
    gsKit_TexManager_invalidate(gsGlobal, txt);
}

void UnloadTexture(GSTEXTURE *txt)
{
	gsKit_TexManager_free(gsGlobal, txt);
	
}

int GetInterlacedFrameMode()
{
    if ((gsGlobal->Interlace == GS_INTERLACED) && (gsGlobal->Field == GS_FRAME))
        return 1;

    return 0;
}

GSGLOBAL *getGSGLOBAL(){return gsGlobal;}

void setVideoMode(s16 mode, int width, int height, int psm, s16 interlace, s16 field) {
	gsGlobal->Mode = mode;
	gsGlobal->Width = width;
	if ((interlace == GS_INTERLACED) && (field == GS_FRAME))
		gsGlobal->Height = height / 2;
	else
		gsGlobal->Height = height;

	gsGlobal->PSM = psm;
	gsGlobal->PSMZ = GS_PSMZ_16;

	gsGlobal->ZBuffering = GS_SETTING_OFF;
	gsGlobal->DoubleBuffering = GS_SETTING_ON;
	gsGlobal->PrimAlphaEnable = GS_SETTING_ON;
	gsGlobal->Dithering = GS_SETTING_OFF;

	gsGlobal->Interlace = interlace;
	gsGlobal->Field = field;

	gsKit_set_primalpha(gsGlobal, GS_SETREG_ALPHA(0, 1, 0, 1, 0), 0);

	printf("\nGraphics: created video surface of (%d, %d)\n",
		gsGlobal->Width, gsGlobal->Height);

	gsKit_set_clamp(gsGlobal, GS_CMODE_REPEAT);
	gsKit_vram_clear(gsGlobal);
	gsKit_init_screen(gsGlobal);
	gsKit_set_display_offset(gsGlobal, -0.5f, -0.5f);
	gsKit_sync_flip(gsGlobal);

	gsKit_mode_switch(gsGlobal, GS_ONESHOT);
    gsKit_clear(gsGlobal, BLACK_RGBAQ);	
}

void fntDrawQuad(rm_quad_t *q)
{
    gsKit_TexManager_bind(gsGlobal, q->txt);
    gsKit_prim_sprite_texture(gsGlobal, q->txt,
                              q->ul.x-0.5f, q->ul.y-0.5f,
                              q->ul.u, q->ul.v,
                              q->br.x-0.5f, q->br.y-0.5f,
                              q->br.u, q->br.v, 1, q->color);
}


void initGraphics()
{

	gsGlobal = gsKit_init_global();

	gsGlobal->Mode = gsKit_check_rom();
	if (gsGlobal->Mode == GS_MODE_PAL){
		gsGlobal->Height = 512;
	} else {
		gsGlobal->Height = 448;
	}

	gsGlobal->PSM  = GS_PSM_CT24;
	gsGlobal->PSMZ = GS_PSMZ_16;
	gsGlobal->ZBuffering = GS_SETTING_OFF;
	gsGlobal->DoubleBuffering = GS_SETTING_ON;
	gsGlobal->PrimAlphaEnable = GS_SETTING_ON;
	gsGlobal->Dithering = GS_SETTING_OFF;

	gsKit_set_primalpha(gsGlobal, GS_SETREG_ALPHA(0, 1, 0, 1, 0), 0);

	dmaKit_init(D_CTRL_RELE_OFF, D_CTRL_MFD_OFF, D_CTRL_STS_UNSPEC, D_CTRL_STD_OFF, D_CTRL_RCYC_8, 1 << DMA_CHANNEL_GIF);
	dmaKit_chan_init(DMA_CHANNEL_GIF);

	printf("\nGraphics: created video surface of (%d, %d)\n",
		gsGlobal->Width, gsGlobal->Height);

	gsKit_set_clamp(gsGlobal, GS_CMODE_REPEAT);

	gsKit_vram_clear(gsGlobal);

	gsKit_init_screen(gsGlobal);

	//gsKit_TexManager_init(gsGlobal);

	gsKit_mode_switch(gsGlobal, GS_ONESHOT);

    gsKit_clear(gsGlobal, BLACK_RGBAQ);	
	gsKit_vsync_wait();
	flipScreen();
	gsKit_clear(gsGlobal, BLACK_RGBAQ);	
	gsKit_vsync_wait();
	flipScreen();

}

void flipScreen()
{	
	//gsKit_set_finish(gsGlobal);
	gsKit_queue_exec(gsGlobal);
	gsKit_finish();
	gsKit_sync_flip(gsGlobal);
	gsKit_TexManager_nextFrame(gsGlobal);
}

void graphicWaitVblankStart(){

	gsKit_vsync_wait();

}
