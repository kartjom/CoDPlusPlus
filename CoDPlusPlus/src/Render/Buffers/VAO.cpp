#include "VAO.h"
#include <glad/glad.h>

void VAO::Init(int usage)
{
	this->Usage = usage;

	glGenVertexArrays(1, &Index);
	glBindVertexArray(Index);

	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);

	glGenBuffers(1, &EBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
}

void VAO::SetData(void* data, unsigned int size)
{
	this->Bind();
	glBufferData(GL_ARRAY_BUFFER, size, data, this->Usage);
}

void VAO::SetIndices(void* data, unsigned int size)
{
	this->Bind();
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, size, data, this->Usage);
}

void VAO::Bind()
{
	glBindVertexArray(Index);
}

void VAO::Dispose()
{
	if (Index) 
		glDeleteVertexArrays(1, &Index);
	
	if (VBO) 
		glDeleteBuffers(1, &VBO);
	
	if (EBO) 
		glDeleteBuffers(1, &EBO);
}
