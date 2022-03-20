#pragma once
#include <string>
#include <cassert>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <stb/stb_image.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "Shader.h"
#include "Cube.h"

extern uint32_t kScreenWidth;
extern uint32_t kScreenHeight;

struct HdrTexture
{
	uint32_t hdrTex = 0;
	uint32_t hdrCubeMap = 0;
	uint32_t irradianceMap = 0;
	uint32_t prefilterMap = 0;

	HdrTexture(const std::string& path)
	{
		// create hdr texture
		{
			stbi_set_flip_vertically_on_load(true);
			int width, height, nrComponents;
			float* data = stbi_loadf(path.c_str(), &width, &height, &nrComponents, 0);
			assert(data);

			glGenTextures(1, &hdrTex);
			glBindTexture(GL_TEXTURE_2D, hdrTex);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, width, height, 0, GL_RGB, GL_FLOAT, data);

			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

			stbi_image_free(data);
		}


		static const glm::mat4 captureProjection = glm::perspective(glm::radians(90.0f), 1.0f, 0.1f, 10.0f);
		static const glm::mat4 captureViews[] =
		{
			glm::lookAt(glm::vec3{ 0.0f, 0.0f, 0.0f }, {  1.0f,  0.0f,  0.0f }, { 0.0f, -1.0f,  0.0f }),
			glm::lookAt(glm::vec3{ 0.0f, 0.0f, 0.0f }, { -1.0f,  0.0f,  0.0f }, { 0.0f, -1.0f,  0.0f }),
			glm::lookAt(glm::vec3{ 0.0f, 0.0f, 0.0f }, {  0.0f,  1.0f,  0.0f }, { 0.0f,  0.0f,  1.0f }),
			glm::lookAt(glm::vec3{ 0.0f, 0.0f, 0.0f }, {  0.0f, -1.0f,  0.0f }, { 0.0f,  0.0f, -1.0f }),
			glm::lookAt(glm::vec3{ 0.0f, 0.0f, 0.0f }, {  0.0f,  0.0f,  1.0f }, { 0.0f, -1.0f,  0.0f }),
			glm::lookAt(glm::vec3{ 0.0f, 0.0f, 0.0f }, {  0.0f,  0.0f, -1.0f }, { 0.0f, -1.0f,  0.0f })
		};

		
		uint32_t fbo = 0;
		glGenFramebuffers(1, &fbo);
		glBindFramebuffer(GL_FRAMEBUFFER, fbo);

		uint32_t rbo = 0;
		glGenRenderbuffers(1, &rbo);
		glBindRenderbuffer(GL_RENDERBUFFER, rbo);

		static Cube cube;

