#pragma once
#include <string>

class Shader
{
public:
	static bool CompileShader(int type, const char* source, unsigned int* out);
	void Compile(std::string vertexShaderSource, std::string fragmentShaderSource);
	void Compile(std::string combinedShaderSource);
	
	void Bind();
	void Dispose();

	inline operator unsigned int() { return Program; }

private:
	unsigned int Program = 0;
};

