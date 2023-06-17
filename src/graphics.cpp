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

/* ps2sdk */
#include "graph.h"

/* GL */
#include "GL/ps2gl.h"

/* ps2stuff */
#include "ps2s/core.h"
#include "ps2s/displayenv.h"
#include "ps2s/drawenv.h"
#include "ps2s/gs.h"
#include "ps2s/timer.h"

/* ps2gl */
#include "ps2gl/debug.h"
#include "ps2gl/displaycontext.h"
#include "ps2gl/drawcontext.h"
#include "ps2gl/glcontext.h"

#define DEG2RAD(x) ((x)*0.01745329251)

void *gsGlobal = NULL;
void *gsFontM = NULL;

static bool vsync = true;
static int vsync_sema_id = 0;
static clock_t curtime = 0;
static float fps = 0.0f;

static int frames = 0;
static int frame_interval = -1;

struct gl_texture_t
{
  GLsizei width;
  GLsizei height;

  GLenum format;
  GLint internalFormat;
  GLuint id;

  GLubyte *texels;
};

unsigned int loadraw(FILE* File)
{
    unsigned int tex_id;

    int image_size = 128 * 128 * 4;
    void* texels = memalign(16, image_size);
	fread(texels, 1, image_size, File);

    glGenTextures(1, &tex_id);
    glBindTexture(GL_TEXTURE_2D, tex_id);
    glTexImage2D(GL_TEXTURE_2D, 0, 3,
        128, 128, 0,
        GL_RGBA, GL_UNSIGNED_BYTE, texels);

	return tex_id;

}

