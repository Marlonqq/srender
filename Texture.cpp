#include "Texture.h"
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#include <iostream>
Texture::~Texture()
{
	
}


bool Texture::loadTexture(const char* path)
{
	this->path = path;
	stbi_set_flip_vertically_on_load(true);
	data = stbi_load(path, &width, &height, &channel, 0);

	if (data) 
		return true;
	else 
		return false;
}

glm::vec3 Texture::sample2D(float u, float v)
{
	clamp(u);
	clamp(v);

	// TODO 默认双线性插值  后期可以为用户提供选择
	//glm::vec3 res = BilinearInterprolation(u, v);
	//return res;


	float   x = u * width;
	float   y = v * height;
	size_t x0 = static_cast<size_t>(x);
	size_t y0 = static_cast<size_t>(y);
	return sample(x0, y0);
}

const unsigned char* Texture::getData()
{
	return data;
}

int Texture::getWidth()
{
	return width;
}

int Texture::getHeight()
{
	return height;
}

int Texture::getChannel()
{
	return channel;
}

void Texture::clamp(float& val)
{
	if (val < 0.0f) val = 0.0f;
	if (val > 1.0f) val = 1.0f;
}

void Texture::clamp(int& val)
{
	//if (val == height || val == width) --val;
	//if (val < 0) val = 0;]
	if (val >= height) val = height - 1;
	if (val >= width) val = width - 1;
	if (val < 0) val = 0;
}

glm::vec3 Texture::BilinearInterprolation(float u, float v)
{
	float   x = u * width;
	float   y = v * height;
	size_t x0 = static_cast<size_t>(x);
	size_t y0 = static_cast<size_t>(y);
	if (x0 == width) x0 -= 2;
	if (y0 == height) y0 -= 2;
	size_t x1 = x0 + 1;
	size_t y1 = y0 + 1;

	{
		glm::vec3 color0, color1, res, param_c0, param_c1;

		// u方向
		param_c0 = sample(x0, y0);
		param_c1 = sample(x1, y0);
		color0 = lerp(x - x0, param_c0, param_c1);

		param_c0 = sample(x0, y1);
		param_c1 = sample(x1, y1);
		color1 = lerp(x - x0, param_c0, param_c1);

		// v方向
		res = lerp(y - y0, color0, color1);
		return res;
	}
}

glm::vec3 Texture::lerp(float alpha, glm::vec3& color0, glm::vec3& color1)
{
	return color0 + alpha * (color1 - color0);
}

glm::vec3 Texture::sample(int x, int y)
{
	clamp(x);
	clamp(y);
	int s = (width * y + x) * channel;

	glm::vec3 res = glm::vec3(static_cast<size_t>(data[s]) / 255.0f,
		static_cast<size_t>(data[s + 1]) / 255.0f,
		static_cast<size_t>(data[s + 2]) / 255.0f
	);

	//if (channel == 4) res.w = data[s + 3];
	return res;
}