#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <malloc.h>
#include <math.h>
#include <fcntl.h>

#include <packet2.h>
#include <packet2_utils.h>

#include "include/graphics.h"

MATRIX view_screen;

VECTOR camera_position = { 0.00f, 0.00f, 0.00f, 1.00f };
VECTOR camera_rotation = { 0.00f, 0.00f, 0.00f, 1.00f };

int light_count;
VECTOR* light_direction;
VECTOR* light_colour;
int* light_type;


struct vertex{
	float x;
	float y;
	float z;
	float t1;
	float t2;
	float n1;
	float n2;
	float n3;
};

struct vertexList{
	vertex v1;
	vertex v2;
	vertex v3;
	vertexList* next;
};

struct rawVertexList{
	vertex* vert;
	rawVertexList* next;
};

void init3D(float aspect)
{
	create_view_screen(view_screen, aspect, -0.20f, 0.20f, -0.20f, 0.20f, 1.00f, 2000.00f);

}

void calculate_vertices_no_clip(VECTOR *output,  int count, VECTOR *vertices, MATRIX local_screen) {
	asm __volatile__ (
					  "lqc2		$vf1, 0x00(%3)	\n"
					  "lqc2		$vf2, 0x10(%3)	\n"
					  "lqc2		$vf3, 0x20(%3)	\n"
					  "lqc2		$vf4, 0x30(%3)	\n"
					  "1:					\n"
					  "lqc2		$vf6, 0x00(%2)	\n"
					  "vmulaw		$ACC, $vf4, $vf0	\n"
					  "vmaddax		$ACC, $vf1, $vf6	\n"
					  "vmadday		$ACC, $vf2, $vf6	\n"
					  "vmaddz		$vf7, $vf3, $vf6	\n"
//					  "vclipw.xyz	$vf7, $vf7	\n" // FIXME: Clip detection is still kinda broken.
					  "cfc2		$10, $18	\n"
					  "beq			$10, $0, 3f	\n"
					  "2:					\n"
   					  "sqc2		$0, 0x00(%0)	\n"
   					  "j			4f		\n"
					  "3:					\n"
					  "vdiv		$Q, $vf0w, $vf7w	\n"
					  "vwaitq				\n"
					  "vmulq.xyz		$vf7, $vf7, $Q	\n"
					  "sqc2		$vf7, 0x00(%0)	\n"
					  "4:					\n"
					  "addi		%0, 0x10	\n"
					  "addi		%2, 0x10	\n"
					  "addi		%1, -1		\n"
					  "bne		$0, %1, 1b	\n"
					  : : "r" (output), "r" (count), "r" (vertices), "r" (local_screen) : "$10", "memory"
					  );
}


typedef union TexCoord { 
    struct {
        float s, t;
    };
    u64 word;
} __attribute__((packed, aligned(8))) TexCoord;

#define GIF_TAG_TRIANGLE_GORAUD_TEXTURED_ST_REGS(ctx) \
    ((u64)(GS_TEX0_1 + ctx) << 0 ) | \
    ((u64)(GS_PRIM)         << 4 ) | \
    ((u64)(GS_RGBAQ)        << 8 ) | \
    ((u64)(GS_ST)           << 12) | \
    ((u64)(GS_XYZ2)         << 16) | \
    ((u64)(GS_RGBAQ)        << 20) | \
    ((u64)(GS_ST)           << 24) | \
    ((u64)(GS_XYZ2)         << 28) | \
    ((u64)(GS_RGBAQ)        << 32) | \
    ((u64)(GS_ST)           << 36) | \
    ((u64)(GS_XYZ2)         << 40) | \
    ((u64)(GIF_NOP)         << 44)


static inline u32 lzw(u32 val) {
    u32 res;
    __asm__ __volatile__ ("   plzcw   %0, %1    " : "=r" (res) : "r" (val));
    return(res);
}

static inline void gsKit_set_tw_th(const GSTEXTURE *Texture, int *tw, int *th) {
    *tw = 31 - (lzw(Texture->Width) + 1);
    if(Texture->Width > (1<<*tw))
        (*tw)++;

    *th = 31 - (lzw(Texture->Height) + 1);
    if(Texture->Height > (1<<*th))
        (*th)++;
}

