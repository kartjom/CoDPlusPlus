#include "VAO_utils.h"

int VAO_u::GetAttribSize(int type, int count)
{
	switch (type)
	{
	case GL_BYTE: return sizeof(char) * count;
	case GL_UNSIGNED_BYTE: return sizeof(unsigned char) * count;
	case GL_SHORT: return sizeof(short) * count;
	case GL_UNSIGNED_SHORT: return sizeof(unsigned short) * count;
	case GL_INT: return sizeof(int) * count;
	case GL_UNSIGNED_INT: return sizeof(unsigned int) * count;
	case GL_FLOAT: return sizeof(float) * count;
	}

	assert(false);
}