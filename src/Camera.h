#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

extern uint32_t kScreenWidth;
extern uint32_t kScreenHeight;

struct Camera
{
	glm::vec3 pos = { 0.0f, 0.0f, 0.0f };
	glm::vec3 dir = { 0.0f, 0.0f, -1.0f };
	glm::vec3 up = { 0.0f, 1.0f, 1.0f };
	glm::vec3 right = { 1.0f, 0.0f, 0.0f };

	float fov = 45.0f;
	float aspectRatio = (float)kScreenWidth / kScreenHeight;

	float zNear = 0.1f;
	float zFar = 100.0f;

	Camera(glm::vec3 pos, glm::vec3 targetPos)
		: pos(pos)
	{
		Update(targetPos);
	}

	void Update(glm::vec3 targetPos)
	{
		dir = glm::normalize(targetPos - pos);
		right = glm::normalize(glm::cross(dir, { 0.0f, 1.0f, 0.0f }));
		up = glm::normalize(glm::cross(right, dir));
	}

	glm::mat4 getView() const
	{
		return glm::lookAt(pos, pos + dir, { 0.0f, 1.0f, 0.0f });
	}

	glm::mat4 getProj() const
	{
		return glm::perspective(glm::radians(fov), aspectRatio, zNear, zFar);
	}
};