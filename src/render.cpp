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

static float camera_pos[] = {0.0f, 0.0f, 0.0f};
static float camera_rot[] = {0.0f, 0.0f, 0.0f};

static int active_lights = 0;

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

void setCameraPosition(float x, float y, float z){
	camera_pos[0] = x;
	camera_pos[1] = y;
	camera_pos[2] = z;
}

void setCameraRotation(float x, float y, float z){
	camera_rot[0] = x;
	camera_rot[1] = y;
	camera_rot[2] = z;
}

void setLightAttribute(int lightid, float a, float b, float c, float d, int attr){
	float data[] = {a, b, c, d};
    glLightfv(GL_LIGHT0+lightid, attr, data);
}

int createLight(){
	glEnable(GL_LIGHT0+active_lights);
	return active_lights++;
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

	//calculate bounding box
	float lowX, lowY, lowZ, hiX, hiY, hiZ;
    lowX = hiX = res_m->positions[0];
    lowY = hiY = res_m->positions[1];
    lowZ = hiZ = res_m->positions[2];
    for (int i = 0; i < res_m->indexCount; i++) {
        if (lowX > res_m->positions[i * 3 + 0]) lowX = res_m->positions[i * 3 + 0];
        if (hiX <  res_m->positions[i * 3 + 0]) hiX =  res_m->positions[i * 3 + 0];
        if (lowY > res_m->positions[i * 3 + 1]) lowY = res_m->positions[i * 3 + 1];
        if (hiY <  res_m->positions[i * 3 + 1]) hiY =  res_m->positions[i * 3 + 1];
        if (lowZ > res_m->positions[i * 3 + 2]) lowZ = res_m->positions[i * 3 + 2];
        if (hiZ <  res_m->positions[i * 3 + 2]) hiZ =  res_m->positions[i * 3 + 2];
    }

    res_m->bounding_box[0] = lowX;
	res_m->bounding_box[1] = lowY;
	res_m->bounding_box[2] = lowZ;

    res_m->bounding_box[3] = lowX;
	res_m->bounding_box[4] = lowY;
	res_m->bounding_box[5] = hiZ;

    res_m->bounding_box[6] = lowX;
	res_m->bounding_box[7] = hiY;
	res_m->bounding_box[8] = lowZ;

    res_m->bounding_box[9] = lowX;
	res_m->bounding_box[10] = hiY;
	res_m->bounding_box[11] = hiZ;

    res_m->bounding_box[12] = hiX;
	res_m->bounding_box[13] = lowY;
	res_m->bounding_box[14] = lowZ;

    res_m->bounding_box[15] = hiX;
	res_m->bounding_box[16] = lowY;
	res_m->bounding_box[17] = hiZ;

    res_m->bounding_box[18] = hiX;
	res_m->bounding_box[19] = hiY;
	res_m->bounding_box[20] = lowZ;

    res_m->bounding_box[21] = hiX;
	res_m->bounding_box[22] = hiY;
	res_m->bounding_box[23] = hiZ;

	fast_obj_destroy(m);

    return res_m;
}


void draw_bbox(model* m, float pos_x, float pos_y, float pos_z, float rot_x, float rot_y, float rot_z, Color color)
{
	glLoadIdentity();
    glTranslatef(pos_x-camera_pos[0], pos_y-camera_pos[1], pos_z-camera_pos[2]);
    glRotatef(rot_x+camera_rot[0], 1, 0, 0); // Rotação em torno do eixo x
    glRotatef(rot_y+camera_rot[1], 0, 1, 0); // Rotação em torno do eixo y
    glRotatef(rot_z+camera_rot[2], 0, 0, 1); // Rotação em torno do eixo z

	glColor4f(R(color)/255.0f, G(color)/255.0f, B(color)/255.0f, A(color)/255.0f);

	glBegin(GL_LINES);
		glVertex3f(m->bounding_box[0], m->bounding_box[1], m->bounding_box[2]);
		glVertex3f(m->bounding_box[3], m->bounding_box[4], m->bounding_box[5]);
		glVertex3f(m->bounding_box[3], m->bounding_box[4], m->bounding_box[5]);
		glVertex3f(m->bounding_box[9], m->bounding_box[10], m->bounding_box[11]);
		glVertex3f(m->bounding_box[6], m->bounding_box[7], m->bounding_box[8]);
		glVertex3f(m->bounding_box[9], m->bounding_box[10], m->bounding_box[11]);
		glVertex3f(m->bounding_box[0], m->bounding_box[1], m->bounding_box[2]);
		glVertex3f(m->bounding_box[6], m->bounding_box[7], m->bounding_box[8]);
		glVertex3f(m->bounding_box[12], m->bounding_box[13], m->bounding_box[14]);
		glVertex3f(m->bounding_box[15], m->bounding_box[16], m->bounding_box[17]);
		glVertex3f(m->bounding_box[15], m->bounding_box[16], m->bounding_box[17]);
		glVertex3f(m->bounding_box[21], m->bounding_box[22], m->bounding_box[23]);
		glVertex3f(m->bounding_box[18], m->bounding_box[19], m->bounding_box[20]);
		glVertex3f(m->bounding_box[21], m->bounding_box[22], m->bounding_box[23]);
		glVertex3f(m->bounding_box[12], m->bounding_box[13], m->bounding_box[14]);
		glVertex3f(m->bounding_box[18], m->bounding_box[19], m->bounding_box[20]);
		glVertex3f(m->bounding_box[0], m->bounding_box[1], m->bounding_box[2]);
		glVertex3f(m->bounding_box[12], m->bounding_box[13], m->bounding_box[14]);
		glVertex3f(m->bounding_box[3], m->bounding_box[4], m->bounding_box[5]);
		glVertex3f(m->bounding_box[15], m->bounding_box[16], m->bounding_box[17]);
		glVertex3f(m->bounding_box[6], m->bounding_box[7], m->bounding_box[8]);
		glVertex3f(m->bounding_box[18], m->bounding_box[19], m->bounding_box[20]);
		glVertex3f(m->bounding_box[9], m->bounding_box[10], m->bounding_box[11]);
		glVertex3f(m->bounding_box[21], m->bounding_box[22], m->bounding_box[23]);
	glEnd();

	
}

void drawOBJ(model* res_m, float pos_x, float pos_y, float pos_z, float rot_x, float rot_y, float rot_z)
{
	glLoadIdentity();
    glTranslatef(pos_x-camera_pos[0], pos_y-camera_pos[1], pos_z-camera_pos[2]);
    glRotatef(rot_x+camera_rot[0], 1, 0, 0); // Rotação em torno do eixo x
    glRotatef(rot_y+camera_rot[1], 0, 1, 0); // Rotação em torno do eixo y
    glRotatef(rot_z+camera_rot[2], 0, 0, 1); // Rotação em torno do eixo z

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


