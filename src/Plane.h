#pragma once
#include <vector>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "Vertex.h"

struct Plane
{
	inline static const std::vector<Vertex> vertices
	{
		{ glm::vec3{-1.0f, -1.0f, 0.0f}, glm::vec3{0.0f, 0.0f, 1.0f}, glm::vec2{0.0f, 0.0f}},
		{ glm::vec3{-1.0f, +1.0f, 0.0f}, glm::vec3{0.0f, 0.0f, 1.0f}, glm::vec2{0.0f, 1.0f}},
		{ glm::vec3{+1.0f, +1.0f, 0.0f}, glm::vec3{0.0f, 0.0f, 1.0f}, glm::vec2{1.0f, 1.0f}},
		{ glm::vec3{+1.0f, -1.0f, 0.0f}, glm::vec3{0.0f, 0.0f, 1.0f}, glm::vec2{1.0f, 0.0f}},
	};
	inline static const std::vector<uint32_t> indices
	{
		0, 1, 2,
		0, 2, 3
	};

	uint32_t vao = 0;
	uint32_t vbo = 0;
	uint32_t ibo = 0;

	Plane()
	{
		glGenVertexArrays(1, &vao);
		glBindVertexArray(vao);

		glGenBuffers(1, &vbo);
		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * vertices.size(), vertices.data(), GL_STATIC_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, position));
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));
		glEnableVertexAttribArray(2);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, texCoords));

		glGenBuffers(1, &ibo);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(uint32_t), indices.data(), GL_STATIC_DRAW);
	}

	~Plane()
	{
		glDeleteBuffers(1, &vbo);
		glDeleteBuffers(1, &ibo);
		glDeleteVertexArrays(1, &vao);
	}
};