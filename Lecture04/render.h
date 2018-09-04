/**
 * SER 431
 * https://speakerdeck.com/javiergs/ser431-lecture-04
 **/

#pragma once
#include <GL/glut.h>
#include "mesh.h"

#define GL_CLAMP_TO_EDGE 0x812F
#define GL_TEXTURE_WRAP_R 0x8072
// draw
GLuint meshToDisplayList(Mesh* m, int id, int texture) {
	GLuint listID = glGenLists(id);
	glNewList(listID, GL_COMPILE);
	glEnable(GL_TEXTURE_2D);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	glBindTexture(GL_TEXTURE_2D, texture);
	//}
	glBegin(GL_TRIANGLES);
	for (unsigned int i = 0; i < m->face_index_vertex.size(); i++) {
		// PER VERTEX NORMALS
		if ((!m->dot_normalPerVertex.empty() && !m->face_index_normalPerVertex.empty())) {
			glNormal3fv(&m->dot_normalPerVertex[m->face_index_normalPerVertex[i]].x);
		}
		// TEXTURES
		if (!m->dot_texture.empty() && !m->face_index_texture.empty()) {
			glTexCoord2fv(&m->dot_texture[m->face_index_texture[i]].x);
		}
		// COLOR
		Vec3f offset = (m->dot_vertex[m->face_index_vertex[i]]);
		// VERTEX
		glColor3f(fabs(sin(offset.x)), fabs(cos(offset.y)), fabs(offset.z));
		glVertex3fv(&m->dot_vertex[m->face_index_vertex[i]].x);
	}
	glEnd();
	glDisable(GL_TEXTURE_2D);
	glEndList();
	return listID;
}
