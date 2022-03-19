#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <stb/stb_image.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

struct Cube
{
	uint32_t vao = 0;
	uint32_t vbo = 0;

	static constexpr float vertices[] =
	{
		// back face
		-1.0f, -1.0f, -1.0f,    0.0f,  0.0f, -1.0f,   0.0f, 0.0f, // bottom-left
		 1.0f,  1.0f, -1.0f,    0.0f,  0.0f, -1.0f,   1.0f, 1.0f, // top-right
		 1.0f, -1.0f, -1.0f,    0.0f,  0.0f, -1.0f,   1.0f, 0.0f, // bottom-right         
		 1.0f,  1.0f, -1.0f,    0.0f,  0.0f, -1.0f,   1.0f, 1.0f, // top-right
		-1.0f, -1.0f, -1.0f,    0.0f,  0.0f, -1.0f,   0.0f, 0.0f, // bottom-left
		-1.0f,  1.0f, -1.0f,    0.0f,  0.0f, -1.0f,   0.0f, 1.0f, // top-left
		// front face		   						  
		-1.0f, -1.0f,  1.0f,    0.0f,  0.0f,  1.0f,   0.0f, 0.0f, // bottom-left
		 1.0f, -1.0f,  1.0f,    0.0f,  0.0f,  1.0f,   1.0f, 0.0f, // bottom-right
		 1.0f,  1.0f,  1.0f,    0.0f,  0.0f,  1.0f,   1.0f, 1.0f, // top-right
		 1.0f,  1.0f,  1.0f,    0.0f,  0.0f,  1.0f,   1.0f, 1.0f, // top-right
		-1.0f,  1.0f,  1.0f,    0.0f,  0.0f,  1.0f,   0.0f, 1.0f, // top-left
		-1.0f, -1.0f,  1.0f,    0.0f,  0.0f,  1.0f,   0.0f, 0.0f, // bottom-left
		// left face		   						  
		-1.0f,  1.0f,  1.0f,   -1.0f,  0.0f,  0.0f,   1.0f, 0.0f, // top-right
		-1.0f,  1.0f, -1.0f,   -1.0f,  0.0f,  0.0f,   1.0f, 1.0f, // top-left
		-1.0f, -1.0f, -1.0f,   -1.0f,  0.0f,  0.0f,   0.0f, 1.0f, // bottom-left
		-1.0f, -1.0f, -1.0f,   -1.0f,  0.0f,  0.0f,   0.0f, 1.0f, // bottom-left
		-1.0f, -1.0f,  1.0f,   -1.0f,  0.0f,  0.0f,   0.0f, 0.0f, // bottom-right
		-1.0f,  1.0f,  1.0f,   -1.0f,  0.0f,  0.0f,   1.0f, 0.0f, // top-right
		// right face		   						  
		 1.0f,  1.0f,  1.0f,    1.0f,  0.0f,  0.0f,   1.0f, 0.0f, // top-left
		 1.0f, -1.0f, -1.0f,    1.0f,  0.0f,  0.0f,   0.0f, 1.0f, // bottom-right
		 1.0f,  1.0f, -1.0f,    1.0f,  0.0f,  0.0f,   1.0f, 1.0f, // top-right         
		 1.0f, -1.0f, -1.0f,    1.0f,  0.0f,  0.0f,   0.0f, 1.0f, // bottom-right
		 1.0f,  1.0f,  1.0f,    1.0f,  0.0f,  0.0f,   1.0f, 0.0f, // top-left
		 1.0f, -1.0f,  1.0f,    1.0f,  0.0f,  0.0f,   0.0f, 0.0f, // bottom-left     
		// bottom face		   						  
		-1.0f, -1.0f, -1.0f,    0.0f, -1.0f,  0.0f,   0.0f, 1.0f, // top-right
		 1.0f, -1.0f, -1.0f,    0.0f, -1.0f,  0.0f,   1.0f, 1.0f, // top-left
		 1.0f, -1.0f,  1.0f,    0.0f, -1.0f,  0.0f,   1.0f, 0.0f, // bottom-left
		 1.0f, -1.0f,  1.0f,    0.0f, -1.0f,  0.0f,   1.0f, 0.0f, // bottom-left
		-1.0f, -1.0f,  1.0f,    0.0f, -1.0f,  0.0f,   0.0f, 0.0f, // bottom-right
		-1.0f, -1.0f, -1.0f,    0.0f, -1.0f,  0.0f,   0.0f, 1.0f, // top-right
		// top face			   						  
		-1.0f,  1.0f, -1.0f,    0.0f,  1.0f,  0.0f,   0.0f, 1.0f, // top-left
		 1.0f,  1.0f , 1.0f,    0.0f,  1.0f,  0.0f,   1.0f, 0.0f, // bottom-right
		 1.0f,  1.0f, -1.0f,    0.0f,  1.0f,  0.0f,   1.0f, 1.0f, // top-right     
		 1.0f,  1.0f,  1.0f,    0.0f,  1.0f,  0.0f,   1.0f, 0.0f, // bottom-right
		-1.0f,  1.0f, -1.0f,    0.0f,  1.0f,  0.0f,   0.0f, 1.0f, // top-left
		-1.0f,  1.0f,  1.0f,    0.0f,  1.0f,  0.0f,   0.0f, 0.0f  // bottom-left        
	};

	Cube()
	{
		glGenVertexArrays(1, &vao);
		glBindVertexArray(vao);

		glGenBuffers(1, &vbo);
		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
		glEnableVertexAttribArray(2);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
	}

	~Cube()
	{
		glDeleteBuffers(1, &vbo);
		glDeleteVertexArrays(1, &vao);
	}
};