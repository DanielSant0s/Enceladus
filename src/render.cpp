#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <malloc.h>
#include <math.h>
#include <fcntl.h>
#include <cstring>

extern "C" {
	#include "fast_obj.h"
}

#include "include/render.h"
/*
MATRIX view_screen;

VECTOR camera_position = { 0.00f, 0.00f, 0.00f, 1.00f };
VECTOR camera_rotation = { 0.00f, 0.00f, 0.00f, 1.00f };

int light_count;
VECTOR* light_direction;
VECTOR* light_colour;
int* light_type;

*/

void perspective(float fov, float aspect, float nearClip, float farClip)
{
    float w, h;
    fov *= 3.141592654f / 180.0f;
    h = 2.0f * nearClip * (float)tanf(fov / 2.0f);
    w = h * aspect;

    glFrustum(-w / 2.0f, w / 2.0f, -h / 2.0f, h / 2.0f, nearClip, farClip);
}

void viewport_3d(float fov, float aspect, float nearClip, float farClip) {
	glViewport(0, 0, 640, 448);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    perspective(fov, aspect, nearClip, farClip);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}


void init3D(float aspect)
{
	//create_view_screen(view_screen, aspect, -0.20f, 0.20f, -0.20f, 0.20f, 1.00f, 2000.00f);
	glEnable(GL_DEPTH_TEST);
    glEnable(GL_RESCALE_NORMAL);

    // lighting
    GLfloat ambient[] = { 0.2, 0.2, 0.2, 1.0 };

    GLfloat l0_position[] = { 0, 1.0, 1.0, 0.0 };
    GLfloat l0_diffuse[]  = { 1.0f, 1.0f, 1.0f, 0 };

    GLfloat l1_position[] = { 0.0, -20.0, -80.0, 1.0 };
    // GLfloat l1_position[] = {0, -1, 1, 0.0};
    GLfloat l1_diffuse[] = { .6f, .6f, .6f, 0.0f };

    GLfloat black[] = { 0, 0, 0, 0 };

    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glEnable(GL_LIGHT1);

    glLightfv(GL_LIGHT0, GL_AMBIENT, ambient);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, l0_diffuse);
    glLightfv(GL_LIGHT0, GL_SPECULAR, l0_diffuse);
    glLightfv(GL_LIGHT0, GL_POSITION, l0_position);

    glLightfv(GL_LIGHT1, GL_AMBIENT, black);
    glLightfv(GL_LIGHT1, GL_DIFFUSE, l1_diffuse);
    glLightfv(GL_LIGHT1, GL_SPECULAR, black);
    glLightfv(GL_LIGHT1, GL_POSITION, l1_position);

    glLightf(GL_LIGHT1, GL_CONSTANT_ATTENUATION, 0.0f);
    glLightf(GL_LIGHT1, GL_LINEAR_ATTENUATION, 0.005f);
    glLightf(GL_LIGHT1, GL_QUADRATIC_ATTENUATION, 0.0f);

}

void setCameraPosition(float x, float y, float z){
	//camera_position[0] = x;
	//camera_position[1] = y;
	//camera_position[2] = z;
	//camera_position[3] = 1.00f;
}

void setCameraRotation(float x, float y, float z){
	//camera_rotation[0] = x;
	//camera_rotation[1] = y;
	//camera_rotation[2] = z;
	//camera_rotation[3] = 1.00f;
}

void setLightQuantity(int quantity){
	//light_count = quantity;
	//light_direction = (VECTOR*)memalign(128, sizeof(VECTOR) * light_count);
	//light_colour = (VECTOR*)memalign(128, sizeof(VECTOR) * light_count);
	//light_type = (int*)memalign(128, sizeof(int) * light_count);
}