static void gsKit_prim_triangle_goraud_texture_3d_st(
    GSGLOBAL *gsGlobal, GSTEXTURE *Texture,
    float x1, float y1, int iz1, float u1, float v1,
    float x2, float y2, int iz2, float u2, float v2,
    float x3, float y3, int iz3, float u3, float v3,
    u64 color1, u64 color2, u64 color3
) {
    gsKit_set_texfilter(gsGlobal, Texture->Filter);
    u64* p_store;
    u64* p_data;
    const int qsize = 6;
    const int bsize = 96;

    int tw, th;
    gsKit_set_tw_th(Texture, &tw, &th);

    int ix1 = gsKit_float_to_int_x(gsGlobal, x1);
    int ix2 = gsKit_float_to_int_x(gsGlobal, x2);
    int ix3 = gsKit_float_to_int_x(gsGlobal, x3);
    int iy1 = gsKit_float_to_int_y(gsGlobal, y1);
    int iy2 = gsKit_float_to_int_y(gsGlobal, y2);
    int iy3 = gsKit_float_to_int_y(gsGlobal, y3);
 
    TexCoord st1 = (TexCoord) { { u1, v1 } };
    TexCoord st2 = (TexCoord) { { u2, v2 } };
    TexCoord st3 = (TexCoord) { { u3, v3 } };

    p_store = p_data = (u64*)gsKit_heap_alloc(gsGlobal, qsize, bsize, GSKIT_GIF_PRIM_TRIANGLE_TEXTURED);

    *p_data++ = GIF_TAG_TRIANGLE_GORAUD_TEXTURED(0);
    *p_data++ = GIF_TAG_TRIANGLE_GORAUD_TEXTURED_ST_REGS(gsGlobal->PrimContext);

    const int replace = 0; // cur_shader->tex_mode == TEXMODE_REPLACE;
    const int alpha = gsGlobal->PrimAlphaEnable;

    if (Texture->VramClut == 0) {
        *p_data++ = GS_SETREG_TEX0(Texture->Vram/256, Texture->TBW, Texture->PSM,
            tw, th, alpha, replace,
            0, 0, 0, 0, GS_CLUT_STOREMODE_NOLOAD);
    } else {
        *p_data++ = GS_SETREG_TEX0(Texture->Vram/256, Texture->TBW, Texture->PSM,
            tw, th, alpha, replace,
            Texture->VramClut/256, Texture->ClutPSM, 0, 0, GS_CLUT_STOREMODE_LOAD);
    }

    *p_data++ = GS_SETREG_PRIM( GS_PRIM_PRIM_TRIANGLE, 1, 1, gsGlobal->PrimFogEnable,
                gsGlobal->PrimAlphaEnable, gsGlobal->PrimAAEnable,
                0, gsGlobal->PrimContext, 0);


    *p_data++ = color1;
    *p_data++ = st1.word;
    *p_data++ = GS_SETREG_XYZ2( ix1, iy1, iz1 );

    *p_data++ = color2;
    *p_data++ = st2.word;
    *p_data++ = GS_SETREG_XYZ2( ix2, iy2, iz2 );

    *p_data++ = color3;
    *p_data++ = st3.word;
    *p_data++ = GS_SETREG_XYZ2( ix3, iy3, iz3 );
}

int draw_convert_rgbq(color_t *output, int count, vertex_f_t *vertices, color_f_t *colours, unsigned char alpha)
{

	int i;
	float q = 1.00f;

	// For each colour...
	for (i=0;i<count;i++)
	{

		// Calculate the Q value.
		if (vertices[i].w != 0)
		{

			q = 1 / vertices[i].w;

		}

		// Calculate the RGBA values.
		output[i].r = (int)(colours[i].r * 128.0f);
		output[i].g = (int)(colours[i].g * 128.0f);
		output[i].b = (int)(colours[i].b * 128.0f);
		output[i].a = alpha;
		output[i].q = q;

	}

	// End function.
	return 0;

}

