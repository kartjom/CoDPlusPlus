#include "Texture.h"
#include <glad/glad.h>
#include <iostream>
#include "stb_image.h"

void Texture::Load(const char* filePath, bool generateMipmaps)
{
    stbi_set_flip_vertically_on_load(true);

    int width, height, channels;
    unsigned char* data = stbi_load(filePath, &width, &height, &channels, 4);
    if (data)
    {
        glGenTextures(1, &this->Index);

        glActiveTexture(GL_TEXTURE0 + 16);
        glBindTexture(GL_TEXTURE_2D, this->Index);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);

        if (generateMipmaps)
        {
            glGenerateMipmap(GL_TEXTURE_2D);
        }
    }
    else
    {
        std::cout << "Failed to load texture\n";
    }
    stbi_image_free(data);
}

void Texture::Bind(unsigned int slot)
{
    if (!this->Index)
    {
        std::cout << "Trying to bind empty texture\n";
    }

    glActiveTexture(GL_TEXTURE0 + slot);
    glBindTexture(GL_TEXTURE_2D, this->Index);
}

void Texture::Dispose()
{
    if (this->Index)
        glDeleteTextures(1, &this->Index);
}
