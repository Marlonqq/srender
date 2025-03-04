#pragma once
#ifndef _STRUCTURE_H_
#define _STRUCTURE_H_
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

enum PLANE { TOP, BOTTOM, NEAR, FAR, LEFT, RIGHT, E5 };
enum RENDER_MODE { FLAT, WIRE };
enum LIGHT_TYPE { AMBITIENT, DIFFUSE, SPECULAR };
enum TEXTURE_TYPE {TEXTURE_0, TEXTURE_1, TEXTURE_2};
enum CAMERA_PARA {FOV, ZNEAR};

struct Vertex {
	glm::vec3 pos;
	glm::vec3 norm;
	glm::vec3 color;
	glm::vec2 texCoord;

	Vertex() {
		pos = glm::vec3(1.0f);
		norm = glm::vec3(1.0f);
		color = glm::vec3(0.0f, 0.0f, 0.0f);
		texCoord = glm::vec2(0.0f, 0.0f);
	}
	Vertex(glm::vec3 _p, glm::vec3 _n, glm::vec3 _c) : pos(_p), norm(_n), color(_c) {}
	Vertex(glm::vec3 _p, glm::vec3 _n, glm::vec2 _t) : pos(_p), norm(_n), texCoord(_t) {}
};

struct V2F {
	V2F() = default;
	glm::vec3 pos;
	glm::vec3 norm;
	glm::vec2 texCoord;
	glm::vec3 color;
	glm::vec4 gl_Position;
};

struct Light {
	glm::vec3 dir;
	glm::vec3 pos;
	glm::vec3 ambitient;
	glm::vec3 diffuse;
	glm::vec3 specular;
	float     shininess;

	// TODO øº¬«º”»Îπ‚’’À•ºı
};

struct Material {
	int   diffuse_id;
	int   specular_id;
	float shininess;
};

struct Triangle {
	Vertex verts[3];

	Triangle() = default;

	Triangle(Vertex p0, Vertex p1, Vertex p2)
	{
		verts[0] = p0;
		verts[1] = p1;
		verts[2] = p2;
	}

	Vertex& operator[](int i)
	{
		assert(0 <= i && i <= 2);
		return verts[i];
	}
};

#endif // _STRUCTURE_H_


