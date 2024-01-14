#pragma once

class Geometry 
{
public:
	// Geometry: Bind the geometry to be used (for our case, it should just be a rectangle that covers the entire screen
	void BindGeoms(unsigned int* VAO, unsigned int* VBO, unsigned int* EBO, float max, float min);
};