int draw_convert_rgbaq(color_t *output, int count, vertex_f_t *vertices, color_f_t *colours)
{

	int i;
	float q = 1.00f;

	// For each colour...
	for (i=0;i<count;i++)
	{

		// Calculate the Q value.
		if (vertices[i].w != 0)
		{

			q = 1 / vertices[i].w;

		}

		// Calculate the RGBA values.
		output[i].r = (int)(colours[i].r * 128.0f);
		output[i].g = (int)(colours[i].g * 128.0f);
		output[i].b = (int)(colours[i].b * 128.0f);
		output[i].a = (int)(colours[i].a * 128.0f);
		output[i].q = q;

	}

	// End function.
	return 0;

}

int draw_convert_st(texel_t *output, int count, vertex_f_t *vertices, texel_f_t *coords)
{

	int i = 0;
	float q = 1.00f;

	// For each coordinate...
	for (i=0;i<count;i++)
	{

		// Calculate the Q value.
		if (vertices[i].w != 0)
		{
			q = 1 / vertices[i].w;
		}

		// Calculate the S and T values.
		output[i].s = coords[i].s * q;
		output[i].t = coords[i].t * q;

	}

	// End function.
	return 0;

}

int draw_convert_xyz(xyz_t *output, float x, float y, int z, int count, vertex_f_t *vertices)
{

	int i;

	int center_x;
	int center_y;

	unsigned int max_z;

	center_x = ftoi4(x);
	center_y = ftoi4(y);

	max_z = 1 << (z - 1);

	// For each colour...
	for (i=0;i<count;i++)
	{

		// Calculate the XYZ values.
		output[i].x = (short)((vertices[i].x + 1.0f) * center_x);
		output[i].y = (short)((vertices[i].y + 1.0f) * -center_y);
		output[i].z = (unsigned int)((vertices[i].z + 1.0f) * max_z);

	}

	// End function.
	return 0;

}

void setCameraPosition(float x, float y, float z){
	camera_position[0] = x;
	camera_position[1] = y;
	camera_position[2] = z;
	camera_position[3] = 1.00f;
}

void setCameraRotation(float x, float y, float z){
	camera_rotation[0] = x;
	camera_rotation[1] = y;
	camera_rotation[2] = z;
	camera_rotation[3] = 1.00f;
}

void setLightQuantity(int quantity){
	light_count = quantity;
	light_direction = (VECTOR*)memalign(128, sizeof(VECTOR) * light_count);
	light_colour = (VECTOR*)memalign(128, sizeof(VECTOR) * light_count);
	light_type = (int*)memalign(128, sizeof(int) * light_count);
}

void createLight(int lightid, float dir_x, float dir_y, float dir_z, int type, float r, float g, float b){
	light_direction[lightid-1][0] = dir_x;
	light_direction[lightid-1][1] = dir_y;
	light_direction[lightid-1][2] = dir_z;
	light_direction[lightid-1][3] = 1.00f;

	light_colour[lightid-1][0] = r;
	light_colour[lightid-1][1] = g;
	light_colour[lightid-1][2] = b;
	light_colour[lightid-1][3] = 1.00f;

	light_type[lightid-1] = type;

}

