#pragma once

#include "camera.h"
#include <cstdlib>
#include <iostream>

#include <GLFW/glfw3.h>
#include <glad/glad.h>
#include <glm/glm.hpp>

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);

constexpr unsigned int SCREEN_WIDTH = 1920;
constexpr unsigned int SCREEN_HEIGHT = 1080;

float lastX = (float)SCREEN_WIDTH / 2.0;
float lastY = (float)SCREEN_WIDTH / 2.0;
bool firstMouse = true;
Camera camera(glm::vec3(0.0f, 0.0f, 5.0f));

class Window {

 public:
  Window(unsigned int width, unsigned int height) {
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 4);
	glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, true);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	windowHandle = glfwCreateWindow(width, height, "Proyecto 2", nullptr, nullptr);
	if (windowHandle == nullptr) {
	  std::cout << "Failed to create GLFW window" << std::endl;
	  glfwTerminate();
	  exit(0);
	}

	glfwSetWindowUserPointer(windowHandle, reinterpret_cast<void*>(this));

	glfwMakeContextCurrent(windowHandle);
	glfwSetFramebufferSizeCallback(windowHandle, framebuffer_size_callback);
	glfwSetCursorPosCallback(windowHandle, mouse_callback);
	glfwSetScrollCallback(windowHandle, scroll_callback);
	glfwSetKeyCallback(windowHandle, key_callback);

	glfwSetInputMode(windowHandle, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
  }

  GLFWwindow* handle() { return windowHandle; }

  void processInput(Camera& camera, float deltaTime) {
	if (glfwGetKey(windowHandle, GLFW_KEY_ESCAPE) == GLFW_PRESS)
	  glfwSetWindowShouldClose(windowHandle, true);

	if (glfwGetKey(windowHandle, GLFW_KEY_W) == GLFW_PRESS)
	  camera.ProcessKeyboard(FORWARD, deltaTime);
	if (glfwGetKey(windowHandle, GLFW_KEY_S) == GLFW_PRESS)
	  camera.ProcessKeyboard(BACKWARD, deltaTime);
	if (glfwGetKey(windowHandle, GLFW_KEY_A) == GLFW_PRESS)
	  camera.ProcessKeyboard(LEFT, deltaTime);
	if (glfwGetKey(windowHandle, GLFW_KEY_D) == GLFW_PRESS)
	  camera.ProcessKeyboard(RIGHT, deltaTime);
  }

  bool shouldClose() {
	return glfwWindowShouldClose(windowHandle);
  }

  int width() {
	int width, height;
	glfwGetFramebufferSize(windowHandle, &width, &height);
	return width;
  }

  int height() {
	int width, height;
	glfwGetFramebufferSize(windowHandle, &width, &height);
	return height;
  }

  bool wireframe = false;
  bool renderMesh = true;
  bool renderPoints = false;
  bool renderLights = true;
  bool refresh = false;

 private:
  GLFWwindow* windowHandle;
};


inline void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
  glViewport(0, 0, width, height);
}

inline void mouse_callback(GLFWwindow* window, double xposIn, double yposIn) {
  float xpos = static_cast<float>(xposIn);
  float ypos = static_cast<float>(yposIn);
  if (firstMouse) {
	lastX = xpos;
	lastY = ypos;
	firstMouse = false;
  }

  float xoffset = xpos - lastX;
  float yoffset = lastY - ypos;

  lastX = xpos;
  lastY = ypos;

  camera.ProcessMouseMovement(xoffset, yoffset);
}

inline void scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
  camera.ProcessMouseScroll(static_cast<float>(yoffset));
}

inline void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {

  auto* handler = reinterpret_cast<Window*>(glfwGetWindowUserPointer(window));
  if (handler) {
	if (key == GLFW_KEY_I && action == GLFW_RELEASE) {
	  handler->wireframe = !(handler->wireframe);
	}
	if (key == GLFW_KEY_P && action == GLFW_RELEASE) {
	  handler->renderPoints = !(handler->renderPoints);
	}
	if (key == GLFW_KEY_M && action == GLFW_RELEASE) {
	  handler->renderMesh = !(handler->renderMesh);
	}
	if (key == GLFW_KEY_L && action == GLFW_RELEASE) {
	  handler->renderLights = !(handler->renderLights);
	}
	if (key == GLFW_KEY_R && action == GLFW_RELEASE) {
	  handler->refresh = !(handler->refresh);
	}
  }
}