unsigned int loadpng(FILE* file) {
    gl_texture_t* tex = (gl_texture_t*)malloc(sizeof(gl_texture_t));
    GLenum textureFormat; // formato de cor da textura
    u32 sig_read = 0;
    // Cria a estrutura png_struct para leitura do arquivo PNG
    png_structp png = png_create_read_struct(PNG_LIBPNG_VER_STRING, (png_voidp)NULL, NULL, NULL);
    if (!png) {
        printf("Erro ao criar a estrutura png_struct.\n");
        fclose(file);
        return -1;
    }
    
    // Cria a estrutura png_info para obter informações da imagem PNG
    png_infop info = png_create_info_struct(png);
    if (!info) {
        printf("Erro ao criar a estrutura png_info.\n");
        png_destroy_read_struct(&png, (png_infopp)NULL, (png_infopp)NULL);
        fclose(file);
        return -1;
    }
    
    // Configura o tratamento de erros da libpng
    if (setjmp(png_jmpbuf(png))) {
        printf("Erro durante o processamento do arquivo PNG.\n");
        png_destroy_read_struct(&png, &info, (png_infopp)NULL);
        fclose(file);
        return -1;
    }
    
    // Inicia a leitura do arquivo PNG
    png_init_io(png, file);

    // Informa que já foram lidos os primeiros 8 bytes do cabeçalho PNG
    png_set_sig_bytes(png, sig_read);
    
    // Obtém as informações da imagem PNG
    png_read_info(png, info);
    
    // Obtém as dimensões da imagem
    tex->width = png_get_image_width(png, info);
    tex->height = png_get_image_height(png, info);
    
    // Obtém o tipo de cor da imagem
    int colorType = png_get_color_type(png, info);
    
    // Verifica se a imagem é RGBA e converte para RGB, se necessário
    switch (colorType)
    {
    case PNG_COLOR_TYPE_GRAY:
      tex->format = GL_LUMINANCE;
      tex->internalFormat = 1;
      break;

    case PNG_COLOR_TYPE_GRAY_ALPHA:
      tex->format = GL_LUMINANCE_ALPHA;
      tex->internalFormat = 2;
      break;

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
    
    // Atualiza as informações da imagem
    png_read_update_info(png, info);
    
    // Obtém o tamanho dos dados da imagem
    size_t rowBytes = png_get_rowbytes(png, info);
    
    // Aloca memória para armazenar os dados da imagem
    tex->texels = (GLubyte *)malloc (sizeof (GLubyte) * tex->width * tex->height * tex->internalFormat);

    /* Setup a pointer array.  Each one points at the begening of a row. */
    png_bytep *row_pointers = (png_bytep *)malloc (sizeof (png_bytep) * tex->height);

    for (int i = 0; i < tex->height; ++i) {
        row_pointers[i] = (png_bytep)(tex->texels + ((i) * tex->width * tex->internalFormat));
    }

    // Lê os dados da imagem linha por linha
    png_read_image(png, row_pointers);
    
    // Fecha o arquivo PNG
    fclose(file);
    
    // Gera uma nova textura OpenGL
    glGenTextures(1, &tex->id);
    
    // Vincula a textura
    glBindTexture(GL_TEXTURE_2D, tex->id);
    
    // Define os parâmetros de filtragem da textura
    //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    
    // Define os parâmetros de repetição da textura
    //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    
    // Carrega os dados da imagem na textura
    glTexImage2D(GL_TEXTURE_2D, 0, tex->format, tex->width, tex->height, 0, tex->format, GL_UNSIGNED_BYTE, tex->texels);
    
    // Libera a memória dos dados da imagem
    free(row_pointers);
    
    // Libera as estruturas da libpng
    png_destroy_read_struct(&png, &info, NULL);

    return tex->id;
}


unsigned int loadbmp(FILE* File, bool delayed)
{
    unsigned int tex = (unsigned int)malloc(sizeof(void));

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
static void  _ps2_load_JPEG_generic(void *Texture, struct jpeg_decompress_struct *cinfo, struct my_error_mgr *jerr, bool scale_down)
{

}

unsigned int loadjpeg(FILE* fp, bool scale_down, bool delayed)
{

	
    unsigned int tex = (unsigned int)malloc(sizeof(void));

	return tex;

}

unsigned int load_image(const char* path, bool delayed){
	FILE* file = fopen(path, "rb");
	uint16_t magic;
	fread(&magic, 1, 2, file);
	fseek(file, 0, SEEK_SET);
	unsigned int image;
	if (magic == 0x4D42) image =      loadbmp(file, delayed);
	else if (magic == 0xD8FF) image = loadjpeg(file, false, delayed);
	else if (magic == 0x5089) image = loadpng(file);
    else image = loadraw(file);
	if (image == -1) printf("Failed to load image %s.", path);

	return image;
}

void gsKit_clear_screens()
{

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


void drawImageCentered(unsigned int source, float x, float y, float width, float height, float startx, float starty, float endx, float endy, Color color)
{


}

void drawImage(unsigned int source, float x, float y, float width, float height, float startx, float starty, float endx, float endy, Color color)
{
    glBindTexture(GL_TEXTURE_2D, source);
    glEnable(GL_TEXTURE_2D);
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);

    glDisable(GL_LIGHTING);
    glEnable(GL_BLEND);

    glColor3f(R(color)/255.0f, G(color)/255.0f, B(color)/255.0f); //blue colors

    glBegin(GL_QUADS);//start drawing triangles
	
    glTexCoord2f(0.0f, 0.0f);  // canto inferior esquerdo
    glVertex2f(x,  y);//triangle one first vertex

    glTexCoord2f(1.0f, 0.0f);  // canto inferior direito
    glVertex2f(x+width, y);//triangle one third vertex

    glTexCoord2f(1.0f, 1.0f);  // canto superior direito
    glVertex2f(x+width, y+height);//triangle one third vertex

    glTexCoord2f(0.0f, 1.0f);  // canto superior esquerdo
    glVertex2f(x, y+height);//triangle one second vertex

    glEnd();//end drawing of triangles

    glDisable(GL_TEXTURE_2D);

    glDisable(GL_BLEND);
    glEnable(GL_LIGHTING);
}


void drawImageRotate(unsigned int source, float x, float y, float width, float height, float startx, float starty, float endx, float endy, float angle, Color color){

}

void drawPixel(float x, float y, Color color)
{
    glColor3f(R(color)/255.0f, G(color)/255.0f, B(color)/255.0f); //blue color

    glBegin(GL_POINTS);//start drawing triangles
	
    glVertex2f(x,  y);//triangle one first vertex

    glEnd();//end drawing of triangles

	glFlush();
}

void drawLine(float x, float y, float x2, float y2, Color color)
{
    glColor3f(R(color)/255.0f, G(color)/255.0f, B(color)/255.0f); //blue color

    glBegin(GL_LINES);//start drawing triangles
	
    glVertex2f(x,  y);//triangle one first vertex
    glVertex2f(x2,  y2);//triangle one first vertex

    glEnd();//end drawing of triangles

	glFlush();
}


void drawRect(float x, float y, int width, int height, Color color)
{
    glBegin(GL_QUADS);//start drawing triangles
	
    glColor3f(R(color)/255.0f, G(color)/255.0f, B(color)/255.0f); //blue color
    glVertex2f(x,  y);//triangle one first vertex
    glColor3f(R(color)/255.0f, G(color)/255.0f, B(color)/255.0f); //blue color
    glVertex2f(x+width, y);//triangle one third vertex
    glColor3f(R(color)/255.0f, G(color)/255.0f, B(color)/255.0f); //blue color
    glVertex2f(x+width, y+height);//triangle one third vertex
    glColor3f(R(color)/255.0f, G(color)/255.0f, B(color)/255.0f); //blue color
    glVertex2f(x, y+height);//triangle one second vertex

    glEnd();//end drawing of triangles

	glFlush();
}

void drawRectCentered(float x, float y, int width, int height, Color color)
{

}

void drawTriangle(float x, float y, float x2, float y2, float x3, float y3, Color color)
{
	glColor3f(R(color)/255.0f, G(color)/255.0f, B(color)/255.0f); //blue color

    glBegin(GL_TRIANGLES);//start drawing triangles
    glVertex2f(x,  y);//triangle one first vertex
    glVertex2f(x2, y2);//triangle one second vertex
    glVertex2f(x3, y3);//triangle one third vertex
    glEnd();//end drawing of triangles

	glFlush();

}

void drawTriangle_gouraud(float x, float y, float x2, float y2, float x3, float y3, Color color, Color color2, Color color3)
{

    glBegin(GL_TRIANGLES);//start drawing triangles

	glColor3f(R(color)/255.0f, G(color)/255.0f, B(color)/255.0f); //blue color
    glVertex2f(x ,  y);//triangle one first vertex

	glColor3f(R(color2)/255.0f, G(color2)/255.0f, B(color2)/255.0f); //blue color
    glVertex2f(x2, y2);//triangle one second vertex

	glColor3f(R(color3)/255.0f, G(color3)/255.0f, B(color3)/255.0f); //blue color
    glVertex2f(x3, y3);//triangle one third vertex

    glEnd();//end drawing of triangles

	glFlush();
}

void drawQuad(float x, float y, float x2, float y2, float x3, float y3, float x4, float y4, Color color)
{
    glBegin(GL_QUADS);//start drawing triangles
	
    glColor3f(R(color)/255.0f, G(color)/255.0f, B(color)/255.0f); //blue color
    glVertex2f(x,  y);//triangle one first vertex
    glColor3f(R(color)/255.0f, G(color)/255.0f, B(color)/255.0f); //blue color
    glVertex2f(x2, y2);//triangle one third vertex
    glColor3f(R(color)/255.0f, G(color)/255.0f, B(color)/255.0f); //blue color
    glVertex2f(x4, y4);//triangle one second vertex
    glColor3f(R(color)/255.0f, G(color)/255.0f, B(color)/255.0f); //blue color
    glVertex2f(x3, y3);//triangle one third vertex

    glEnd();//end drawing of triangles

	glFlush();
}

void drawQuad_gouraud(float x, float y, float x2, float y2, float x3, float y3, float x4, float y4, Color color, Color color2, Color color3, Color color4)
{
    glBegin(GL_QUADS);//start drawing triangles
	
    glColor3f(R(color)/255.0f, G(color)/255.0f, B(color)/255.0f); //blue color
    glVertex2f(x,  y);//triangle one first vertex

    glColor3f(R(color2)/255.0f, G(color2)/255.0f, B(color2)/255.0f); //blue color
    glVertex2f(x2, y2);//triangle one third vertex

    glColor3f(R(color4)/255.0f, G(color4)/255.0f, B(color4)/255.0f); //blue color
    glVertex2f(x4, y4);//triangle one second vertex

    glColor3f(R(color3)/255.0f, G(color3)/255.0f, B(color3)/255.0f); //blue color
    glVertex2f(x3, y3);//triangle one third vertex

    glEnd();//end drawing of triangles

	glFlush();
}

void drawCircle(float x, float y, float radius, u64 color, u8 filled)
{
    glColor3f(R(color)/255.0f, G(color)/255.0f, B(color)/255.0f); //blue color

    if(filled) {
        glBegin(GL_TRIANGLE_FAN);
        glVertex2f(x, y);  // centro do círculo
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

void InvalidateTexture(void *txt)
{

}

void UnloadTexture(void *txt)
{
	
}

int GetInterlacedFrameMode()
{
    return 1;
}

void *getGSGLOBAL(){return gsGlobal;}

void setVideoMode(s16 mode, int width, int height, int psm, s16 interlace, s16 field, bool zbuffering, int psmz) {

}

void fntDrawQuad(rm_quad_t *q)
{

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
    glClearColor(0.0f, 0.0f, 0.0f, 0.5f); // Black Background
    //glClearDepth(1.0f);                   // Depth Buffer Setup
    //glEnable(GL_DEPTH_TEST);              // Enables Depth Testing
    glDepthFunc(GL_LEQUAL);               // The Type Of Depth Testing To Do
    
    glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);

    // ps2gl needs lighting + color_material for per-vertex colors
    glEnable(GL_COLOR_MATERIAL);
    glColorMaterial(GL_FRONT_AND_BACK, GL_DIFFUSE);
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
}

void initGraphics()
{
    glutInit();                        // Erm Just Write It =)
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

    pglWaitForVSync();
    pglSwapBuffers();
    pglRenderGeometry();

	pglBeginGeometry();
}

void graphicWaitVblankStart(){

}
