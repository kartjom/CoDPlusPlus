#include "Shader.h"
#include <glad/glad.h>
#include <sstream>
#include <iostream>

bool Shader::CompileShader(int type, const char* source, unsigned int* out)
{
    unsigned int shader = glCreateShader(type);

    glShaderSource(shader, 1, &source, nullptr);
    glCompileShader(shader);

    int isCompiled = 0;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &isCompiled);
    if (isCompiled == GL_FALSE)
    {
        GLint maxLength = 0;
        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &maxLength);

        char* errorLog = (char*)alloca(maxLength);
        glGetShaderInfoLog(shader, maxLength, &maxLength, errorLog);

        std::cout << errorLog << std::endl;
    }

    *out = shader;

    return isCompiled;
}

void Shader::Compile(std::string vertexShaderSource, std::string fragmentShaderSource)
{
    unsigned int vertexShader, fragmentShader;

    bool vertexShaderCompiled = Shader::CompileShader(GL_VERTEX_SHADER, vertexShaderSource.data(), &vertexShader);
    bool fragmentShaderCompiled = Shader::CompileShader(GL_FRAGMENT_SHADER, fragmentShaderSource.data(), &fragmentShader);

    if (vertexShaderCompiled && fragmentShaderCompiled)
    {
        this->Program = glCreateProgram();

        glAttachShader(this->Program, vertexShader);
        glAttachShader(this->Program, fragmentShader);
        glLinkProgram(this->Program);
    }

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
}

void Shader::Compile(std::string combinedShaderSource)
{
    enum ShaderType
    {
        Vertex,
        Fragment,
        None
    };

    std::stringstream shaderSources[2];
    std::stringstream ss(combinedShaderSource);
    ShaderType type;

    std::string line;
    while (std::getline(ss, line))
    {
        if (line.find("#shader ") != std::string::npos)
        {
            if (line.find(" fragment") != std::string::npos) type = ShaderType::Fragment;
            else if (line.find(" vertex") != std::string::npos) type = ShaderType::Vertex;
            else type = ShaderType::None;
        }
        else if (type != ShaderType::None)
        {
            shaderSources[(int)type] << line << '\n';
        }
    }

    this->Compile(shaderSources[ShaderType::Vertex].str().c_str(), shaderSources[ShaderType::Fragment].str().c_str());
}

void Shader::Bind()
{
    if (this->Program == 0)
    {
        std::cout << "Trying to use empty shader program\n";
    }

    glUseProgram(this->Program);
}

void Shader::Dispose()
{
    if (this->Program)
    {
        glDeleteProgram(this->Program);
    }
}
