#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <malloc.h>
#include <string.h>
#include <math.h>
#include <fcntl.h>

#include <jpeglib.h>
#include <time.h>
#include <png.h>

#include "include/graphics.h"

static bool vsync = true;
static clock_t curtime = 0;
static float fps = 0.0f;

static int frames = 0;
static int frame_interval = -1;

unsigned int loadraw(FILE* file)
{
    unsigned int tex_id;

    int image_size = 128 * 128 * 4;
    void* texels = memalign(128, image_size);
	fread(texels, 1, image_size, file);

    glGenTextures(1, &tex_id);
    glBindTexture(GL_TEXTURE_2D, tex_id);
    glTexImage2D(GL_TEXTURE_2D, 0, 3,
        128, 128, 0,
        GL_RGBA, GL_UNSIGNED_BYTE, texels);

	return tex_id;

}

gl_texture_t* loadpng(FILE* file) {
    gl_texture_t* tex = (gl_texture_t*)malloc(sizeof(gl_texture_t));

    GLenum textureFormat; // formato de cor da textura
    u32 sig_read = 0;
    int interlace_type, bit_depth, colorType;
    // Cria a estrutura png_struct para leitura do arquivo PNG
    png_structp png = png_create_read_struct(PNG_LIBPNG_VER_STRING, (png_voidp)NULL, NULL, NULL);
    if (!png) {
        printf("Erro ao criar a estrutura png_struct.\n");
        fclose(file);
        return NULL;
    }
    
    // Cria a estrutura png_info para obter informa��es da imagem PNG
    png_infop info = png_create_info_struct(png);

    if (!info) {
        printf("Erro ao criar a estrutura png_info.\n");
		fclose(file);
        png_destroy_read_struct(&png, (png_infopp)NULL, (png_infopp)NULL);
        return NULL;
    }
    
    // Configura o tratamento de erros da libpng
    if (setjmp(png_jmpbuf(png))) {
        printf("Erro durante o processamento do arquivo PNG.\n");
        png_destroy_read_struct(&png, &info, (png_infopp)NULL);
        fclose(file);
        return NULL;
    }

    png_init_io(png, file);

    png_set_sig_bytes(png, sig_read);

    png_read_info(png, info);
    
    png_get_IHDR(png, info, &(tex->width), &(tex->height), &bit_depth, &colorType, &interlace_type, NULL, NULL);

    if (bit_depth == 16) png_set_strip_16(png);
	if (colorType == PNG_COLOR_TYPE_GRAY || bit_depth < 4) png_set_expand(png);
	if (png_get_valid(png, info, PNG_INFO_tRNS)) png_set_tRNS_to_alpha(png);

	png_set_filler(png, 0xff, PNG_FILLER_AFTER);

	png_read_update_info(png, info);

	tex->clut = NULL;

	colorType = png_get_color_type(png, info);
    
    // Verifica se a imagem � RGBA e converte para RGB, se necess�rio
    switch (colorType)
    {
    /*case PNG_COLOR_TYPE_GRAY:
      tex->format = GL_LUMINANCE;
      tex->internalFormat = 1;
      break;*/

    case PNG_COLOR_TYPE_PALETTE:
    {
		struct png_clut { u8 r, g, b, a; };

		png_colorp palette = NULL;
		int num_pallete = 0;
		png_bytep trans = NULL;
		int num_trans = 0;

        png_get_PLTE(png, info, &palette, &num_pallete);
        png_get_tRNS(png, info, &trans, &num_trans, NULL);

		tex->format = GL_COLOR_INDEX;
        tex->internalFormat = 1;

        tex->clut_size = (bit_depth == 4? 16 : 256);
        tex->clut = (u32*)memalign(128, tex->clut_size*sizeof(int));
        memset(tex->clut, 0, tex->clut_size*sizeof(int));

    	struct png_clut *clut = (struct png_clut *)tex->clut;

    	int i, j, k = 0;

    	for (i = num_pallete; i < tex->clut_size; i++) {
    	    memset(&clut[i], 0, sizeof(clut[i]));
    	}

    	for (i = 0; i < num_pallete; i++) {
    	    clut[i].r = palette[i].red;
    	    clut[i].g = palette[i].green;
    	    clut[i].b = palette[i].blue;
    	    clut[i].a = 0xFF;
    	}

    	for (i = 0; i < num_trans; i++)
    	    clut[i].a = trans[i];

        if(bit_depth == 8) {
    	    // rotate clut
    	    for (i = 0; i < num_pallete; i++) {
    	        if ((i & 0x18) == 8) {
    	            struct png_clut tmp = clut[i];
    	            clut[i] = clut[i + 8];
    	            clut[i + 8] = tmp;
    	        }
    	    }
        }

	}
      break;

    /*case PNG_COLOR_TYPE_GRAY_ALPHA:
      tex->format = GL_LUMINANCE_ALPHA;
      tex->internalFormat = 2;
      break;*/

    case PNG_COLOR_TYPE_RGB:
      tex->format = GL_RGB;
      tex->internalFormat = 3;
      break;

    case PNG_COLOR_TYPE_RGB_ALPHA:
      tex->format = GL_RGBA;
      tex->internalFormat = 4;
      break;

    default:
      /* Badness */
      break;
    }
    
    // Obt�m o tamanho dos dados da imagem
    size_t rowBytes = png_get_rowbytes(png, info);
    
    // Aloca mem�ria para armazenar os dados da imagem
    tex->texels = (GLubyte *)memalign (16, sizeof (GLubyte) * tex->width * tex->height * tex->internalFormat);

    /* Setup a pointer array.  Each one points at the begening of a row. */
    png_bytep *row_pointers = (png_bytep *)malloc (sizeof (png_bytep) * tex->height);

    for (int i = 0; i < tex->height; ++i) {
        row_pointers[i] = (png_bytep)(tex->texels + (i * tex->width * tex->internalFormat));
    }

    // L� os dados da imagem linha por linha
    png_read_image(png, row_pointers);
    png_read_end(png, NULL);

	// Libera a mem�ria dos dados da imagem
    free(row_pointers);

    png_destroy_read_struct(&png, &info, NULL);
    fclose(file);
    
    // Gera uma nova textura OpenGL
    glGenTextures(1, &tex->id);
    
    // Vincula a textura
    glBindTexture(GL_TEXTURE_2D, tex->id);
    
    // Define os par�metros de filtragem da textura
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    
    // Define os par�metros de repeti��o da textura
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    
    // Carrega os dados da imagem na textura
    glTexImage2D(GL_TEXTURE_2D, 0, tex->format, tex->width, tex->height, 0, tex->format, GL_UNSIGNED_BYTE, tex->texels);

    return tex;
}


