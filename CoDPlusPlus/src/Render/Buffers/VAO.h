#pragma once
#include "VAO_utils.h"

class VAO
{
public:
	void Init(int usage);

	void SetData(void* data, unsigned int size);
	void SetIndices(void* data, unsigned int size);

	void Bind();
	void Dispose();

	template <typename StructT>
	inline void SetAttributes(void(*func)());

private:
	unsigned int Index = 0;
	unsigned int VBO = 0;
	unsigned int EBO = 0;

	int Usage = 0;
};

template<typename StructT>
inline void VAO::SetAttributes(void(*func)())
{
	this->Bind();
	VAO_u::VertexStructSize = 0;
	VAO_u::VertexAttribIndex = 0;

	func();
	ValidateSize(StructT);
}