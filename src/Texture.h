#pragma once
#include <string>
#include <cassert>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <stb/stb_image.h>

struct Texture
{
	uint32_t texture = 0;

	Texture(const std::string& path)
	{
		glGenTextures(1, &texture);
		glBindTexture(GL_TEXTURE_2D, texture);

		int width, height;
		unsigned char* image = stbi_load(path.c_str(), &width, &height, 0, STBI_rgb_alpha);
		assert(image);

		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
		glGenerateMipmap(GL_TEXTURE_2D);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glBindTexture(GL_TEXTURE_2D, 0);

		stbi_image_free(image);
	}

	~Texture()
	{
		glDeleteTextures(1, &texture);
	}
};