		// convert hdr image to cubemap
		{
			static constexpr uint32_t kCubeMapWidth = 512;
			static constexpr uint32_t kCubeMapHeight = 512;

			glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, kCubeMapWidth, kCubeMapHeight);
			glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rbo);
			
			glGenTextures(1, &hdrCubeMap);
			glBindTexture(GL_TEXTURE_CUBE_MAP, hdrCubeMap);
			for (uint32_t i = 0; i < 6; ++i)
			{
				// note that we store each face with 16 bit floating point values
				glTexImage2D(
					GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
					0,
					GL_RGB16F,
					kCubeMapWidth,
					kCubeMapHeight,
					0,
					GL_RGB,
					GL_FLOAT,
					nullptr);
			}
			glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
			glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);


			static Shader shader("shader/cubemap.vert", "shader/hdr_input.frag");

			shader.bind();

			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, hdrTex);
			shader.setValue("equirectangularMap", 0);

			shader.setValue("proj", captureProjection);


			glBindFramebuffer(GL_FRAMEBUFFER, fbo);
			glBindRenderbuffer(GL_RENDERBUFFER, rbo);
			glViewport(0, 0, kCubeMapWidth, kCubeMapHeight);
			for (uint32_t i = 0; i < 6; ++i)
			{
				glFramebufferTexture2D(
					GL_FRAMEBUFFER,
					GL_COLOR_ATTACHMENT0,
					GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
					hdrCubeMap,
					0);
				glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

				shader.setValue("view", captureViews[i]);

				glBindVertexArray(cube.vao);
				glDrawArrays(GL_TRIANGLES, 0, 36);
			}
			glBindFramebuffer(GL_FRAMEBUFFER, 0);
		}
		// prefilter will use the mipmap
		glBindTexture(GL_TEXTURE_CUBE_MAP, hdrCubeMap);
		glGenerateMipmap(GL_TEXTURE_CUBE_MAP);


		// generate irradiance map
		{
			glGenTextures(1, &irradianceMap);
			glBindTexture(GL_TEXTURE_CUBE_MAP, irradianceMap);
			for (uint32_t i = 0; i < 6; ++i)
			{
				glTexImage2D(
					GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
					0,
					GL_RGB16F,
					32,
					32,
					0,
					GL_RGB,
					GL_FLOAT,
					nullptr);
			}
			glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);


			static Shader shader("shader/cubemap.vert", "shader/irradiance_convolution.frag");
			shader.bind();

			shader.setValue("proj", captureProjection);

			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_CUBE_MAP, hdrCubeMap);
			shader.setValue("HdrCubeMap", 0);


			glViewport(0, 0, 32, 32); // don't forget to configure the viewport to the capture dimensions.
			glBindFramebuffer(GL_FRAMEBUFFER, fbo);
			glBindRenderbuffer(GL_RENDERBUFFER, rbo);
			glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, 32, 32);

			for (uint32_t i = 0; i < 6; ++i)
			{
				glFramebufferTexture2D(
					GL_FRAMEBUFFER,
					GL_COLOR_ATTACHMENT0,
					GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
					irradianceMap,
					0);
				glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

				shader.setValue("view", captureViews[i]);

				glBindVertexArray(cube.vao);
				glDrawArrays(GL_TRIANGLES, 0, 36);
			}

			glBindFramebuffer(GL_FRAMEBUFFER, 0);
		}


		// prefilter cubemap
		{
			static constexpr uint32_t kPrefilterResolution = 128;

			glGenTextures(1, &prefilterMap);
			glBindTexture(GL_TEXTURE_CUBE_MAP, prefilterMap);
			for (uint32_t i = 0; i < 6; ++i)
			{
				glTexImage2D(
					GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
					0,
					GL_RGB16F,
					kPrefilterResolution,
					kPrefilterResolution,
					0,
					GL_RGB,
					GL_FLOAT,
					nullptr);
			}
			glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
			glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

			glGenerateMipmap(GL_TEXTURE_CUBE_MAP);


			static Shader shader("shader/cubemap.vert", "shader/prefilter.frag");
			shader.bind();

			shader.setValue("proj", captureProjection);

			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_CUBE_MAP, hdrCubeMap);
			shader.setValue("HdrCubeMap", 0);


			static constexpr uint32_t kMipmapCount = 5;
			glBindFramebuffer(GL_FRAMEBUFFER, fbo);
			for (uint32_t mipLevel = 0; mipLevel < kMipmapCount; ++mipLevel)
			{
				uint32_t mipRes = (kPrefilterResolution >> mipLevel);

				glViewport(0, 0, mipRes, mipRes);

				glBindRenderbuffer(GL_RENDERBUFFER, rbo);
				glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, mipRes, mipRes);

				const float roughness = (float)mipLevel / (kMipmapCount - 1.0f);
				shader.setValue("roughness", roughness);
				shader.setValue("resolution", mipRes);

				for (uint32_t i = 0; i < 6; ++i)
				{
					glFramebufferTexture2D(
						GL_FRAMEBUFFER,
						GL_COLOR_ATTACHMENT0,
						GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
						prefilterMap,
						mipLevel);
					glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

					shader.setValue("view", captureViews[i]);

					glBindVertexArray(cube.vao);
					glDrawArrays(GL_TRIANGLES, 0, 36);
				}
			}
			glBindFramebuffer(GL_FRAMEBUFFER, 0);
		}


		glDeleteRenderbuffers(1, &rbo);
		glDeleteFramebuffers(1, &fbo);

		glViewport(0, 0, kScreenWidth, kScreenHeight);
	}

	~HdrTexture()
	{
		glDeleteTextures(1, &prefilterMap);
		glDeleteTextures(1, &irradianceMap);
		glDeleteTextures(1, &hdrTex);
		glDeleteBuffers(1, &hdrCubeMap);
	}
};