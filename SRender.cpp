#define _CRT_SECURE_NO_WARNINGS
#include "SRender.h"
//#define STB_IMAGE_WRITE_IMPLEMENTATION
//#include <stb_image_write.h>
#include <OpenMesh/Core/Mesh/TriMesh_ArrayKernelT.hh>
#include <OpenMesh/Core/IO/MeshIO.hh>
#include <vector>

SRender::SRender()
{
	frameBuffer = new FrameBuffer();
	depthBuffer = new DepthBuffer();
	w = frameBuffer->getWidth();
	h = frameBuffer->getHeight();
}

SRender::~SRender()
{
	delete frameBuffer;
	delete mesh;
}

void SRender::read_obj(std::string path)
{
	if (mesh)
	{
		triangle_list.clear();
		delete mesh;
	}
	mesh = new m_mesh();

	mesh->request_vertex_normals();    // 添加法线属性
	assert(mesh->has_vertex_normals());

	mesh->request_face_normals();
	mesh->request_vertex_normals();


	OpenMesh::IO::Options opt;
	assert(OpenMesh::IO::read_mesh(*mesh, path, opt));


	mesh->update_normals();
	mesh->release_face_normals();
	mesh->release_vertex_normals();
	if (!opt.check(OpenMesh::IO::Options::VertexNormal)) {
		/*mesh->request_face_normals();
		mesh->update_vertex_normals();
		mesh->release_face_normals();*/
	}



	for (m_mesh::FaceIter f_it = mesh->faces_begin(); f_it != mesh->faces_end(); ++f_it)
	{
		Triangle triangle;
		int cnt = 0;
		for (m_mesh::FaceVertexIter fv_it = mesh->fv_iter(f_it); fv_it.is_valid(); ++fv_it)
		{
			Vertex vert;
			vert.pos = glm::make_vec3(mesh->point(*fv_it).data());
			vert.norm = glm::make_vec3(mesh->normal(*fv_it).data());
			//vert.texCoord = glm::make_vec2(mesh->texcoord2D(*fv_it).data());
			//vert.color    = glm::make_vec3(mesh->color(*fv_it).data());
			triangle.verts[cnt] = vert;
			++cnt;
		}

		triangle_list.emplace_back(triangle);
		cnt = 0;
	}

}

void SRender::set_display_mode(RENDER_MODE mode)
{
	render_mode = mode;
}

void SRender::set_multiThread(bool mul)
{
	multiThread = mul;
}

void SRender::rasterize(Vertex v0, Vertex v1, Vertex v2)
{
	// test BackFaceCuling
	//if (!BackFaceCulling(v0, v1, v2, glm::vec3(0.0f, 0.0f, 2.0f))) return;

	// vertex shader
	//-----------------------------------------
	V2F vert_list[8];
	vert_list[0] = shader->vertexShader(v0);
	vert_list[1] = shader->vertexShader(v1);
	vert_list[2] = shader->vertexShader(v2);

	// clip
	//-----------------------------------------
	int vert_num = 3;
	V2F in_vert_list[8];
	//std::swap(in_vert_list, vert_list);
	vert_num = clip_with_plane(E5, vert_num, vert_list, in_vert_list);
	vert_num = clip_with_plane(TOP, vert_num, in_vert_list, vert_list);
	vert_num = clip_with_plane(BOTTOM, vert_num, vert_list, in_vert_list);
	vert_num = clip_with_plane(LEFT, vert_num, in_vert_list, vert_list);
	vert_num = clip_with_plane(RIGHT, vert_num, vert_list, in_vert_list);
	vert_num = clip_with_plane(NEAR,   vert_num, in_vert_list, vert_list);
	vert_num = clip_with_plane(FAR,    vert_num, vert_list,    in_vert_list);


	// 透视除法 & ndc2screen
	//-----------------------------------------
	for (int i = 0; i < vert_num; ++i) {
		PerspectiveDivision(in_vert_list + i);
		Ndc2Screen(in_vert_list + i);
	}


	//rasterize
	//-----------------------------------------
	V2F clipcoord_v2f[3];
	for (int i = 0; i < vert_num - 2; ++i) {
		int ind0 = 0;
		int ind1 = i + 1;
		int ind2 = i + 2;

		clipcoord_v2f[0] = in_vert_list[ind0];
		clipcoord_v2f[1] = in_vert_list[ind1];
		clipcoord_v2f[2] = in_vert_list[ind2];

		//if (!BackFaceCulling(clipcoord_v2f[0].norm, clipcoord_v2f[1].norm, clipcoord_v2f[2].norm, glm::vec3(0.0f, 0.0f, 2.0f))) continue;
		if (render_mode == FLAT)
		{
			draw_with_flat(clipcoord_v2f);
		}
		else
		{
			int s = i == 0 ? i : i + 1;
			for (; s < i + 3 && s < vert_num; ++s)
			{
				int t = (s - i) % 3 + s;
				draw_with_wire(in_vert_list[s].gl_Position, in_vert_list[t].gl_Position);
			}
		}
	}

}

