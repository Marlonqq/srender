#include "Mesh.h"

void Mesh::Draw()
{
	SRender::getInstance().vertices = vertices;
	SRender::getInstance().indices = indices;
	SRender::getInstance().shader->material.diffuse_id = diffuseTextureIndex;
	SRender::getInstance().shader->material.specular_id = specularTextureIndex;
	SRender::getInstance().Render();
}