void createLight(int lightid, float dir_x, float dir_y, float dir_z, int type, float r, float g, float b){
	//light_direction[lightid-1][0] = dir_x;
	//light_direction[lightid-1][1] = dir_y;
	//light_direction[lightid-1][2] = dir_z;
	//light_direction[lightid-1][3] = 1.00f;

	//light_colour[lightid-1][0] = r;
	//light_colour[lightid-1][1] = g;
	//light_colour[lightid-1][2] = b;
	//light_colour[lightid-1][3] = 1.00f;

	//light_type[lightid-1] = type;
}

model* loadOBJ(const char* path, gl_texture_t* text){
	model* res_m = (model*)malloc(sizeof(model));
    fastObjMesh* m = fast_obj_read(path);

	res_m->texture = text;
	res_m->indexCount = m->index_count;
	res_m->materialCount = m->material_count;
	res_m->positions = (float*)malloc(sizeof(float) * res_m->indexCount * 3);
	res_m->colours =   (float*)malloc(sizeof(float) * res_m->indexCount * 3);
	res_m->normals =   (float*)malloc(sizeof(float) * res_m->indexCount * 3);
	res_m->texcoords = (float*)malloc(sizeof(float) * res_m->indexCount * 2);

	res_m->materials = (material*)malloc(sizeof(material) * res_m->materialCount);

	for(int i = 0; i < res_m->materialCount; i++) {
		res_m->materials[i].Ka[0] = m->materials[i].Ka[0];
		res_m->materials[i].Ka[1] = m->materials[i].Ka[1];
		res_m->materials[i].Ka[2] = m->materials[i].Ka[2];
		res_m->materials[i].Ka[3] = 1.0f;

		res_m->materials[i].Kd[0] = m->materials[i].Kd[0];
		res_m->materials[i].Kd[1] = m->materials[i].Kd[1];
		res_m->materials[i].Kd[2] = m->materials[i].Kd[2];
		res_m->materials[i].Kd[3] = 1.0f;

		res_m->materials[i].Ks[0] = m->materials[i].Ks[0];
		res_m->materials[i].Ks[1] = m->materials[i].Ks[1];
		res_m->materials[i].Ks[2] = m->materials[i].Ks[2];
		res_m->materials[i].Ks[3] = 1.0f;

		res_m->materials[i].Ke[0] = m->materials[i].Ke[0];
		res_m->materials[i].Ke[1] = m->materials[i].Ke[1];
		res_m->materials[i].Ke[2] = m->materials[i].Ke[2];
		res_m->materials[i].Ke[3] = 1.0f;

		res_m->materials[i].Kt[0] = m->materials[i].Kt[0];
		res_m->materials[i].Kt[1] = m->materials[i].Kt[1];
		res_m->materials[i].Kt[2] = m->materials[i].Kt[2];
		res_m->materials[i].Kt[3] = 1.0f;

		res_m->materials[i].Ns = m->materials[i].Ns;
		if (m->materials[i].map_Kd.name) {
			res_m->materials[i].texture = load_image(m->materials[i].map_Kd.path, false);
		} else {
			res_m->materials[i].texture = nullptr;
		}
	}

	int new_fMI = -1, old_fMI = -1, rangeCount = 0;
	for (int i = 0; i < res_m->indexCount; i++) {
		new_fMI = m->face_materials[i / 3];
		if(new_fMI != old_fMI) {
			rangeCount++;
		}
		old_fMI = new_fMI;

	}

	res_m->rangeCount = rangeCount;
	res_m->ranges = (materialRangeInfo*)malloc(res_m->rangeCount * sizeof(materialRangeInfo));

	int faceMaterialIndex = -1, oldFaceMaterialIndex = -1, j = 0;
	for (int i = 0; i < res_m->indexCount; i++) {
		// Copy positions
    	int pos_index = m->indices[i].p;
    	res_m->positions[i * 3 + 0] = m->positions[pos_index * 3 + 0];
    	res_m->positions[i * 3 + 1] = m->positions[pos_index * 3 + 1];
    	res_m->positions[i * 3 + 2] = m->positions[pos_index * 3 + 2];

    	// Copy normals
    	int normal_index = m->indices[i].n;
    	res_m->normals[i * 3 + 0] = m->normals[normal_index * 3 + 0];
    	res_m->normals[i * 3 + 1] = m->normals[normal_index * 3 + 1];
    	res_m->normals[i * 3 + 2] = m->normals[normal_index * 3 + 2];

    	// Copy texcoords
    	int texcoord_index = m->indices[i].t;
    	res_m->texcoords[i * 2 + 0] = m->texcoords[texcoord_index * 2 + 0];
    	res_m->texcoords[i * 2 + 1] = m->texcoords[texcoord_index * 2 + 1];

		if(res_m->materialCount > 0) {
			faceMaterialIndex = m->face_materials[i / 3];
			if (faceMaterialIndex != oldFaceMaterialIndex) {
				res_m->ranges[j].materialIndex = faceMaterialIndex;
				res_m->ranges[j].start = i;
				j++;
			}
			res_m->ranges[j-1].end = i;
			oldFaceMaterialIndex = faceMaterialIndex;
		}
	}

	/*
	//calculate bounding box
	float lowX, lowY, lowZ, hiX, hiY, hiZ;
    lowX = hiX = res_m->positions[res_m->idxList[0]][0];
    lowY = hiY = res_m->positions[res_m->idxList[0]][1];
    lowZ = hiZ = res_m->positions[res_m->idxList[0]][2];
    for (int i = 0; i < res_m->facesCount; i++)
    {
        if (lowX > res_m->positions[res_m->idxList[i]][0]) lowX = res_m->positions[res_m->idxList[i]][0];
        if (hiX < res_m->positions[res_m->idxList[i]][0]) hiX = res_m->positions[res_m->idxList[i]][0];
        if (lowY > res_m->positions[res_m->idxList[i]][1]) lowY = res_m->positions[res_m->idxList[i]][1];
        if (hiY < res_m->positions[res_m->idxList[i]][1]) hiY = res_m->positions[res_m->idxList[i]][1];
        if (lowZ > res_m->positions[res_m->idxList[i]][2]) lowZ = res_m->positions[res_m->idxList[i]][2];
        if (hiZ < res_m->positions[res_m->idxList[i]][2]) hiZ = res_m->positions[res_m->idxList[i]][2];
    }

	res_m->bounding_box = (VECTOR*)malloc(sizeof(VECTOR)*8);

    res_m->bounding_box[0][0] = lowX;
	res_m->bounding_box[0][1] = lowY;
	res_m->bounding_box[0][2] = lowZ;
	res_m->bounding_box[0][3] = 1.00f;

    res_m->bounding_box[1][0] = lowX;
	res_m->bounding_box[1][1] = lowY;
	res_m->bounding_box[1][2] = hiZ;
	res_m->bounding_box[1][3] = 1.00f;

    res_m->bounding_box[2][0] = lowX;
	res_m->bounding_box[2][1] = hiY;
	res_m->bounding_box[2][2] = lowZ;
	res_m->bounding_box[2][3] = 1.00f;

    res_m->bounding_box[3][0] = lowX;
	res_m->bounding_box[3][1] = hiY;
	res_m->bounding_box[3][2] = hiZ;
	res_m->bounding_box[3][3] = 1.00f;

    res_m->bounding_box[4][0] = hiX;
	res_m->bounding_box[4][1] = lowY;
	res_m->bounding_box[4][2] = lowZ;
	res_m->bounding_box[4][3] = 1.00f;

    res_m->bounding_box[5][0] = hiX;
	res_m->bounding_box[5][1] = lowY;
	res_m->bounding_box[5][2] = hiZ;
	res_m->bounding_box[5][3] = 1.00f;

    res_m->bounding_box[6][0] = hiX;
	res_m->bounding_box[6][1] = hiY;
	res_m->bounding_box[6][2] = lowZ;
	res_m->bounding_box[6][3] = 1.00f;

    res_m->bounding_box[7][0] = hiX;
	res_m->bounding_box[7][1] = hiY;
	res_m->bounding_box[7][2] = hiZ;
	res_m->bounding_box[7][3] = 1.00f;

	*/

    return res_m;
}


