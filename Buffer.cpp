#include "Buffer.h"
FrameBuffer::FrameBuffer()
{
	buffer = new unsigned char[BUFFER_SIZE * 4];
	clearColor(glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
}

FrameBuffer::~FrameBuffer()
{
	delete[] buffer;
}

glm::vec4 FrameBuffer::sample(int x, int y)
{
	//if (y == 1080) --y;
	if (x < 0 || x > w - 1 || y < 0 || y > h - 1) return glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
	int s = (w * y + x) * 4;
	return glm::vec4(static_cast<float>(buffer[s]),
		static_cast<float>(buffer[s + 1]),
		static_cast<float>(buffer[s + 2]),
		static_cast<float>(buffer[s + 3])
	);
}

void FrameBuffer::clearColor(glm::vec4 color)
{
	for (int x = 0; x < w; ++x) {
		for (int y = 0; y < h; ++y) {
			cover(x, y, color);
		}
	}
}

void FrameBuffer::cover(int x, int y, glm::vec4& color)
{
	//assert(0 <= x && x < w);
	//assert(0 <= y && y <= h);
	/*if (y < 0) y = 0;
	if (y > h - 1) y = h - 1;
	if (x < 0) x = 0;
	if (x > w - 1) x = w - 1;*/
	if (x < 0 || x > w - 1 || y < 0 || y > h - 1) return;
	int s = (w * y + x) * 4;
	/*color.r = (static_cast<int>(color.r) << 8) - color.r;
	color.g = (static_cast<int>(color.g) << 8) - color.g;
	color.b = (static_cast<int>(color.b) << 8) - color.b;
	color.a = (static_cast<int>(color.a) << 8) - color.a;*/
	buffer[s] = static_cast<unsigned char>(color.r * 255.0f);
	buffer[s + 1] = static_cast<unsigned char>(color.g * 255.0f);
	buffer[s + 2] = static_cast<unsigned char>(color.b * 255.0f);
	buffer[s + 3] = static_cast<unsigned char>(color.a * 255.0f);
}

unsigned char* FrameBuffer::getBuffer()
{
	return buffer;
}

int FrameBuffer::getWidth()
{
	return w;
}

int FrameBuffer::getHeight()
{
	return h;
}

int FrameBuffer::getChannel()
{
	return channel;
}

DepthBuffer::DepthBuffer()
{
	buffer = new float[BUFFER_SIZE];
	clearBuffer();
}

DepthBuffer::~DepthBuffer()
{
	delete[] buffer;
}

float DepthBuffer::sample(int x, int y)
{
	//if (y == 1080) --y;
	if (x < 0 || x > w - 1 || y < 0 || y > h - 1) return FLT_MAX;
	return buffer[w * y + x];
}

void DepthBuffer::cover(int x, int y, float& depth)
{
	//if (y == 1080) --y;
	if (x < 0 || x > w - 1 || y < 0 || y > h - 1) return;
	buffer[w * y + x] = static_cast<float>(depth);
}

void DepthBuffer::clearBuffer()
{
	for (int i = 0; i < BUFFER_SIZE; ++i) buffer[i] = FLT_MAX;
}