int SRender::clip_with_plane(PLANE plane, int vert_num, V2F* vert_list, V2F* in_vert_list)
{
	int in_vert_num = 0;
	int pre_vert_ind = vert_num - 1, cur_vert_ind;

	for (int i = 0; i < vert_num; ++i) {
		cur_vert_ind = i;
		pre_vert_ind = pre_vert_ind % vert_num;

		V2F* pre = vert_list + pre_vert_ind;  // 边的七点
		V2F* cur = vert_list + cur_vert_ind;  // 边的终点

		bool pre_state = judge_vert_state(plane, pre->gl_Position);
		bool cur_state = judge_vert_state(plane, cur->gl_Position);

		// 如果该边与面有交点, 则计算交点并存入数组
		if (cur_state ^ pre_state) {
			V2F v2f;
			float t = cal_intersect_ratio(plane, pre->gl_Position, cur->gl_Position);
			v2f.gl_Position = pre->gl_Position + t * (cur->gl_Position - pre->gl_Position);
			v2f.pos         = pre->pos         + t * (cur->pos - pre->pos);
			v2f.norm        = pre->norm        + t * (cur->norm - pre->norm);
			v2f.texCoord    = pre->texCoord    + t * (cur->texCoord - pre->texCoord);

			in_vert_list[in_vert_num] = v2f;
			++in_vert_num;
		}

		// 如果终止点在内侧, 则加入数组
		if (cur_state) {
			in_vert_list[in_vert_num] = *cur;
			++in_vert_num;
		}

		++pre_vert_ind;
	}

	return in_vert_num;
}

bool SRender::judge_vert_state(PLANE plane, glm::vec4& q)
{
	/*
		top    : y < w
		bottom : y > -w
		near   : z > -w
		far    : z < w
		left   : x > -w
		right  : x < w

		true  : 保留
		false : 舍弃
	*/
	/*switch (plane)
	{
	case TOP: return q.y <= q.w;
	case BOTTOM: return q.y >= -q.w;
	case LEFT: return q.x >= -q.w;
	case RIGHT: return q.x <= q.w;
	case NEAR: return q.z >= -q.w;
	case FAR: return q.z <= q.w;
	case E5: return q.w > 1e-5;
	}*/

	switch (plane)
	{
	case TOP: return q.y <= q.w;
	case BOTTOM: return q.y >= -q.w;
	case LEFT: return q.x >= -q.w;
	case RIGHT: return q.x <= q.w;
	case NEAR: return q.z >= 0.0f;
	case FAR: return q.z <= q.w;
	case E5: return q.w > 1e-5;
	}


}

float SRender::cal_intersect_ratio(PLANE plane, glm::vec4& pre, glm::vec4& cur)
{
	/*
		top    : w =  y
		bottom : w = -y
		near   : w = -z
		far    : w =  z
		left   : w = -x
		right  : w =  x
	*/
	/*switch (plane)
	{
	case TOP    : return  (pre.y - pre.w) / ((pre.y - pre.w) - (cur.y - cur.w));
	case BOTTOM : return  (pre.y + pre.w) / ((pre.y + pre.w) - (cur.y + cur.w));
	case LEFT   : return  (pre.x + pre.w) / ((pre.x + pre.w) - (cur.x + cur.w));
	case RIGHT  : return  (pre.x - pre.w) / ((pre.x - pre.w) - (cur.x - cur.w));
	case NEAR   : return  (pre.z + pre.w) / ((pre.z + pre.w) - (cur.z + cur.w));
	case FAR    : return  (pre.z - pre.w) / ((pre.z - pre.w) - (cur.z - cur.w));
	case E5     : return  (1e-5  - pre.w) / (cur.w - pre.w);
	}*/

	switch (plane)
	{
	case TOP: return  (pre.y - pre.w) / ((pre.y - pre.w) - (cur.y - cur.w));
	case BOTTOM: return  (pre.y + pre.w) / ((pre.y + pre.w) - (cur.y + cur.w));
	case LEFT: return  (pre.x + pre.w) / ((pre.x + pre.w) - (cur.x + cur.w));
	case RIGHT: return  (pre.x - pre.w) / ((pre.x - pre.w) - (cur.x - cur.w));
	case NEAR: return   pre.w / (pre.w - cur.w);
	case FAR: return  (pre.z - pre.w) / ((pre.z - pre.w) - (cur.z - cur.w));
	case E5     : return  (1e-5 - pre.w) / (cur.w - pre.w);
	}
}

