#pragma once
#ifndef _TEXTURE_H_
#define _TEXTURE_H_
#include <glm/glm.hpp>
#include <iostream>

class Texture {
public:
	std::string path;

	Texture() = default;
	~Texture();
	bool           loadTexture(const char*);
	glm::vec3      sample2D(float u, float v);
	int            getWidth();
	int            getHeight();
	int            getChannel();
	const unsigned char* getData();

private:
	void      clamp(float& val);
	void      clamp(int& val);
	glm::vec3 BilinearInterprolation(float u, float v);
	glm::vec3 lerp(float alpha, glm::vec3& color0, glm::vec3& color1);
	glm::vec3 sample(int x, int y);

private:
	int width, height, channel;
	const unsigned char* data = nullptr;

};

#endif // !_TEXTURE_H_



