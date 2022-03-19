#pragma once
#include <vector>
#include <memory>
#include <cmath>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "Vertex.h"

template<int xSegmentCount, int ySegmentCount>
struct SphereBase
{
	std::vector<Vertex> vertices;
	std::vector<uint32_t> indices;

	SphereBase()
	{
		static constexpr float PI = 3.14159265359f;

		assert(xSegmentCount > 0);
		assert(ySegmentCount > 0);

		for (int x = 0; x <= xSegmentCount; ++x)
		{
			for (int y = 0; y <= ySegmentCount; ++y)
			{
				float xSegment = (float)x / (float)xSegmentCount;
				float ySegment = (float)y / (float)ySegmentCount;
				float xPos = std::cos(xSegment * 2.0f * PI) * std::sin(ySegment * PI);
				float yPos = std::cos(ySegment * PI);
				float zPos = std::sin(xSegment * 2.0f * PI) * std::sin(ySegment * PI);

				vertices.push_back(
				{
					glm::vec3(xPos, yPos, zPos),
					glm::vec3(xPos, yPos, zPos),
					glm::vec2(xSegment, ySegment)
				});
			}
		}

		bool oddRow = false;
		for (int y = 0; y < ySegmentCount; ++y)
		{
			if (!oddRow) // even rows: y == 0, y == 2; and so on
			{
				for (int x = 0; x <= xSegmentCount; ++x)
				{
					indices.push_back(y * (xSegmentCount + 1) + x);
					indices.push_back((y + 1) * (xSegmentCount + 1) + x);
				}
			}
			else
			{
				for (int x = xSegmentCount; x >= 0; --x)
				{
					indices.push_back((y + 1) * (xSegmentCount + 1) + x);
					indices.push_back(y * (xSegmentCount + 1) + x);
				}
			}
			oddRow = !oddRow;
		}
	}
};

template<uint32_t xSegments, uint32_t ySegments>
struct Sphere
{
	uint32_t vao = 0;
	uint32_t vbo = 0;
	uint32_t ibo = 0;

	inline static SphereBase<xSegments, ySegments> data;

	Sphere()
	{
		glGenVertexArrays(1, &vao);
		glBindVertexArray(vao);

		glGenBuffers(1, &vbo);
		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		glBufferData(GL_ARRAY_BUFFER, data.vertices.size() * sizeof(Vertex), data.vertices.data(), GL_STATIC_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, position));
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));
		glEnableVertexAttribArray(2);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, texCoords));
		
		glGenBuffers(1, &ibo);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, data.indices.size() * sizeof(uint32_t), data.indices.data(), GL_STATIC_DRAW);
	}

	~Sphere()
	{
		glDeleteBuffers(1, &vbo);
		glDeleteBuffers(1, &ibo);
		glDeleteVertexArrays(1, &vao);
	}
};