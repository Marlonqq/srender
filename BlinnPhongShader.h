#pragma once
#ifndef _SHADER_H_
#define _SHADER_H_
#include "DataStructure.h"
#include "Texture.h"
#include "SRender.h"
#include "IShader.h"

class BlinnPhongShader : public IShader {
public:
	BlinnPhongShader();
	~BlinnPhongShader();

	void ACTIVE_LIGHT(bool state);
	void ACTIVE_TEXTURE(bool state);

	virtual V2F       vertexShader(Vertex&) override;
	virtual glm::vec4 fragmentShader(V2F* v2f, glm::vec3& abg, float& z_t) override;

private:
	bool SRENDER_LIGHT = true;
	bool SRENDER_TEXTURE = false;

};



#endif // !_SHADER_H_



