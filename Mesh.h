#pragma once
#include "DataStructure.h"
#include "SRender.h"
#include <vector>
struct Mesh {
	std::vector<Vertex> vertices;
	std::vector<unsigned int> indices;
	int diffuseTextureIndex{ -1 };
	int specularTextureIndex{ -1 };

	Mesh() = default;
	void Draw();
};