model* loadOBJ(const char* path, GSTEXTURE* text){
    // Opening model file and loading it on RAM
	int file = open(path, O_RDONLY, 0777);
	uint32_t size = lseek(file, 0, SEEK_END);
	lseek(file, 0, SEEK_SET);
	char* content = (char*)malloc(size+1);
	read(file, content, size);
	content[size] = 0;
	
	// Closing file
	close(file);
	
	// Creating temp vertexList
	rawVertexList* vl = (rawVertexList*)malloc(sizeof(rawVertexList));
	rawVertexList* init = vl;
	
	// Init variables
	char* str = content;
	char* ptr = strstr(str,"v ");
	int idx;
	char float_val[16];
	char* init_val;
	char magics[3][3] = {"v ","vt","vn"};
	int magics_idx = 0;
	vertex* res;
	int v_idx = 0;
	bool skip = false;
	char* end_vert;
	char* end_val;
	float* vert_args;
	rawVertexList* old_vl;
	
	// Vertices extraction
	for(;;){
		
		// Check if a magic change is needed
		while (ptr == NULL){
			if (magics_idx < 2){
				res = init->vert;
				vl = init;
				magics_idx++;
				ptr = strstr(str,magics[magics_idx]);
			}else{
				skip = true;
				break;
			}
		}
		if (skip) break;
		
		// Extract vertex
		if (magics_idx == 0) idx = 0;
		else if (magics_idx == 1) idx = 3;
		else idx = 5;
		if (magics_idx == 0) init_val = ptr + 2;
		else init_val = ptr + 3;
		while (init_val[0] == ' ') init_val++;
		end_vert = strstr(init_val,"\n");
		if (magics_idx == 0) res = (vertex*)malloc(sizeof(vertex));
		end_val = strstr(init_val," ");
		vert_args = (float*)res; // Hacky way to iterate in vertex struct		
		while (init_val < end_vert){
			if (end_val > end_vert) end_val = end_vert;
			strncpy(float_val, init_val, end_val - init_val);
			float_val[end_val - init_val] = 0;
			vert_args[idx] = atof(float_val);
			idx++;
			init_val = end_val + 1;
			while (init_val[0] == ' ') init_val++;
			end_val = strstr(init_val," ");
		}
		
		// Update rawVertexList struct
		if (magics_idx == 0){
			vl->vert = res;
			vl->next = (rawVertexList*)malloc(sizeof(rawVertexList));
		}
		old_vl = vl;
		vl = vl->next;
		if (magics_idx == 0){
			vl->vert = NULL;
			vl->next = NULL;
		}else{
			if (vl == NULL){
				old_vl->next = (rawVertexList*)malloc(sizeof(rawVertexList));
				vl = old_vl->next;
				vl->vert = (vertex*)malloc(sizeof(vertex));
				vl->next = NULL;
			}else if(vl->vert == NULL) vl->vert = (vertex*)malloc(sizeof(vertex));
			res = vl->vert;
		}
		
		// Searching for next vertex
		str = ptr + 1;
		ptr = strstr(str,magics[magics_idx]);
		
	}

	// Creating real RAW vertexList
	ptr = strstr(str, "f ");
	rawVertexList* faces = (rawVertexList*)malloc(sizeof(rawVertexList));
	rawVertexList* initFaces = faces;
	faces->vert = NULL;
	faces->next = NULL;
	int len = 0;
	char val[8];
	int f_idx;
	char* ptr2;
	int t_idx;
	rawVertexList* tmp;
	
	// Faces extraction
	while (ptr != NULL){
		
		// Skipping padding
		ptr+=2;		
		
		// Extracting face info
		f_idx = 0;
		while (f_idx < 3){
		
			// Allocating new vertex
			faces->vert = (vertex*)malloc(sizeof(vertex));
		
			// Extracting x,y,z
			ptr2 = strstr(ptr,"/");
			strncpy(val,ptr,ptr2-ptr);
			val[ptr2-ptr] = 0;
			v_idx = atoi(val);
			t_idx = 1;
			tmp = init;
			while (t_idx < v_idx){
				tmp = tmp->next;
				t_idx++;
			}
			faces->vert->x = tmp->vert->x;
			faces->vert->y = tmp->vert->y;
			faces->vert->z = tmp->vert->z;
			
			// Extracting texture info
			ptr = ptr2+1;
			ptr2 = strstr(ptr,"/");
			if (ptr2 != ptr){
				strncpy(val,ptr,ptr2-ptr);
				val[ptr2-ptr] = 0;
				v_idx = atoi(val);
				t_idx = 1;
				tmp = init;
				while (t_idx < v_idx){
					tmp = tmp->next;
					t_idx++;
				}
				faces->vert->t1 = tmp->vert->t1;
				faces->vert->t2 = 1.0f - tmp->vert->t2;
			}else{
				faces->vert->t1 = 0.0f;
				faces->vert->t2 = 0.0f;
			}
			
			// Extracting normals info
			ptr = ptr2+1;
			if (f_idx < 2) ptr2 = strstr(ptr," ");
			else{
				ptr2 = strstr(ptr,"\n");
				if (ptr2 == NULL) ptr2 = content + size;
			}
			strncpy(val,ptr,ptr2-ptr);
			val[ptr2-ptr] = 0;
			v_idx = atoi(val);
			t_idx = 1;
			tmp = init;
			while (t_idx < v_idx){
				tmp = tmp->next;
				t_idx++;
			}
			faces->vert->n1 = tmp->vert->n1;
			faces->vert->n2 = tmp->vert->n2;
			faces->vert->n3 = tmp->vert->n3;

			// Setting values for next vertex
			ptr = ptr2;
			faces->next = (rawVertexList*)malloc(sizeof(rawVertexList));
			faces = faces->next;
			faces->next = NULL;
			faces->vert = NULL;
			len++;
			f_idx++;
		}
		
		ptr = strstr(ptr,"f ");
		
	}
	
	// Freeing temp vertexList and allocated file
	free(content);
	rawVertexList* tmp_init;
	while (init != NULL){
		tmp_init = init;
		free(init->vert);
		init = init->next;
		free(tmp_init);
	}
	
	// Create the model struct and populating vertex list
	model* res_m = (model*)malloc(sizeof(model));
	vertexList* vlist = (vertexList*)malloc(sizeof(vertexList));
	vertexList* vlist_start = vlist;
	vlist->next = NULL;
	bool first = true;
	for(int i = 0; i < len; i+=3) {
		if (first) first = false;
		else{
			vlist->next = (vertexList*)malloc(sizeof(vertexList));
			vlist = vlist->next;
			vlist->next = NULL;
		}
		tmp_init = initFaces;
		memcpy(&vlist->v1,initFaces->vert,sizeof(vertex));
		initFaces = initFaces->next;
		free(tmp_init->vert);
		free(tmp_init);
		tmp_init = initFaces;
		memcpy(&vlist->v2,initFaces->vert,sizeof(vertex));
		initFaces = initFaces->next;
		free(tmp_init->vert);
		free(tmp_init);
		tmp_init = initFaces;
		memcpy(&vlist->v3,initFaces->vert,sizeof(vertex));
		initFaces = initFaces->next;
		free(tmp_init->vert);
		free(tmp_init);
	}
	res_m->facesCount = len / 3;
  
	
	// Setting texture
	res_m->texture = text;

	vlist = vlist_start;
	res_m->positions = (VECTOR*)memalign(128, res_m->facesCount * 3 * sizeof(VECTOR));
    res_m->texcoords = (VECTOR*)memalign(128, res_m->facesCount * 3 * sizeof(VECTOR));
    res_m->normals =  (VECTOR*)memalign(128, res_m->facesCount * 3 * sizeof(VECTOR));
	res_m->idxList = (uint16_t*)memalign(128, res_m->facesCount * 3 * sizeof(uint16_t));
	vertexList* object = vlist;
	int n = 0;
	while (object != NULL){

        //v1
        res_m->positions[n][0] = object->v1.x;
		res_m->positions[n][1] = object->v1.y;
		res_m->positions[n][2] = object->v1.z;
		res_m->positions[n][3] = 1.000f;

        res_m->normals[n][0] = object->v1.n1;
		res_m->normals[n][1] = object->v1.n2;
		res_m->normals[n][2] = object->v1.n3;
		res_m->normals[n][3] = 1.000f;

        res_m->texcoords[n][0] = object->v1.t1;
		res_m->texcoords[n][1] = object->v1.t2;
		res_m->texcoords[n][2] = 0.000f;
		res_m->texcoords[n][3] = 0.000f;

        //v2
        res_m->positions[n+1][0] = object->v2.x;
		res_m->positions[n+1][1] = object->v2.y;
		res_m->positions[n+1][2] = object->v2.z;
		res_m->positions[n+1][3] = 1.000f;

        res_m->normals[n+1][0] = object->v2.n1;
		res_m->normals[n+1][1] = object->v2.n2;
		res_m->normals[n+1][2] = object->v2.n3;
		res_m->normals[n+1][3] = 1.000f;

        res_m->texcoords[n+1][0] = object->v2.t1;
		res_m->texcoords[n+1][1] = object->v2.t2;
		res_m->texcoords[n+1][2] = 0.000f;
		res_m->texcoords[n+1][3] = 0.000f;

        //v3
        res_m->positions[n+2][0] = object->v3.x;
		res_m->positions[n+2][1] = object->v3.y;
		res_m->positions[n+2][2] = object->v3.z;
		res_m->positions[n+2][3] = 1.000f;

        res_m->normals[n+2][0] = object->v3.n1;
		res_m->normals[n+2][1] = object->v3.n2;
		res_m->normals[n+2][2] = object->v3.n3;
		res_m->normals[n+2][3] = 1.000f;

        res_m->texcoords[n+2][0] = object->v3.t1;
		res_m->texcoords[n+2][1] = object->v3.t2;
		res_m->texcoords[n+2][2] = 0.000f;
		res_m->texcoords[n+2][3] = 0.000f;

		res_m->idxList[n] = n;
		res_m->idxList[n+1] = n+1;
		res_m->idxList[n+2] = n+2;
		object = object->next;
		n += 3;
	}

	while (vlist != NULL){
		vertexList* old = vlist;
		vlist = vlist->next;
		free(old);
	}

    return res_m;
}


