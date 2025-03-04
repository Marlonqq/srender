#include "BlinnPhongShader.h"
#include <iostream>
BlinnPhongShader::BlinnPhongShader()
{
	//model = glm::mat4(1.0f);
	//view = glm::lookAt(glm::vec3(0.0f, 0.0f, 2.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	////view = glm::mat4(1.0f);
	//projection = glm::ortho(-1, 1, -1, 1, -1, 1);
	////projection = glm::mat4(1.0f);
}

BlinnPhongShader::~BlinnPhongShader()
{
}

void BlinnPhongShader::ACTIVE_LIGHT(bool state)
{
	SRENDER_LIGHT = state;
}

void BlinnPhongShader::ACTIVE_TEXTURE(bool state)
{
	SRENDER_TEXTURE = state;
}

V2F BlinnPhongShader::vertexShader(Vertex& v)
{
	V2F v2f;
	glm::vec4 gl_Position = projection * view * model * glm::vec4(v.pos, 1.0f);
	v2f.gl_Position = gl_Position;

	glm::mat4 model_inv_t = glm::transpose(glm::inverse(view * model));
	v2f.pos = glm::vec3(view * model * glm::vec4(v.pos, 1.0f));
	v2f.texCoord = v.texCoord;
	v2f.norm = glm::normalize(glm::vec3(model_inv_t * glm::vec4(v.norm, 1.0f)));


	return v2f;
}

glm::vec4 BlinnPhongShader::fragmentShader(V2F* v2f, glm::vec3& abg, float& z_t)
{
	glm::vec3 pos = (abg.x * v2f[0].pos / v2f[0].pos.z +
		abg.y * v2f[1].pos / v2f[1].pos.z +
		abg.z * v2f[2].pos / v2f[2].pos.z) * z_t;
	glm::vec3 norm = (abg.x * v2f[0].norm / v2f[0].pos.z +
		abg.y * v2f[1].norm / v2f[1].pos.z +
		abg.z * v2f[2].norm / v2f[2].pos.z) * z_t;
	glm::vec2 texCoord = (abg.x * v2f[0].texCoord / v2f[0].pos.z +
		abg.y * v2f[1].texCoord / v2f[1].pos.z +
		abg.z * v2f[2].texCoord / v2f[2].pos.z) * z_t;

	glm::vec3 mat_diffuse(0.5f, 0.5f, 0.5f);
	glm::vec3 mat_specular(1.0f, 1.0f, 1.0f);
	if (material.diffuse_id != -1) 
		mat_diffuse = SRender::getInstance().texture_list.at(material.diffuse_id).sample2D(texCoord.x, texCoord.y);
	if (material.specular_id != -1)
		mat_specular = SRender::getInstance().texture_list.at(material.specular_id).sample2D(texCoord.x, texCoord.y);


	norm = glm::normalize(norm);
	glm::vec3 light_dir = glm::normalize(-light.dir);
	glm::vec3 h = glm::normalize(cameraPos - pos + light_dir);
	glm::vec3 ambitient = light.ambitient * mat_diffuse;
	glm::vec3 diffuse = light.diffuse * glm::max(glm::dot(light_dir, norm), 0.0f) * mat_diffuse;
	glm::vec3 specular = light.specular * glm::pow(glm::max(glm::dot(norm, h), 0.0f), material.shininess) * mat_specular;

	glm::vec3 res = ambitient + diffuse + specular;
	return glm::vec4(res, 1.0f);
}


