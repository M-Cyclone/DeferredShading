#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>

struct PointLight
{
	glm::vec3 position;
	glm::vec3 intensity;
};