void drawOBJ(model* m, float pos_x, float pos_y, float pos_z, float rot_x, float rot_y, float rot_z)
{
	
	VECTOR object_position = { pos_x, pos_y, pos_z, 1.00f };
	VECTOR object_rotation = { rot_x, rot_y, rot_z, 1.00f };

	int i;
    GSGLOBAL *gsGlobal = getGSGLOBAL();

	// Matrices to setup the 3D environment and camera
	MATRIX local_world;
	MATRIX local_light;
	MATRIX world_view;
	MATRIX local_screen;

	// Allocate calculation space.
	VECTOR *temp_normals  = (VECTOR     *)memalign(128, sizeof(VECTOR) * (m->facesCount*3));
	VECTOR *temp_lights   = (VECTOR     *)memalign(128, sizeof(VECTOR) *  (m->facesCount*3));
	color_f_t *temp_colours  = (color_f_t  *)memalign(128, sizeof(color_f_t)  *  (m->facesCount*3));
	vertex_f_t *temp_vertices = (vertex_f_t *)memalign(128, sizeof(vertex_f_t) *  (m->facesCount*3));


	//gsGlobal->PrimAlphaEnable = GS_SETTING_OFF;
	//gsKit_set_test(gsGlobal, GS_ATEST_OFF);
	gsGlobal->PrimAAEnable = GS_SETTING_ON;
	gsKit_set_test(gsGlobal, GS_ZTEST_ON);

	// Create the local_world matrix.
	create_local_world(local_world, object_position, object_rotation);

	// Create the local_light matrix.
	create_local_light(local_light, object_rotation);

	// Create the world_view matrix.
	create_world_view(world_view, camera_position, camera_rotation);

	// Create the local_screen matrix.
	create_local_screen(local_screen, local_world, world_view, view_screen);

	// Calculate the normal values.
	calculate_normals(temp_normals,  (m->facesCount*3), m->normals, local_light);
	
	// Calculate the lighting values.
	calculate_lights(temp_lights,  (m->facesCount*3), temp_normals, light_direction, light_colour, light_type, light_count);

	// Calculate the colour values after lighting.
	//calculate_colours((VECTOR *)temp_colours, m->facesCount, m->colours, temp_lights);

	// Calculate the vertex values.
	calculate_vertices_no_clip((VECTOR *)temp_vertices,  (m->facesCount*3), m->positions, local_screen);

	// Convert floating point vertices to fixed point and translate to center of screen.
	xyz_t   *verts  = (xyz_t   *)memalign(128, sizeof(xyz_t)   *  (m->facesCount*3));
	color_t *colors = (color_t *)memalign(128, sizeof(color_t) *  (m->facesCount*3));
	texel_t *tex = (texel_t *)memalign(128, sizeof(texel_t) *  (m->facesCount*3));
	
	draw_convert_xyz(verts, 2048, 2048, 16,  (m->facesCount*3), temp_vertices);
	draw_convert_rgbq(colors,  (m->facesCount*3), temp_vertices, (color_f_t*)temp_lights, 0x80);
	draw_convert_st(tex,  (m->facesCount*3), temp_vertices, (texel_f_t *)m->texcoords);


	float fX=gsGlobal->Width/2;
	float fY=gsGlobal->Height/2;

	if (m->texture != NULL) gsKit_TexManager_bind(gsGlobal, m->texture);

	for (i = 0; i < (m->facesCount*3); i+=3) {

		//Backface culling
		float orientation = (temp_vertices[m->idxList[i+1]].x - temp_vertices[m->idxList[i]].x) * (temp_vertices[m->idxList[i+2]].y - temp_vertices[m->idxList[i]].y) - (temp_vertices[m->idxList[i+1]].y - temp_vertices[m->idxList[i]].y) * (temp_vertices[m->idxList[i+2]].x - temp_vertices[m->idxList[i]].x);
		if(orientation < 0.0) {
			continue;
		}
		
		// Basic clipping
		if(temp_vertices[m->idxList[i]].z < -1.0 || temp_vertices[m->idxList[i]].z > 0 || temp_vertices[m->idxList[i]].x > 1.0 || temp_vertices[m->idxList[i]].x < -1.0 || temp_vertices[m->idxList[i]].y > 1.0 || temp_vertices[m->idxList[i]].y < -1.0){
			continue;
		}
		if(temp_vertices[m->idxList[i+1]].z < -1.0 || temp_vertices[m->idxList[i+1]].z > 0 || temp_vertices[m->idxList[i+1]].x > 1.0 || temp_vertices[m->idxList[i+1]].x < -1.0 || temp_vertices[m->idxList[i+1]].y > 1.0 || temp_vertices[m->idxList[i+1]].y < -1.0){
			continue;
		}
		if(temp_vertices[m->idxList[i+2]].z < -1.0 || temp_vertices[m->idxList[i+2]].z > 0 || temp_vertices[m->idxList[i+2]].x > 1.0 || temp_vertices[m->idxList[i+2]].x < -1.0 || temp_vertices[m->idxList[i+2]].y > 1.0 || temp_vertices[m->idxList[i+2]].y < -1.0){
			continue;
		}

		if (m->texture == NULL){
			gsKit_prim_triangle_gouraud_3d(gsGlobal
				, (temp_vertices[m->idxList[i]].x + 1.0f) * fX, (temp_vertices[m->idxList[i]].y + 1.0f) * fY, verts[m->idxList[i]].z
				, (temp_vertices[m->idxList[i+1]].x + 1.0f) * fX, (temp_vertices[m->idxList[i+1]].y + 1.0f) * fY, verts[m->idxList[i+1]].z
				, (temp_vertices[m->idxList[i+2]].x + 1.0f) * fX, (temp_vertices[m->idxList[i+2]].y + 1.0f) * fY, verts[m->idxList[i+2]].z
				, colors[m->idxList[i]].rgbaq, colors[m->idxList[i+1]].rgbaq, colors[m->idxList[i+2]].rgbaq);
		} else {
			gsKit_prim_triangle_goraud_texture_3d_st(gsGlobal, m->texture,
					(temp_vertices[m->idxList[i]].x + 1.0f) * fX, (temp_vertices[m->idxList[i]].y + 1.0f) * fY, verts[m->idxList[i]].z, tex[m->idxList[i]].s, tex[m->idxList[i]].t,
					(temp_vertices[m->idxList[i+1]].x + 1.0f) * fX, (temp_vertices[m->idxList[i+1]].y + 1.0f) * fY, verts[m->idxList[i+1]].z, tex[m->idxList[i+1]].s, tex[m->idxList[i+1]].t,
					(temp_vertices[m->idxList[i+2]].x + 1.0f) * fX, (temp_vertices[m->idxList[i+2]].y + 1.0f) * fY, verts[m->idxList[i+2]].z, tex[m->idxList[i+2]].s, tex[m->idxList[i+2]].t,
					colors[m->idxList[i]].rgbaq, colors[m->idxList[i+1]].rgbaq, colors[m->idxList[i+2]].rgbaq);
		}

	}
	
	free(temp_normals);
	free(temp_lights);
	free(temp_colours);
	free(temp_vertices);
	free(verts);
	free(colors);
	free(tex);

}