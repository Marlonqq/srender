#pragma once
#ifndef _SRENDER_H_
#define _SRENDER_H_
#include <vector>
#include <string>
//#include <OpenMesh/Core/Mesh/TriMesh_ArrayKernelT.hh>
//#include <OpenMesh/Core/IO/MeshIO.hh>
#include <OpenMesh/Core/Mesh/TriMesh_ArrayKernelT.hh>
#include <OpenMesh/Core/IO/MeshIO.hh>
#include "DataStructure.h"
#include "Buffer.h" 
#include "IShader.h"
#include "Texture.h"
#include "Camera.h"
#include <glm/gtc/type_ptr.hpp>
#include <glm/glm.hpp>
#include <string>


#ifndef Q_MOC_RUN
#if defined(emit)
#undef emit
#define emit // restore the macro definition of "emit", as it was defined in gtmetamacros.h
#else
#include <tbb/parallel_for.h>
#endif // defined(emit)
#endif // Q_MOC_RUN
#include <tbb/blocked_range.h>


typedef OpenMesh::TriMesh_ArrayKernelT<> m_mesh;

struct Greater {
	bool operator() (const float& a, const float& b)
	{
		return a >= b;
	}
};

struct Less {
	bool operator() (const float& a, const float& b)
	{
		return a <= b;
	}
};

class SRender {
public:
	static void Init()
	{
		getInstance();
	}
	static SRender& getInstance()
	{
		static SRender instance;
		return instance;
	}

	SRender();
	~SRender();
	void read_obj(std::string path);
	void render();
	void Render();
	void calculate_triangle_list();

	void clearBuffer();
	void swapBuffer();
	unsigned char* getBuffer();

	void save_image(const char* path);
	void set_display_mode(RENDER_MODE mode);
	void set_multiThread(bool mul);

	template <typename T>
	glm::mat4   ortho01(T left, T right, T bottom, T top, T zNear, T zFar);

	template<typename T>
	glm::mat4   perspective_ReversedZ(T fov, T aspect, T zNear, T zFar);

private:
	void        rasterize(Vertex, Vertex, Vertex);
	glm::vec4   getBoundingBox(glm::vec4&, glm::vec4&, glm::vec4&);
	glm::vec3   getBarycentricCoord(glm::vec3&, glm::vec4&, glm::vec4&, glm::vec4&);
	inline bool inTriangle(glm::vec3&);
	int         clip_with_plane(PLANE plane, int vert_num, V2F* vert_list, V2F* in_vert_list);
	bool        judge_vert_state(PLANE plane, glm::vec4& q);
	float       cal_intersect_ratio(PLANE plane, glm::vec4& q1, glm::vec4& q2);
	void        draw_with_flat(V2F* v2f);
	void        draw_with_wire(glm::vec4& gl_p0, glm::vec4& gl_p1);
	bool        BackFaceCulling(glm::vec3 a, glm::vec3 b, glm::vec3 c, glm::vec3 eye);
	void        PerspectiveDivision(V2F* v2f);
	void        Ndc2Screen(V2F* v2f);
	glm::vec4   MSAA(int x, int y, V2F* v2f, glm::vec4& color);
	bool        EdgeDetection(glm::vec3& abg);
	

	template <typename Cmp = Less>
	bool        depthTest(int&, int&, float&);
	


private:
	bool multiThread = true;
	bool faceCulling = true;
	glm::vec4 LINE_COLOR = glm::vec4(0.5f, 0.5f, 0.5f, 1.0f);

public:
	int w, h;

	// 模型有关数据
	//-----------------------------------------
	std::vector<Vertex> vertices;
	std::vector<unsigned int> indices;
	std::vector<Texture> texture_list;
	std::vector<Triangle> triangle_list;

	// other
	//------------------------------------------
	RENDER_MODE render_mode = FLAT;
	IShader* shader = nullptr;
	FrameBuffer* frameBuffer = nullptr;
	DepthBuffer* depthBuffer;
	m_mesh* mesh = nullptr;
	
};


#endif // !_SRENDER_H_

template<typename T>
inline glm::mat4 SRender::ortho01(T left, T right, T bottom, T top, T zNear, T zFar)
{
	glm::mat4 ortho(0.0f);
	ortho[0][0] = static_cast<T>(2) / (right - left);
	ortho[1][1] = static_cast<T>(2) / (top - bottom); 
	ortho[2][2] = static_cast<T>(1) / (zFar - zNear);
	ortho[3][0] = -(left + right) / (right - left);
	ortho[3][1] = -(bottom + top) / (top - bottom);
	ortho[3][2] = -(zNear) / (zFar - zNear);
	ortho[3][3] = static_cast<T>(1);

	return ortho;
}

template<typename T>
inline glm::mat4 SRender::perspective_ReversedZ(T fov, T aspect, T zNear, T zFar)
{
	const T tanHalfFov = glm::tan(glm::radians(fov) / static_cast<T>(2));
	glm::mat4 persp(0.0f);
	persp[0][0] = static_cast<T>(1) / (aspect * tanHalfFov);
	persp[1][1] = static_cast<T>(1) / tanHalfFov;
	persp[2][2] = zFar / (zFar - zNear);
	persp[2][3] = static_cast<T>(1);
	persp[3][2] = -zFar * zNear / (zFar - zNear);
	
	//persp[2][2] = -zNear / (zFar - zNear);
	//persp[3][2] = zFar * zNear / (zFar - zNear);

	return persp;
}
