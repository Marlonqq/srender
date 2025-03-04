#pragma once
#ifndef _BUFFER_H_
#define _BUFFER_H_
#define BUFFER_SIZE 2073600 // 1920 * 1080
#include <glm/glm.hpp>

template <typename T>
class Buffer {
public:
	virtual T    sample(int, int) = 0;
	virtual void cover(int, int, T&) = 0;
};

// [0,1] -> [0,255]
class FrameBuffer : Buffer<glm::vec4> {
public:
	FrameBuffer();
	~FrameBuffer();
	virtual glm::vec4 sample(int, int) override;
	virtual void      cover(int, int, glm::vec4&) override;
	void              clearColor(glm::vec4);
	unsigned char*    getBuffer();
	int               getWidth();
	int               getHeight();
	int               getChannel();

private:
	int w = 1920;
	int h = 1080;
	int channel = 4;
	unsigned char* buffer;
};


class DepthBuffer : Buffer<float> {
public:
	DepthBuffer();
	~DepthBuffer();
	virtual float sample(int, int) override;
	virtual void  cover(int, int, float&) override;
	void          clearBuffer();

private:
	int w = 1920;
	int h = 1080;
	float* buffer;
};



#endif // !_BUFFER_H_



