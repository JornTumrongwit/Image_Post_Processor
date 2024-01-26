#pragma once

class FrameBuffer {
public:
	unsigned int texture;
	unsigned int frameBuffer;
	unsigned int rbo;

	FrameBuffer(int width, int height);
	void DeleteBuffers();
};