void draw_bbox(model* m, float pos_x, float pos_y, float pos_z, float rot_x, float rot_y, float rot_z, Color color)
{
	/*VECTOR object_position = { pos_x, pos_y, pos_z, 1.00f };
	VECTOR object_rotation = { rot_x, rot_y, rot_z, 1.00f };

    GSGLOBAL *gsGlobal = getGSGLOBAL();

	// Matrices to setup the 3D environment and camera
	MATRIX local_world;
	MATRIX world_view;
	MATRIX local_screen;

	create_local_world(local_world, object_position, object_rotation);
	create_world_view(world_view, camera_position, camera_rotation);
	create_local_screen(local_screen, local_world, world_view, view_screen);
	if(clip_bounding_box(local_screen, m->bounding_box)) return;

	vertex_f_t *t_xyz = (vertex_f_t *)memalign(128, sizeof(vertex_f_t)*8);
	calculate_vertices_no_clip((VECTOR *)t_xyz,  8, m->bounding_box, local_screen);

	xyz_t *xyz  = (xyz_t *)memalign(128, sizeof(xyz_t)*8);
	draw_convert_xyz(xyz, 2048, 2048, 16,  8, t_xyz);

	float fX=gsGlobal->Width/2;
	float fY=gsGlobal->Height/2;

	gsKit_prim_line_3d(gsGlobal, (t_xyz[0].x+1.0f)*fX, (t_xyz[0].y+1.0f)*fY, xyz[0].z, (t_xyz[1].x+1.0f)*fX, (t_xyz[1].y+1.0f)*fY, xyz[1].z, color);
	gsKit_prim_line_3d(gsGlobal, (t_xyz[1].x+1.0f)*fX, (t_xyz[1].y+1.0f)*fY, xyz[1].z, (t_xyz[3].x+1.0f)*fX, (t_xyz[3].y+1.0f)*fY, xyz[3].z, color);
	gsKit_prim_line_3d(gsGlobal, (t_xyz[2].x+1.0f)*fX, (t_xyz[2].y+1.0f)*fY, xyz[2].z, (t_xyz[3].x+1.0f)*fX, (t_xyz[3].y+1.0f)*fY, xyz[3].z, color);
	gsKit_prim_line_3d(gsGlobal, (t_xyz[0].x+1.0f)*fX, (t_xyz[0].y+1.0f)*fY, xyz[0].z, (t_xyz[2].x+1.0f)*fX, (t_xyz[2].y+1.0f)*fY, xyz[2].z, color);
	gsKit_prim_line_3d(gsGlobal, (t_xyz[4].x+1.0f)*fX, (t_xyz[4].y+1.0f)*fY, xyz[4].z, (t_xyz[5].x+1.0f)*fX, (t_xyz[5].y+1.0f)*fY, xyz[5].z, color);
	gsKit_prim_line_3d(gsGlobal, (t_xyz[5].x+1.0f)*fX, (t_xyz[5].y+1.0f)*fY, xyz[5].z, (t_xyz[7].x+1.0f)*fX, (t_xyz[7].y+1.0f)*fY, xyz[7].z, color);
	gsKit_prim_line_3d(gsGlobal, (t_xyz[6].x+1.0f)*fX, (t_xyz[6].y+1.0f)*fY, xyz[6].z, (t_xyz[7].x+1.0f)*fX, (t_xyz[7].y+1.0f)*fY, xyz[7].z, color);
	gsKit_prim_line_3d(gsGlobal, (t_xyz[4].x+1.0f)*fX, (t_xyz[4].y+1.0f)*fY, xyz[4].z, (t_xyz[6].x+1.0f)*fX, (t_xyz[6].y+1.0f)*fY, xyz[6].z, color);
	gsKit_prim_line_3d(gsGlobal, (t_xyz[0].x+1.0f)*fX, (t_xyz[0].y+1.0f)*fY, xyz[0].z, (t_xyz[4].x+1.0f)*fX, (t_xyz[4].y+1.0f)*fY, xyz[4].z, color);
	gsKit_prim_line_3d(gsGlobal, (t_xyz[1].x+1.0f)*fX, (t_xyz[1].y+1.0f)*fY, xyz[1].z, (t_xyz[5].x+1.0f)*fX, (t_xyz[5].y+1.0f)*fY, xyz[5].z, color);
	gsKit_prim_line_3d(gsGlobal, (t_xyz[2].x+1.0f)*fX, (t_xyz[2].y+1.0f)*fY, xyz[2].z, (t_xyz[6].x+1.0f)*fX, (t_xyz[6].y+1.0f)*fY, xyz[6].z, color);
	gsKit_prim_line_3d(gsGlobal, (t_xyz[3].x+1.0f)*fX, (t_xyz[3].y+1.0f)*fY, xyz[3].z, (t_xyz[7].x+1.0f)*fX, (t_xyz[7].y+1.0f)*fY, xyz[7].z, color);
	
	free(t_xyz);
	free(xyz);*/
}