void SRender::draw_with_flat(V2F* v2f)
{
	glm::vec4 bound;
	glm::vec3 abg, p;
	glm::vec4 color;
	float z_t;
	bound = getBoundingBox(v2f[0].gl_Position, v2f[1].gl_Position, v2f[2].gl_Position);

	for (int y = bound[2]; y <= bound[3]; ++y) {
		for (int x = bound[0]; x <= bound[1]; ++x) {
			p = glm::vec3(x, y, 1.0f);

			abg = getBarycentricCoord(p, v2f[0].gl_Position, v2f[1].gl_Position, v2f[2].gl_Position);
			if (!inTriangle(abg)) continue;

			z_t = 1 / (abg.x / v2f[0].pos.z + abg.y / v2f[1].pos.z + abg.z / v2f[2].pos.z);
			if (!depthTest<Less>(x, y, z_t)) continue;

			color = shader->fragmentShader(v2f, abg, z_t);
			//if (EdgeDetection(abg)) color = MSAA(x, y, v2f, color);
			frameBuffer->cover(x, y, color);
			depthBuffer->cover(x, y, z_t);
		}
	}
}

void SRender::draw_with_wire(glm::vec4& gl_p0, glm::vec4& gl_p1)
{
	glm::vec4 p0 = gl_p0;
	glm::vec4 p1 = gl_p1;
	if (gl_p0.x > gl_p1.x)
	{
		p0 = gl_p0;
		p1 = gl_p1;
	}

	bool invert = p1.y > p0.y ? false : true;

	int dy = p1.y - p0.y;
	int dx = p1.x - p0.x;
	int d = dy << 1;
	int x = p0.x;
	int y = p0.y;
	int y0 = p0.y;

	frameBuffer->cover(x, y, LINE_COLOR);

	if (d > dx << 1) d -= dx << 1;

	while (x <= p1.x)
	{
		++x;
		if (d > dx)
		{
			++y;
			d -= dx << 1;
		}

		d += dy << 1;

		if (invert) frameBuffer->cover(x, (y0 << 1) - y, LINE_COLOR);
		else frameBuffer->cover(x, y, LINE_COLOR);
	}
}

void SRender::Ndc2Screen(V2F* v2f)
{
	v2f->gl_Position.x = static_cast<int>((v2f->gl_Position.x + 1.0f) * w / 2);
	v2f->gl_Position.y = static_cast<int>((v2f->gl_Position.y + 1.0f) * h / 2);
}

glm::vec4 SRender::
MSAA(int x, int y, V2F* v2f, glm::vec4& color)
{
	float for_each_x[4] = { 0.5f, 0.5f, 0.0f, 0.0f };
	float for_each_y[4] = { 0.5f, 0.0f, 0.5f, 0.0f };
	float percent = 0.0f;
	for (int i = 0; i < 4; ++i)
	{
		glm::vec3 pos(x + for_each_x[i], y + for_each_y[i], 1.0f);
		glm::vec3 abg = getBarycentricCoord(pos, v2f[0].gl_Position, v2f[1].gl_Position, v2f[2].gl_Position);
		if (inTriangle(abg)) percent += 1.0f;
	}

	percent /= 4.0f;
	
	return color * percent;
}

bool SRender::EdgeDetection(glm::vec3& abg)
{
	float epsilon = 1e-4;
	return abs(abg.x - 0.0f) <= epsilon ||
		   abs(abg.y - 0.0f) <= epsilon ||
		   abs(abg.z - 0.0f) <= epsilon;
}

bool SRender::BackFaceCulling(glm::vec3 a, glm::vec3 b, glm::vec3 c, glm::vec3 eye)
{
	glm::vec3 norm = glm::normalize((a + b + c) / 3.0f);
	if (glm::dot(eye, norm) > 0.5f) return false;
	else return true;
}

