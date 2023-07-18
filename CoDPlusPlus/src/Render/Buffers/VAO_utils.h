#pragma once
#include "glad/glad.h"
#include <cassert>

#define ValidateSize(_class) assert(VAO_u::VertexStructSize == sizeof(_class));
#define DefineAttribute(count, type, normalized, _class, member) glVertexAttribPointer(VAO_u::VertexAttribIndex, count, type, normalized, sizeof(_class), (void*)offsetof(_class, member)); \
    glEnableVertexAttribArray(VAO_u::VertexAttribIndex++); \
    VAO_u::VertexStructSize += VAO_u::GetAttribSize(type, count);

namespace VAO_u
{
	inline int VertexStructSize = 0;
	inline int VertexAttribIndex = 0;
	int GetAttribSize(int type, int count);
}