void drawOBJ(model* res_m, float pos_x, float pos_y, float pos_z, float rot_x, float rot_y, float rot_z)
{
	glLoadIdentity();
    glTranslatef(pos_x, pos_y, pos_z);
    glRotatef(rot_x, 1, 0, 0); // Rotação em torno do eixo x
    glRotatef(rot_y, 0, 1, 0); // Rotação em torno do eixo y
    glRotatef(rot_z, 0, 0, 1); // Rotação em torno do eixo z

	glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_NORMAL_ARRAY);
    glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	//glEnableClientState(GL_COLOR_ARRAY);

	glVertexPointer(3, GL_FLOAT, 0, res_m->positions);
	glNormalPointer(GL_FLOAT, 0, res_m->normals);
	glTexCoordPointer(2, GL_FLOAT, 0, res_m->texcoords);

	glEnable(GL_TEXTURE_2D);

	if(res_m->rangeCount > 0) {
		material mat;
		for(int i = 0; i < res_m->rangeCount; i++) {
			mat = res_m->materials[res_m->ranges[i].materialIndex];
			glMaterialfv(GL_FRONT, GL_AMBIENT,   mat.Ka);
			glMaterialfv(GL_FRONT, GL_DIFFUSE,   mat.Kd);
			glMaterialfv(GL_FRONT, GL_SPECULAR,  mat.Ks);
			glMaterialfv(GL_FRONT, GL_EMISSION,  mat.Ke);
			glMaterialf (GL_FRONT, GL_SHININESS, mat.Ns);

			if (mat.texture) {
				glBindTexture(GL_TEXTURE_2D, mat.texture->id);
				glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
    			if(mat.texture->clut) {
    			    glColorTable(GL_COLOR_TABLE, GL_RGBA, mat.texture->clut_size, GL_RGBA, GL_UNSIGNED_INT_8_8_8_8, mat.texture->clut);
    			}
			}

			glDrawArrays(GL_TRIANGLES, res_m->ranges[i].start, (res_m->ranges[i].end-res_m->ranges[i].start)+1);
		}
	} else {
		if (res_m->texture) {
			glBindTexture(GL_TEXTURE_2D, res_m->texture->id);
			glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
    		if(res_m->texture->clut) {
    		    glColorTable(GL_COLOR_TABLE, GL_RGBA, res_m->texture->clut_size, GL_RGBA, GL_UNSIGNED_INT_8_8_8_8, res_m->texture->clut);
    		}
		}

		glDrawArrays(GL_TRIANGLES, 0, res_m->indexCount);

	}

	glDisableClientState(GL_VERTEX_ARRAY);
    glDisableClientState(GL_NORMAL_ARRAY);
    glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	//glDisableClientState(GL_COLOR_ARRAY);

	glDisable(GL_TEXTURE_2D);
}