struct gsBitMapFileHeader
{
	u16	Type;
	u32	Size;
	u16 Reserved1;
	u16 Reserved2;
	u32 Offset;
} __attribute__ ((packed));
typedef struct gsBitMapFileHeader GSBMFHDR;

struct gsBitMapInfoHeader
{
	u32	Size;
	u32	Width;
	u32	Height;
	u16	Planes;
	u16 BitCount;
	u32 Compression;
	u32 SizeImage;
	u32 XPelsPerMeter;
	u32 YPelsPerMeter;
	u32 ColorUsed;
	u32 ColorImportant;
} __attribute__ ((packed));
typedef struct gsBitMapInfoHeader GSBMIHDR;

struct gsBitMapClut
{
	u8 Blue;
	u8 Green;
	u8 Red;
	u8 Alpha;
} __attribute__ ((packed));
typedef struct gsBitMapClut GSBMCLUT;

struct gsBitmap
{
	GSBMFHDR FileHeader;
	GSBMIHDR InfoHeader;
	char *Texture;
	GSBMCLUT *Clut;
};
typedef struct gsBitmap GSBITMAP;

gl_texture_t* loadbmp(FILE* file)
{
    GSBITMAP Bitmap;
	int x, y;
	int cy;
	u32 FTexSize;
	u8  *image;
	u8  *p;

    gl_texture_t* tex = (gl_texture_t*)malloc(sizeof(gl_texture_t));

	if (file == NULL)
	{
		printf("BMP: Failed to load bitmap\n");
		return NULL;
	}
	if (fread(&Bitmap.FileHeader, sizeof(Bitmap.FileHeader), 1, file) <= 0)
	{
		printf("BMP: Could not load bitmap\n");
		fclose(file);
		return NULL;
	}

	if (fread(&Bitmap.InfoHeader, sizeof(Bitmap.InfoHeader), 1, file) <= 0)
	{
		printf("BMP: Could not load bitmap\n");
		fclose(file);
		return NULL;
	}

	tex->width = Bitmap.InfoHeader.Width;
	tex->height = Bitmap.InfoHeader.Height;

	if(Bitmap.InfoHeader.BitCount == 4)
	{
		tex->format = GL_COLOR_INDEX;
        tex->internalFormat = 1;
        
        tex->clut_size = 16;
		tex->clut = (u32*)memalign(128, tex->clut_size*sizeof(u32));

		memset(tex->clut, 0, tex->clut_size*sizeof(u32));
		fseek(file, 54, SEEK_SET);
		if (fread(tex->clut, Bitmap.InfoHeader.ColorUsed*sizeof(u32), 1, file) <= 0)
		{
			if (tex->clut) {
				free(tex->clut);
				tex->clut = NULL;
			}
			printf("BMP: Could not load bitmap\n");
			fclose(file);
			return NULL;
		}

		GSBMCLUT *clut = (GSBMCLUT *)tex->clut;
		int i;
		for (i = Bitmap.InfoHeader.ColorUsed; i < tex->clut_size; i++)
		{
			memset(&clut[i], 0, sizeof(clut[i]));
		}

		for (i = 0; i < tex->clut_size; i++)
		{
			u8 tmp = clut[i].Blue;
			clut[i].Blue = clut[i].Red;
			clut[i].Red = tmp;
			clut[i].Alpha = 0xFF;
		}

	}
	else if(Bitmap.InfoHeader.BitCount == 8)
	{
		tex->format = GL_COLOR_INDEX;
        tex->internalFormat = 1;
        
        tex->clut_size = 256;
		tex->clut = (u32*)memalign(128, tex->clut_size*sizeof(u32));

		memset(tex->clut, 0, tex->clut_size*sizeof(u32));
        
		fseek(file, 54, SEEK_SET);
		if (fread(tex->clut, Bitmap.InfoHeader.ColorUsed*sizeof(u32), 1, file) <= 0)
		{
			if (tex->clut) {
				free(tex->clut);
				tex->clut = NULL;
			}
			printf("BMP: Could not load bitmap\n");
			fclose(file);
			return NULL;
		}

		GSBMCLUT *clut = (GSBMCLUT *)tex->clut;
		int i;
		for (i = Bitmap.InfoHeader.ColorUsed; i < tex->clut_size; i++)
		{
			memset(&clut[i], 0, sizeof(clut[i]));
		}

		for (i = 0; i < tex->clut_size; i++)
		{
			u8 tmp = clut[i].Blue;
			clut[i].Blue = clut[i].Red;
			clut[i].Red = tmp;
			clut[i].Alpha = 0xFF;
		}

		// rotate clut
		for (i = 0; i < tex->clut_size; i++)
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
		tex->format = GL_RGB5_A1;
        tex->internalFormat = 2;
		tex->clut_size = 0;
		tex->clut = NULL;
	}
	else if(Bitmap.InfoHeader.BitCount == 24)
	{
		tex->format = GL_RGB;
        tex->internalFormat = 3;
	}

	fseek(file, 0, SEEK_END);
	FTexSize = ftell(file);
	FTexSize -= Bitmap.FileHeader.Offset;

	fseek(file, Bitmap.FileHeader.Offset, SEEK_SET);

	u32 TextureSize = tex->width * tex->height * tex->internalFormat;

	tex->texels = (u8*)memalign(128, TextureSize);

	if(Bitmap.InfoHeader.BitCount == 24)
	{
		image = (u8*)memalign(128, FTexSize);
		if (image == NULL) {
			printf("BMP: Failed to allocate memory\n");
			if (tex->texels) {
				free(tex->texels);
				tex->texels = NULL;
			}
			if (tex->clut) {
				free(tex->clut);
				tex->clut = NULL;
			}
			fclose(file);
			return NULL;
		}

		fread(image, FTexSize, 1, file);
		p = (u8*)((u32)tex->texels);
		for (y = tex->height - 1, cy = 0; y >= 0; y--, cy++) {
			for (x = 0; x < tex->width; x++) {
				p[(y * tex->width + x) * 3 + 2] = image[(cy * tex->width + x) * 3 + 0];
				p[(y * tex->width + x) * 3 + 1] = image[(cy * tex->width + x) * 3 + 1];
				p[(y * tex->width + x) * 3 + 0] = image[(cy * tex->width + x) * 3 + 2];
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
			if (tex->texels) {
				free(tex->texels);
				tex->texels = NULL;
			}
			if (tex->clut) {
				free(tex->clut);
				tex->clut = NULL;
			}
			fclose(file);
			return NULL;
		}

		fread(image, FTexSize, 1, file);

		p = (u8*)((u32*)tex->texels);
		for (y = tex->height - 1, cy = 0; y >= 0; y--, cy++) {
			for (x = 0; x < tex->width; x++) {
				u16 value;
				value = *(u16*)&image[(cy * tex->width + x) * 2];
				value = (value & 0x8000) | value << 10 | (value & 0x3E0) | (value & 0x7C00) >> 10;	//ARGB -> ABGR

				*(u16*)&p[(y * tex->width + x) * 2] = value;
			}
		}
		free(image);
		image = NULL;
	}
	else if(Bitmap.InfoHeader.BitCount == 8 || Bitmap.InfoHeader.BitCount == 4)
	{
		char *text = (char *)((u32)tex->texels);
		image = (u8*)memalign(128, FTexSize);
		if (image == NULL) {
			printf("BMP: Failed to allocate memory\n");
			if (tex->texels) {
				free(tex->texels);
				tex->texels = NULL;
			}
			if (tex->clut) {
				free(tex->clut);
				tex->clut = NULL;
			}
			fclose(file);
			return NULL;
		}

		if (fread(image, FTexSize, 1, file) != 1)
		{
			if (tex->texels) {
				free(tex->texels);
				tex->texels = NULL;
			}
			if (tex->clut) {
				free(tex->clut);
				tex->clut = NULL;
			}
			printf("BMP: Read failed!, Size %d\n", FTexSize);
			free(image);
			image = NULL;
			fclose(file);
			return NULL;
		}
		for (y = tex->height - 1; y >= 0; y--)
		{
			if(Bitmap.InfoHeader.BitCount == 8)
				memcpy(&text[y * tex->width], &image[(tex->height - y - 1) * tex->width], tex->width);
			else
				memcpy(&text[y * (tex->width / 2)], &image[(tex->height - y - 1) * (tex->width / 2)], tex->width / 2);
		}
		free(image);
		image = NULL;

		if(Bitmap.InfoHeader.BitCount == 4)
		{
			int byte;
			u8 *tmpdst = (u8 *)((u32)tex->texels);
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

	fclose(file);

    // Gera uma nova textura OpenGL
    glGenTextures(1, &tex->id);
    
    // Vincula a textura
    glBindTexture(GL_TEXTURE_2D, tex->id);
    
    // Define os par�metros de filtragem da textura
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    
    // Define os par�metros de repeti��o da textura
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    
    // Carrega os dados da imagem na textura
    glTexImage2D(GL_TEXTURE_2D, 0, tex->format, tex->width, tex->height, 0, tex->format, GL_UNSIGNED_BYTE, tex->texels);

    // Retorna a textura carregada
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
static void  _ps2_load_JPEG_generic(gl_texture_t* tex, struct jpeg_decompress_struct *cinfo, struct my_error_mgr *jerr, bool scale_down)
{
	int textureSize = 0;
	if (scale_down) {
		unsigned int longer = cinfo->image_width > cinfo->image_height ? cinfo->image_width : cinfo->image_height;
		float downScale = (float)longer / (float)MAX_TEXTURE;
		cinfo->scale_denom = ceilf(downScale);
	}

	jpeg_start_decompress(cinfo);

	tex->width =  cinfo->output_width;
	tex->height = cinfo->output_height;
    tex->format = cinfo->out_color_components == 3 ? GL_RGB : GL_RGBA;
	tex->internalFormat = cinfo->out_color_components;
	tex->clut_size = 0;
	tex->clut = NULL;

	textureSize = cinfo->output_width*cinfo->output_height*cinfo->out_color_components;
	#ifdef DEBUG
	printf("Texture Size = %i\n",textureSize);
	#endif
	tex->texels = (u8*)memalign(128, textureSize);

	unsigned int row_stride = textureSize/tex->height;
	unsigned char *row_pointer = (unsigned char *)tex->texels;
	while (cinfo->output_scanline < cinfo->output_height) {
		jpeg_read_scanlines(cinfo, (JSAMPARRAY)&row_pointer, 1);
		row_pointer += row_stride;
	}

	jpeg_finish_decompress(cinfo);
}

gl_texture_t* loadjpeg(FILE* fp, bool scale_down)
{
    gl_texture_t* tex = (gl_texture_t*)malloc(sizeof(gl_texture_t));

	struct jpeg_decompress_struct cinfo;
	struct my_error_mgr jerr;

	if (tex == NULL) {
		printf("jpeg: error Texture is NULL\n");
		return NULL;
	}

	if (fp == NULL)
	{
		printf("jpeg: Failed to load file\n");
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
		if (tex->texels)
			free(tex->texels);
		printf("jpeg: error during processing file\n");
		return NULL;
	}
	jpeg_create_decompress(&cinfo);
	jpeg_stdio_src(&cinfo, fp);
	jpeg_read_header(&cinfo, TRUE);

	_ps2_load_JPEG_generic(tex, &cinfo, &jerr, scale_down);
	
	jpeg_destroy_decompress(&cinfo);
	fclose(fp);

    // Gera uma nova textura OpenGL
    glGenTextures(1, &tex->id);
    
    // Vincula a textura
    glBindTexture(GL_TEXTURE_2D, tex->id);
    
    // Define os par�metros de filtragem da textura
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    
    // Define os par�metros de repeti��o da textura
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    
    // Carrega os dados da imagem na textura
    glTexImage2D(GL_TEXTURE_2D, 0, tex->format, tex->width, tex->height, 0, tex->format, GL_UNSIGNED_BYTE, tex->texels);

	return tex;


}

gl_texture_t* load_image(const char* path, bool delayed){
	FILE* file = fopen(path, "rb");
	uint16_t magic;
	fread(&magic, 1, 2, file);
	fseek(file, 0, SEEK_SET);
	gl_texture_t* image = NULL;
	if (magic == 0x4D42) {
		image = loadbmp(file);
	} else if (magic == 0xD8FF) {
		image = loadjpeg(file, false);
	} else if (magic == 0x5089) {
		image = loadpng(file);
	}

	if (image == NULL) printf("Failed to load image %s.\n", path);

	return image;
}

void clearScreen(Color color)
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    //glLoadIdentity();//load identity matrix
	glClearColor(R(color)/255.0f, G(color)/255.0f, B(color)/255.0f, A(color)/255.0f);

	//glTranslatef(0.0f,0.0f,-4.0f);//move forward 4 units

	glLoadIdentity();

}

void loadFontM()
{

}

void printFontMText(const char* text, float x, float y, float scale, Color color)
{

}

void unloadFontM()
{

}

float FPSCounter(int interval)
{
	frame_interval = interval;
	return fps;
}

void* loadFont(const char* path){
	int file = open(path, O_RDONLY, 0777);
	uint16_t magic;
	read(file, &magic, 2);
	close(file);
	void* font = NULL;
	if (magic == 0x4D42) {

	} else if (magic == 0x4246) {

	} else if (magic == 0x5089) { 

	}

	return font;
}

void printFontText(void* font, const char* text, float x, float y, float scale, Color color)
{

}

void unloadFont(void* font)
{

}

int getFreeVRAM(){
	return 0;
}


void drawImageCentered(gl_texture_t* source, float x, float y, float width, float height, float startx, float starty, float endx, float endy, Color color)
{


}

void drawImage(gl_texture_t* source, float x, float y, float width, float height, float startx, float starty, float endx, float endy, Color color)
{
    glBindTexture(GL_TEXTURE_2D, source->id);
    glEnable(GL_TEXTURE_2D);
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
    if(source->clut) {
        glColorTable(GL_COLOR_TABLE, GL_RGBA, source->clut_size, GL_RGBA, GL_UNSIGNED_INT_8_8_8_8, source->clut);
    }

    //glDisable(GL_LIGHTING);

    glColor4f(R(color)/255.0f, G(color)/255.0f, B(color)/255.0f, A(color)/255.0f); //blue colors

    glBegin(GL_QUADS);
	
    glTexCoord2f(startx, starty);  // canto inferior esquerdo
    glVertex2f(x,  y);

    glTexCoord2f(endx, starty);  // canto inferior direito
    glVertex2f(x+width, y);

    glTexCoord2f(endx, endy);  // canto superior direito
    glVertex2f(x+width, y+height);

    glTexCoord2f(startx, endy);  // canto superior esquerdo
    glVertex2f(x, y+height);

    glEnd();

	glFlush();

    glDisable(GL_TEXTURE_2D);

    //glEnable(GL_LIGHTING);
}


void drawImageRotate(gl_texture_t* source, float x, float y, float width, float height, float startx, float starty, float endx, float endy, float angle, Color color){
    glBindTexture(GL_TEXTURE_2D, source->id);
    glEnable(GL_TEXTURE_2D);
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
    if(source->clut) {
        glColorTable(GL_COLOR_TABLE, GL_RGBA, source->clut_size, GL_RGBA, GL_UNSIGNED_INT_8_8_8_8, source->clut);
    }

    glDisable(GL_LIGHTING);

    glColor4f(R(color)/255.0f, G(color)/255.0f, B(color)/255.0f, A(color)/255.0f); //blue colors

    glPushMatrix();
    glTranslatef(x + width/2, y + height/2, 0.0f); // Translada para o centro do ret�ngulo
    glRotatef(angle, 0.0f, 0.0f, 1.0f); // Aplica a rota��o no eixo Z
    glTranslatef(-width/2, -height/2, 0.0f); // Translada de volta para a posi��o original

    glBegin(GL_QUADS);
	
    glTexCoord2f(startx, starty);  // canto inferior esquerdo
    glVertex2f(x,  y);

    glTexCoord2f(endx, starty);  // canto inferior direito
    glVertex2f(x+width, y);

    glTexCoord2f(endx, endy);  // canto superior direito
    glVertex2f(x+width, y+height);

    glTexCoord2f(startx, endy);  // canto superior esquerdo
    glVertex2f(x, y+height);

    glEnd();

    glPopMatrix();

    glDisable(GL_TEXTURE_2D);

    glEnable(GL_LIGHTING);
}

void drawPixel(float x, float y, Color color)
{
    glColor4f(R(color)/255.0f, G(color)/255.0f, B(color)/255.0f, A(color)/255.0f);

    glBegin(GL_POINTS);
	
    glVertex2f(x,  y);

    glEnd();

}

void drawLine(float x, float y, float x2, float y2, Color color)
{
    glColor4f(R(color)/255.0f, G(color)/255.0f, B(color)/255.0f, A(color)/255.0f);

    glBegin(GL_LINES);
	
    glVertex2f(x,  y);
    glVertex2f(x2,  y2);

    glEnd();

}


void drawRect(float x, float y, int width, int height, Color color)
{
    glBegin(GL_QUADS);
	
    glColor4f(R(color)/255.0f, G(color)/255.0f, B(color)/255.0f, A(color)/255.0f);
    glVertex2f(x,  y);
    glColor4f(R(color)/255.0f, G(color)/255.0f, B(color)/255.0f, A(color)/255.0f);
    glVertex2f(x+width, y);
    glColor4f(R(color)/255.0f, G(color)/255.0f, B(color)/255.0f, A(color)/255.0f);
    glVertex2f(x+width, y+height);
    glColor4f(R(color)/255.0f, G(color)/255.0f, B(color)/255.0f, A(color)/255.0f);
    glVertex2f(x, y+height);

    glEnd();

}

void drawRectCentered(float x, float y, int width, int height, Color color)
{

}

void drawTriangle(float x, float y, float x2, float y2, float x3, float y3, Color color)
{
	glColor4f(R(color)/255.0f, G(color)/255.0f, B(color)/255.0f, A(color)/255.0f);

    glBegin(GL_TRIANGLES);
    glVertex2f(x,  y);
    glVertex2f(x2, y2);
    glVertex2f(x3, y3);
    glEnd();


}

void drawTriangle_gouraud(float x, float y, float x2, float y2, float x3, float y3, Color color, Color color2, Color color3)
{

    glBegin(GL_TRIANGLES);

	glColor4f(R(color)/255.0f, G(color)/255.0f, B(color)/255.0f, A(color)/255.0f);
    glVertex2f(x ,  y);

	glColor4f(R(color2)/255.0f, G(color2)/255.0f, B(color2)/255.0f, A(color2)/255.0f);
    glVertex2f(x2, y2);

	glColor4f(R(color3)/255.0f, G(color3)/255.0f, B(color3)/255.0f, A(color3)/255.0f);
    glVertex2f(x3, y3);

    glEnd();

}

void drawQuad(float x, float y, float x2, float y2, float x3, float y3, float x4, float y4, Color color)
{
    glBegin(GL_QUADS);
	
    glColor4f(R(color)/255.0f, G(color)/255.0f, B(color)/255.0f, A(color)/255.0f);
    glVertex2f(x,  y);
    glColor4f(R(color)/255.0f, G(color)/255.0f, B(color)/255.0f, A(color)/255.0f);
    glVertex2f(x2, y2);
    glColor4f(R(color)/255.0f, G(color)/255.0f, B(color)/255.0f, A(color)/255.0f);
    glVertex2f(x4, y4);
    glColor4f(R(color)/255.0f, G(color)/255.0f, B(color)/255.0f, A(color)/255.0f);
    glVertex2f(x3, y3);

    glEnd();

}

void drawQuad_gouraud(float x, float y, float x2, float y2, float x3, float y3, float x4, float y4, Color color, Color color2, Color color3, Color color4)
{
    glBegin(GL_QUADS);
	
    glColor4f(R(color)/255.0f, G(color)/255.0f, B(color)/255.0f, A(color)/255.0f);
    glVertex2f(x,  y);

    glColor4f(R(color2)/255.0f, G(color2)/255.0f, B(color2)/255.0f, A(color2)/255.0f);
    glVertex2f(x2, y2);

    glColor4f(R(color4)/255.0f, G(color4)/255.0f, B(color4)/255.0f, A(color4)/255.0f);
    glVertex2f(x4, y4);

    glColor4f(R(color3)/255.0f, G(color3)/255.0f, B(color3)/255.0f, A(color3)/255.0f);
    glVertex2f(x3, y3);

    glEnd();

}

void drawCircle(float x, float y, float radius, u64 color, u8 filled)
{
    glColor4f(R(color)/255.0f, G(color)/255.0f, B(color)/255.0f, A(color)/255.0f);

    if(filled) {
        glBegin(GL_TRIANGLE_FAN);
        glVertex2f(x, y);  // centro do c�rculo
    } else {
        glBegin(GL_LINE_LOOP);
    }

    for (int i = 0; i <= 36; i++) {
        float angle = 2.0f * M_PI * (float)i / (float)36;
        float sx = radius * cosf(angle);
        float sy = radius * sinf(angle);
        glVertex2f(x + sx, y + sy);
    }
    glEnd();
}

void InvalidateTexture(gl_texture_t *txt)
{

}

void UnloadTexture(gl_texture_t *txt)
{
	glDeleteTextures(1, &(txt->id));
	if(txt->clut) free(txt->clut);
	free(txt->texels);
}

int GetInterlacedFrameMode()
{
    return 1;
}

void setVSync(bool vsync_flag){ vsync = vsync_flag; }

void *getGSGLOBAL(){return NULL;}

void setVideoMode(s16 mode, int width, int height, int psm, s16 interlace, s16 field, bool zbuffering, int psmz) {

}

void fntDrawQuad(rm_quad_t *q)
{
    glBindTexture(GL_TEXTURE_2D, q->txt->id);
    glEnable(GL_TEXTURE_2D);
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

    glColorTable(GL_COLOR_TABLE, GL_RGBA, 256, GL_RGBA, GL_UNSIGNED_INT_8_8_8_8, q->txt->clut);

    //glDisable(GL_LIGHTING);

    glColor4f(R(q->color)/255.0f, G(q->color)/255.0f, B(q->color)/255.0f, A(q->color)/255.0f); //blue colors

    glBegin(GL_QUADS);

    glTexCoord2f(q->ul.u/q->txt->width, q->ul.v/q->txt->height);  // canto inferior esquerdo
    glVertex2f(q->ul.x,  q->ul.y);

    glTexCoord2f(q->br.u/q->txt->width, q->ul.v/q->txt->height);  // canto inferior direito
    glVertex2f( q->br.x, q->ul.y);

    glTexCoord2f(q->br.u/q->txt->width, q->br.v/q->txt->height);  // canto superior direito
    glVertex2f( q->br.x, q->br.y);

    glTexCoord2f(q->ul.u/q->txt->width, q->br.v/q->txt->height);  // canto superior esquerdo
    glVertex2f(q->ul.x, q->br.y);

    glEnd();

	glFlush();

    glDisable(GL_TEXTURE_2D);
}


static void
initGsMemory()
{
    // frame and depth buffer
    pgl_slot_handle_t frame_slot_0, frame_slot_1, depth_slot;
    frame_slot_0 = pglAddGsMemSlot(0, 70, GS::kPsm32);
    frame_slot_1 = pglAddGsMemSlot(70, 70, GS::kPsm32);
    depth_slot   = pglAddGsMemSlot(140, 70, GS::kPsmz24);
    // lock these slots so that they aren't allocated by the memory manager
    pglLockGsMemSlot(frame_slot_0);
    pglLockGsMemSlot(frame_slot_1);
    pglLockGsMemSlot(depth_slot);

    // create gs memory area objects to use for frame and depth buffers
    pgl_area_handle_t frame_area_0, frame_area_1, depth_area;
    frame_area_0 = pglCreateGsMemArea(640, 224, GS::kPsm24);
    frame_area_1 = pglCreateGsMemArea(640, 224, GS::kPsm24);
    depth_area   = pglCreateGsMemArea(640, 224, GS::kPsmz24);
    // bind the areas to the slots we created above
    pglBindGsMemAreaToSlot(frame_area_0, frame_slot_0);
    pglBindGsMemAreaToSlot(frame_area_1, frame_slot_1);
    pglBindGsMemAreaToSlot(depth_area, depth_slot);

    // draw to the new areas...
    pglSetDrawBuffers(PGL_INTERLACED, frame_area_0, frame_area_1, depth_area);
    // ...and display from them
    pglSetDisplayBuffers(PGL_INTERLACED, frame_area_0, frame_area_1);

    // 32 bit

    // a slot for fonts (probably)
    pglAddGsMemSlot(210, 2, GS::kPsm8);

    // 64x32
    pglAddGsMemSlot(212, 1, GS::kPsm32);
    pglAddGsMemSlot(213, 1, GS::kPsm32);
    // 64x64
    pglAddGsMemSlot(214, 2, GS::kPsm32);
    pglAddGsMemSlot(216, 2, GS::kPsm32);
    pglAddGsMemSlot(218, 2, GS::kPsm32);
    pglAddGsMemSlot(220, 2, GS::kPsm32);
    // 128x128
    pglAddGsMemSlot(222, 8, GS::kPsm32);
    pglAddGsMemSlot(230, 8, GS::kPsm32);
    // 256x256
    pglAddGsMemSlot(238, 32, GS::kPsm32);
    pglAddGsMemSlot(270, 32, GS::kPsm32);
    // 512x256
    pglAddGsMemSlot(302, 64, GS::kPsm32);
    pglAddGsMemSlot(366, 64, GS::kPsm32);

    pglAddGsMemSlot(430, 128, GS::kPsm32);
    pglAddGsMemSlot(558, 128, GS::kPsm32);

    pglPrintGsMemAllocation();
}

void gluPerspective(GLdouble fovy, GLdouble aspect, GLdouble zNear, GLdouble zFar)
{
    GLdouble xmin, xmax, ymin, ymax;

    ymax = zNear * tan(fovy * M_PI / 360.0f);
    ymin = -ymax;
    xmin = ymin * aspect;
    xmax = ymax * aspect;

    glFrustum(xmin, xmax, ymin, ymax, zNear, zFar);
}


void glutInit()
{
    // does the ps2gl library need to be initialized?

    if (!pglHasLibraryBeenInitted()) {
        // reset the machine
        //      sceDevVif0Reset();
        //      sceDevVu0Reset();
        //      sceDmaReset(1);
        //      sceGsResetPath();

        // Reset the GIF. OSDSYS leaves PATH3 busy, that ends up having
        // our PATH1/2 transfers ignored by the GIF.
        *GIF::Registers::ctrl = 1;

        //      sceGsResetGraph(0, SCE_GS_INTERLACE, SCE_GS_NTSC, SCE_GS_FRAME);
    	SetGsCrt(1 /* interlaced */, 2 /* ntsc */, 1 /* frame */);

        printf("ps2gl library has not been initialized by the user; using default values.");
        int immBufferVertexSize = 64 * 1024;
        pglInit(immBufferVertexSize, 1000);
    }

    // does gs memory need to be initialized?

    if (!pglHasGsMemBeenInitted()) {
        printf("GS memory has not been allocated by the user; using default values.");
        initGsMemory();
    }
}

void viewport_2d(int width, int height) // Create The Reshape Function (the viewport)
{
    glViewport(0, 0, width, height); // Reset The Current Viewport

    glMatrixMode(GL_PROJECTION);     // Select The Projection Matrix
	glLoadIdentity();
	
	glOrtho(0, width, height, 0, -1, 1);
    //glLoadIdentity();                // Reset The Projection Matrix

    // Calculate The Aspect Ratio Of The Window
    //gluPerspective(80, (GLfloat)width / (GLfloat)height, 1, 5000);

    glMatrixMode(GL_MODELVIEW); // Select The Modelview Matrix
    glLoadIdentity();
}

void reshape(int width, int height) // Create The Reshape Function (the viewport)
{
    glViewport(0, 0, width, height); // Reset The Current Viewport

    glMatrixMode(GL_PROJECTION);     // Select The Projection Matrix
	glLoadIdentity();
	
	glOrtho(0, width, height, 0, -1, 1);
    //glLoadIdentity();                // Reset The Projection Matrix

    // Calculate The Aspect Ratio Of The Window
    //gluPerspective(80, (GLfloat)width / (GLfloat)height, 1, 5000);

    glMatrixMode(GL_MODELVIEW); // Select The Modelview Matrix
    glLoadIdentity();
}

void InitGL(GLvoid) // Create Some Everyday Functions
{
    glShadeModel(GL_SMOOTH);              // Enable Smooth Shading
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f); // Black Background
    glClearDepth(1.0f);                   // Depth Buffer Setup
    glEnable(GL_DEPTH_TEST);              // Enables Depth Testing
	glEnable(GL_RESCALE_NORMAL);
    glDepthFunc(GL_LEQUAL);               // The Type Of Depth Testing To Do
    
    glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);

    // ps2gl needs lighting + color_material for per-vertex colors
    glEnable(GL_COLOR_MATERIAL);
    glColorMaterial(GL_FRONT_AND_BACK, GL_DIFFUSE);
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void initGraphics()
{
    glutInit();
    InitGL();

    bool firstTime = true;

    reshape(640, 448);

}

bool firstTime = true;

void flipScreen()
{	
    pglEndGeometry();

    if (!firstTime)
        pglFinishRenderingGeometry(PGL_DONT_FORCE_IMMEDIATE_STOP);
    else
        firstTime = false;

	if(vsync) pglWaitForVSync();
    pglSwapBuffers();
    pglRenderGeometry();

	if (frames > frame_interval && frame_interval != -1) {
		clock_t prevtime = curtime;
		curtime = clock();

		fps = ((float)(frame_interval)) / (((float)(curtime - prevtime)) / ((float)CLOCKS_PER_SEC));

		frames = 0;
	}
	frames++;

	pglBeginGeometry();
}

void graphicWaitVblankStart(){
    pglWaitForVSync();
}
