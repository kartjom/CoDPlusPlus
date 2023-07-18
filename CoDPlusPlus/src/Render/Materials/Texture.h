#pragma once
class Texture
{
public:
	void Load(const char* filePath, bool generateMipmaps = true);

	void Bind(unsigned int slot);
	void Dispose();

private:
	unsigned int Index = 0;
};

