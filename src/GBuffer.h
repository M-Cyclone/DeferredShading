#pragma once
#include <cassert>
#include <algorithm>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

extern uint32_t kScreenWidth;
extern uint32_t kScreenHeight;

struct GBuffer
{
	uint32_t framebuffer = 0;

	uint32_t gPosition = 0;
	uint32_t gBasecolor = 0;
	uint32_t gNormal = 0;
	uint32_t gMetallicRoughness = 0;

	uint32_t depthRenderBuffer = 0;

	GBuffer()
	{
		glGenFramebuffers(1, &framebuffer);
		glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);

		glGenTextures(1, &gPosition);
		glBindTexture(GL_TEXTURE_2D, gPosition);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, kScreenWidth, kScreenHeight, 0, GL_RGB, GL_FLOAT, nullptr);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, gPosition, 0);

		glGenTextures(1, &gBasecolor);
		glBindTexture(GL_TEXTURE_2D, gBasecolor);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, kScreenWidth, kScreenHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, gBasecolor, 0);

		glGenTextures(1, &gNormal);
		glBindTexture(GL_TEXTURE_2D, gNormal);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, kScreenWidth, kScreenHeight, 0, GL_RGB, GL_FLOAT, nullptr);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, gNormal, 0);

		glGenTextures(1, &gMetallicRoughness);
		glBindTexture(GL_TEXTURE_2D, gMetallicRoughness);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RG32F, kScreenWidth, kScreenHeight, 0, GL_RGB, GL_FLOAT, nullptr);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT3, GL_TEXTURE_2D, gMetallicRoughness, 0);

		GLuint attachments[] =
		{
			GL_COLOR_ATTACHMENT0,
			GL_COLOR_ATTACHMENT1,
			GL_COLOR_ATTACHMENT2,
			GL_COLOR_ATTACHMENT3,
		};
		glDrawBuffers(static_cast<GLsizei>(std::size(attachments)), attachments);

		glGenRenderbuffers(1, &depthRenderBuffer);
		glBindRenderbuffer(GL_RENDERBUFFER, depthRenderBuffer);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, kScreenWidth, kScreenHeight);
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthRenderBuffer);

		assert(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	~GBuffer()
	{
		glDeleteTextures(1, &gPosition);
		glDeleteTextures(1, &gBasecolor);
		glDeleteTextures(1, &gNormal);
		glDeleteTextures(1, &gMetallicRoughness);
		glDeleteRenderbuffers(1, &depthRenderBuffer);
		glDeleteFramebuffers(1, &framebuffer);
	}

	void begin()
	{
		glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
	}

	void end()
	{
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}
};