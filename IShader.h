#pragma once
#include "DataStructure.h"
class IShader {
public:
	// vertexShader data
	//--------------------------------------------
	glm::mat4 model=glm::mat4(1.0f);
	glm::mat4 view = glm::mat4(1.0f);
	glm::mat4 projection = glm::mat4(1.0f);

	// fragmentShader data
	//--------------------------------------------
	Light light;
	Material material;
	glm::vec3 cameraPos;

	virtual V2F       vertexShader(Vertex&) = 0;
	virtual glm::vec4 fragmentShader(V2F*, glm::vec3&, float& z_t) = 0;
};