void SRender::PerspectiveDivision(V2F* v2f)
{
	v2f->gl_Position.x /= v2f->gl_Position.w;
	v2f->gl_Position.y /= v2f->gl_Position.w;
	v2f->gl_Position.z /= v2f->gl_Position.w;
	v2f->gl_Position.w = 1.0f;
}

void SRender::calculate_triangle_list()
{
	triangle_list.clear();
	for (int i = 0; i < indices.size(); i += 3)
	{
		assert(i + 2 < indices.size());
		triangle_list.emplace_back(Triangle(
			vertices.at(indices.at(i)),
			vertices.at(indices.at(i + 1)),
			vertices.at(indices.at(i + 2))
		)
		);
	}
}

unsigned char* SRender::getBuffer()
{
	return frameBuffer->getBuffer();
}

void SRender::render()
{
	triangle_list.clear();
	if (multiThread)
	{
		tbb::parallel_for(tbb::blocked_range<size_t>(0, triangle_list.size(), 1),
			[&](tbb::blocked_range<size_t> r)
			{
				for (size_t i = r.begin(); i < r.end(); ++i)
				{
					rasterize(triangle_list[i].verts[0], triangle_list[i].verts[1], triangle_list[i].verts[2]);
				}
			}
		);
	}
	else
	{
		for (int i = 0; i < triangle_list.size(); ++i)
		{
			rasterize(triangle_list[i][0], triangle_list[i][1], triangle_list[i][2]);
		}
	}

}

void SRender::Render()
{
	calculate_triangle_list();
	if (multiThread)
	{
		tbb::parallel_for(tbb::blocked_range<size_t>(0, triangle_list.size(), 1),
			[&](tbb::blocked_range<size_t> r)
			{
				for (size_t i = r.begin(); i < r.end(); ++i)
				{
					rasterize(triangle_list[i].verts[0], triangle_list[i].verts[1], triangle_list[i].verts[2]);
				}
			}
		);
	}
	else
	{
		for (int i = 0; i < triangle_list.size(); ++i)
		{
			rasterize(triangle_list[i][0], triangle_list[i][1], triangle_list[i][2]);
		}
	}
}

void SRender::clearBuffer()
{
	frameBuffer->clearColor(glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
	depthBuffer->clearBuffer();
}

void SRender::swapBuffer()
{
}

void SRender::save_image(const char* path)
{
	/*stbi_flip_vertically_on_write(true);
	stbi_write_jpg(path, w, h, backBuffer->getChannel(), backBuffer->getBuffer(), 0);*/
}

glm::vec4 SRender::getBoundingBox(glm::vec4& v0, glm::vec4& v1, glm::vec4& v2)
{
	glm::vec4 box; // x_left, x_right, y_low, y_top
	box[0] = std::min(std::min(v0.x, v1.x), v2.x);
	box[1] = std::max(std::max(v0.x, v1.x), v2.x);
	box[2] = std::min(std::min(v0.y, v1.y), v2.y);
	box[3] = std::max(std::max(v0.y, v1.y), v2.y);

	if (box[0] < 0) box[0] = 0;
	if (box[1] > w - 1) box[1] = w - 1;
	if (box[2] < 0) box[2] = 0;
	if (box[3] > h - 1) box[3] = h - 1;

	return box;
}

bool SRender::inTriangle(glm::vec3& barycentricCoord)
{
	return barycentricCoord.x >= 0.0f && barycentricCoord.y >= 0.0f && barycentricCoord.z >= 0.0f;
}

template <typename Cmp>
bool SRender::depthTest(int& x, int& y, float& z)
{
	Cmp cmp;
	float __z = depthBuffer->sample(x, y);
	return cmp(z, __z);
}

glm::vec3 SRender::getBarycentricCoord(glm::vec3& p, glm::vec4& a, glm::vec4& b, glm::vec4& c)
{
	float beta = ((a.y - c.y) * p.x + (c.x - a.x) * p.y + a.x * c.y - c.x * a.y) / ((a.y - c.y) * b.x + (c.x - a.x) * b.y + a.x * c.y - c.x * a.y);
	float gama = ((a.y - b.y) * p.x + (b.x - a.x) * p.y + a.x * b.y - b.x * a.y) / ((a.y - b.y) * c.x + (b.x - a.x) * c.y + a.x * b.y - b.x * a.y);
	float alpha = 1.0f - beta - gama;

	return glm::vec3(alpha, beta, gama);
}

