#include <cstdlib>
#include <iostream>

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "gl_debug.h"
#include "shader.h"
#include "structures.h"
#include "window.h"
#include "camera.h"
#include "model.h"

#include "framebuffer.h"

class Renderer {
 public:
  Renderer(Window& w, Camera& c) : window{ w }, camera{ c } {
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
	  std::cout << "Failed to initialize GLAD" << std::endl;
	  exit(0);
	}

	int flags;
	glGetIntegerv(GL_CONTEXT_FLAGS, &flags);
	if (flags & GL_CONTEXT_FLAG_DEBUG_BIT) {
	  glEnable(GL_DEBUG_OUTPUT);
	  glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
	  glDebugMessageCallback(glDebugOutput, nullptr);
	  glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, nullptr, GL_TRUE);
	}
  }

  void clear() {
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  }

  void setupContext() {
	glEnable(GL_DEPTH_TEST);
  }

  void update() {
	if (window.wireframe) {
	  glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	} else {

	  glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	}
  }

  void renderCube(Shader& shader, glm::vec3 position) {
	if (cubeVAO == 0) {
	  float vertices[] = {
		// back face
		-1.0f,
		-1.0f,
		-1.0f,
		0.0f,
		0.0f,
		-1.0f,
		0.0f,
		0.0f,// bottom-left
		1.0f,
		1.0f,
		-1.0f,
		0.0f,
		0.0f,
		-1.0f,
		1.0f,
		1.0f,// top-right
		1.0f,
		-1.0f,
		-1.0f,
		0.0f,
		0.0f,
		-1.0f,
		1.0f,
		0.0f,// bottom-right
		1.0f,
		1.0f,
		-1.0f,
		0.0f,
		0.0f,
		-1.0f,
		1.0f,
		1.0f,// top-right
		-1.0f,
		-1.0f,
		-1.0f,
		0.0f,
		0.0f,
		-1.0f,
		0.0f,
		0.0f,// bottom-left
		-1.0f,
		1.0f,
		-1.0f,
		0.0f,
		0.0f,
		-1.0f,
		0.0f,
		1.0f,// top-left
		// front face
		-1.0f,
		-1.0f,
		1.0f,
		0.0f,
		0.0f,
		1.0f,
		0.0f,
		0.0f,// bottom-left
		1.0f,
		-1.0f,
		1.0f,
		0.0f,
		0.0f,
		1.0f,
		1.0f,
		0.0f,// bottom-right
		1.0f,
		1.0f,
		1.0f,
		0.0f,
		0.0f,
		1.0f,
		1.0f,
		1.0f,// top-right
		1.0f,
		1.0f,
		1.0f,
		0.0f,
		0.0f,
		1.0f,
		1.0f,
		1.0f,// top-right
		-1.0f,
		1.0f,
		1.0f,
		0.0f,
		0.0f,
		1.0f,
		0.0f,
		1.0f,// top-left
		-1.0f,
		-1.0f,
		1.0f,
		0.0f,
		0.0f,
		1.0f,
		0.0f,
		0.0f,// bottom-left
		// left face
		-1.0f,
		1.0f,
		1.0f,
		-1.0f,
		0.0f,
		0.0f,
		1.0f,
		0.0f,// top-right
		-1.0f,
		1.0f,
		-1.0f,
		-1.0f,
		0.0f,
		0.0f,
		1.0f,
		1.0f,// top-left
		-1.0f,
		-1.0f,
		-1.0f,
		-1.0f,
		0.0f,
		0.0f,
		0.0f,
		1.0f,// bottom-left
		-1.0f,
		-1.0f,
		-1.0f,
		-1.0f,
		0.0f,
		0.0f,
		0.0f,
		1.0f,// bottom-left
		-1.0f,
		-1.0f,
		1.0f,
		-1.0f,
		0.0f,
		0.0f,
		0.0f,
		0.0f,// bottom-right
		-1.0f,
		1.0f,
		1.0f,
		-1.0f,
		0.0f,
		0.0f,
		1.0f,
		0.0f,// top-right
		// right face
		1.0f,
		1.0f,
		1.0f,
		1.0f,
		0.0f,
		0.0f,
		1.0f,
		0.0f,// top-left
		1.0f,
		-1.0f,
		-1.0f,
		1.0f,
		0.0f,
		0.0f,
		0.0f,
		1.0f,// bottom-right
		1.0f,
		1.0f,
		-1.0f,
		1.0f,
		0.0f,
		0.0f,
		1.0f,
		1.0f,// top-right
		1.0f,
		-1.0f,
		-1.0f,
		1.0f,
		0.0f,
		0.0f,
		0.0f,
		1.0f,// bottom-right
		1.0f,
		1.0f,
		1.0f,
		1.0f,
		0.0f,
		0.0f,
		1.0f,
		0.0f,// top-left
		1.0f,
		-1.0f,
		1.0f,
		1.0f,
		0.0f,
		0.0f,
		0.0f,
		0.0f,// bottom-left
		// bottom face
		-1.0f,
		-1.0f,
		-1.0f,
		0.0f,
		-1.0f,
		0.0f,
		0.0f,
		1.0f,// top-right
		1.0f,
		-1.0f,
		-1.0f,
		0.0f,
		-1.0f,
		0.0f,
		1.0f,
		1.0f,// top-left
		1.0f,
		-1.0f,
		1.0f,
		0.0f,
		-1.0f,
		0.0f,
		1.0f,
		0.0f,// bottom-left
		1.0f,
		-1.0f,
		1.0f,
		0.0f,
		-1.0f,
		0.0f,
		1.0f,
		0.0f,// bottom-left
		-1.0f,
		-1.0f,
		1.0f,
		0.0f,
		-1.0f,
		0.0f,
		0.0f,
		0.0f,// bottom-right
		-1.0f,
		-1.0f,
		-1.0f,
		0.0f,
		-1.0f,
		0.0f,
		0.0f,
		1.0f,// top-right
		// top face
		-1.0f,
		1.0f,
		-1.0f,
		0.0f,
		1.0f,
		0.0f,
		0.0f,
		1.0f,// top-left
		1.0f,
		1.0f,
		1.0f,
		0.0f,
		1.0f,
		0.0f,
		1.0f,
		0.0f,// bottom-right
		1.0f,
		1.0f,
		-1.0f,
		0.0f,
		1.0f,
		0.0f,
		1.0f,
		1.0f,// top-right
		1.0f,
		1.0f,
		1.0f,
		0.0f,
		1.0f,
		0.0f,
		1.0f,
		0.0f,// bottom-right
		-1.0f,
		1.0f,
		-1.0f,
		0.0f,
		1.0f,
		0.0f,
		0.0f,
		1.0f,// top-left
		-1.0f,
		1.0f,
		1.0f,
		0.0f,
		1.0f,
		0.0f,
		0.0f,
		0.0f// bottom-left
	  };
	  glGenVertexArrays(1, &cubeVAO);
	  glGenBuffers(1, &cubeVBO);
	  // fill buffer
	  glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
	  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	  // link vertex attributes
	  glBindVertexArray(cubeVAO);
	  glEnableVertexAttribArray(0);
	  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	  glEnableVertexAttribArray(1);
	  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
	  glEnableVertexAttribArray(2);
	  glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
	  glBindBuffer(GL_ARRAY_BUFFER, 0);
	  glBindVertexArray(0);
	}
	shader.use();

	auto modelMat = glm::mat4(1.0f);
	modelMat = glm::translate(modelMat, position);
	modelMat = glm::scale(modelMat, glm::vec3(0.15f));
	shader.setMat4("model", modelMat);
	// render Cube
	glBindVertexArray(cubeVAO);
	glDrawArrays(GL_TRIANGLES, 0, 36);
	glBindVertexArray(0);
  }

  void renderPoints(Shader& shader, std::vector<Vector3D*>& points) {
	std::vector<double> vertices;

	for (auto point : points) {
	  vertices.push_back(point->X);
	  vertices.push_back(point->Y);
	  vertices.push_back(point->Z);
	}

	unsigned int VAO = 0;
	unsigned int VBO = 0;
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);

	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(double), vertices.data(), GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_DOUBLE, GL_FALSE, 3 * sizeof(double), (void*)0);

	shader.use();

	auto modelMat = glm::mat4(1.0f);
	modelMat = glm::translate(modelMat, glm::vec3(1.0f));
	modelMat = glm::scale(modelMat, glm::vec3(1.0f));
	shader.setMat4("model", modelMat);
	glBindVertexArray(VAO);
	glDrawArrays(GL_POINTS, 0, vertices.size() / 3);

	// reset
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	glDeleteBuffers(1, &VBO);
	glDeleteVertexArrays(1, &VAO);
  }

  void renderMesh(Shader& shader, std::vector<TriangleList*>& mesh) {
	std::vector<double> vertices;

	for (auto triangle : mesh) {
	  auto one = std::get<0>(*triangle);
	  auto two = std::get<1>(*triangle);
	  auto three = std::get<2>(*triangle);

	  // Calculate surface normal
	  auto BA = glm::dvec3(two->X, two->Y, two->Z) - glm::dvec3(one->X, one->Y, one->Z);
	  auto CA = glm::dvec3(three->X, three->Y, three->Z) - glm::dvec3(one->X, one->Y, one->Z);
	  auto dir = glm::cross(BA, CA);
	  auto normal = -glm::normalize(dir);

	  vertices.push_back(one->X);
	  vertices.push_back(one->Y);
	  vertices.push_back(one->Z);

	  vertices.push_back(normal.x);
	  vertices.push_back(normal.y);
	  vertices.push_back(normal.z);

	  vertices.push_back(two->X);
	  vertices.push_back(two->Y);
	  vertices.push_back(two->Z);

	  vertices.push_back(normal.x);
	  vertices.push_back(normal.y);
	  vertices.push_back(normal.z);

	  vertices.push_back(three->X);
	  vertices.push_back(three->Y);
	  vertices.push_back(three->Z);

	  vertices.push_back(normal.x);
	  vertices.push_back(normal.y);
	  vertices.push_back(normal.z);
	}

	unsigned int VAO = 0;
	unsigned int VBO = 0;
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);

	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(double), vertices.data(), GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_DOUBLE, GL_FALSE, 3 * sizeof(double), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_DOUBLE, GL_FALSE, 6 * sizeof(double), (void*)(3 * sizeof(double)));
	glEnableVertexAttribArray(1);

	shader.use();

	auto modelMat = glm::mat4(1.0f);
	modelMat = glm::translate(modelMat, glm::vec3(1.0f));
	modelMat = glm::scale(modelMat, glm::vec3(1.0f));
	shader.setMat4("model", modelMat);
	glBindVertexArray(VAO);
	glDrawArrays(GL_TRIANGLES, 0, vertices.size() / 3);

	// reset
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	glDeleteBuffers(1, &VBO);
	glDeleteVertexArrays(1, &VAO);
  }

  void renderQuad() {
	if (quadVAO == 0) {
	  float quadVertices[] = {
		// positions        // texture Coords
		-1.0f,
		1.0f,
		0.0f,
		0.0f,
		1.0f,
		-1.0f,
		-1.0f,
		0.0f,
		0.0f,
		0.0f,
		1.0f,
		1.0f,
		0.0f,
		1.0f,
		1.0f,
		1.0f,
		-1.0f,
		0.0f,
		1.0f,
		0.0f,
	  };
	  // setup plane VAO
	  glGenVertexArrays(1, &quadVAO);
	  glGenBuffers(1, &quadVBO);
	  glBindVertexArray(quadVAO);
	  glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
	  glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
	  glEnableVertexAttribArray(0);
	  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
	  glEnableVertexAttribArray(1);
	  glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
	}
	glBindVertexArray(quadVAO);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	glBindVertexArray(0);
  }

  unsigned int width() { return window.width(); }
  unsigned int height() { return window.height(); }

 private:
  Window& window;
  Camera& camera;
  unsigned int gBuffer;
  unsigned int gPosition, gNormal, gAlbedoSpec;
  unsigned int rboDepth;
  std::vector<glm::vec3> lightPositions;
  std::vector<glm::vec3> lightColors;
  unsigned int cubeVAO = 0;
  unsigned int cubeVBO = 0;
  unsigned int quadVAO = 0;
  unsigned int quadVBO = 0;
};
