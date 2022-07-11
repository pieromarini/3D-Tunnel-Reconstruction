#include <vector>
#include <random>
#include <glad/glad.h>
#include <math.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "renderer.h"
#include "window.h"
#include "shader.h"
#include "camera.h"
#include "model.h"

#include "framebuffer.h"
#include "gl_debug.h"

// #include "triangulate.h"
#include "bpa.h"

#include <iostream>

std::vector<Point> genSphericalCloud(int slices, int stacks) {
  std::vector<Point> points;
  points.emplace_back(Point{ { 0, 0, -1 }, { 0, 0, -1 } });
  for (auto slice = 0; slice < slices; slice++) {
	for (auto stack = 1; stack < stacks; stack++) {
	  const auto yaw = (static_cast<double>(slice) / slices) * 2 * std::numbers::pi;
	  const auto z = std::sin((static_cast<double>(stack) / stacks - 0.5) * std::numbers::pi);
	  const auto r = std::sqrt(1 - z * z);

	  glm::vec3 v;
	  v.x = static_cast<float>(r * std::sin(yaw));
	  v.y = static_cast<float>(r * std::cos(yaw));
	  v.z = static_cast<float>(z);
	  points.push_back({ v, glm::normalize(v - glm::vec3{}) });
	}
  }
  points.emplace_back(Point{ { 0, 0, 1 }, { 0, 0, 1 } });
  return points;
}

std::vector<Point> genRandomPointCloud(std::size_t numPoints) {
  std::vector<Point> out{};

  std::random_device rd{};
  std::mt19937 engine{ rd() };

  std::uniform_real_distribution<double> distAngle{ 0.0, 360.0 };
  std::uniform_real_distribution<double> dist{ -4.0, 4.0 };

  double radius = 2.0;

  for (std::size_t i = 0; i < numPoints; ++i) {
	auto theta = glm::radians(distAngle(engine));
	auto r = dist(engine);

	auto x = radius * std::cos(theta);
	auto y = radius * std::sin(theta);
	auto z = r;

	glm::vec3 normal{ 0.0f, 1.0f, 0.0f };

	out.emplace_back(glm::vec3{ x, y, z }, normal);
  }

  return out;
}

int main() {
  Window window{ SCREEN_WIDTH, SCREEN_HEIGHT };

  Renderer renderer{ window, camera };
  renderer.setupContext();

  Shader shader("res/shaders/basic.vs.glsl", "res/shaders/basic.fs.glsl");
  Shader lightShader("res/shaders/light_box.vs.glsl", "res/shaders/light_box.fs.glsl");

  std::size_t numPoints = 20000;

  glm::vec3 pointLightPositions[] = {
	glm::vec3(0.7f, 0.2f, 2.0f),
	glm::vec3(2.3f, -3.3f, -4.0f),
	glm::vec3(-4.0f, 2.0f, -12.0f),
	glm::vec3(0.0f, 0.0f, -3.0f)
  };

  glm::vec3 color{ 1.0f, 1.0f, 1.0f };
  lightShader.use();
  lightShader.setVec3("lightColor", color);

  // Gen cloud
  auto cloud = genRandomPointCloud(numPoints);
  // auto cloud = genSphericalCloud(200, 100);

  // Triangulate
  /*
	auto triangulation = DelaunayTriangulation();
	auto mesh = triangulation.GetTriangulationResult(cloud);
	std::cout << triangulation.GetStatistics() << '\n';
  */
  auto mesh = measuredReconstruct(cloud, 0.095f);

  shader.use();

  float deltaTime = 0.0f;
  float lastFrame = 0.0f;
  while (!window.shouldClose()) {
	auto currentFrame = static_cast<float>(glfwGetTime());
	deltaTime = currentFrame - lastFrame;
	lastFrame = currentFrame;

	/*
	if (window.refresh) {
	  window.refresh = false;
	  triangulation = DelaunayTriangulation();
	  mesh = triangulation.GetTriangulationResult(cloud);
	  std::cout << triangulation.GetStatistics() << '\n';
	}
	*/

	renderer.update();

	glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	shader.use();
	shader.setVec3("viewPos", camera.Position);
	shader.setFloat("material.shininess", 32.0f);
	shader.setVec3("dirLight.direction", -0.2f, -1.0f, -0.3f);
	shader.setVec3("dirLight.ambient", 0.6f, 0.05f, 0.05f);
	shader.setVec3("dirLight.diffuse", 0.4f, 0.4f, 0.4f);
	shader.setVec3("dirLight.specular", 0.5f, 0.5f, 0.5f);
	// point light 1
	shader.setVec3("pointLights[0].position", pointLightPositions[0]);
	shader.setVec3("pointLights[0].ambient", 0.05f, 0.05f, 0.05f);
	shader.setVec3("pointLights[0].diffuse", 0.8f, 0.8f, 0.8f);
	shader.setVec3("pointLights[0].specular", 1.0f, 1.0f, 1.0f);
	shader.setFloat("pointLights[0].constant", 1.0f);
	shader.setFloat("pointLights[0].linear", 0.09f);
	shader.setFloat("pointLights[0].quadratic", 0.032f);
	// point light 2
	shader.setVec3("pointLights[1].position", pointLightPositions[1]);
	shader.setVec3("pointLights[1].ambient", 0.05f, 0.05f, 0.05f);
	shader.setVec3("pointLights[1].diffuse", 0.8f, 0.8f, 0.8f);
	shader.setVec3("pointLights[1].specular", 1.0f, 1.0f, 1.0f);
	shader.setFloat("pointLights[1].constant", 1.0f);
	shader.setFloat("pointLights[1].linear", 0.09f);
	shader.setFloat("pointLights[1].quadratic", 0.032f);
	// point light 3
	shader.setVec3("pointLights[2].position", pointLightPositions[2]);
	shader.setVec3("pointLights[2].ambient", 0.05f, 0.05f, 0.05f);
	shader.setVec3("pointLights[2].diffuse", 0.8f, 0.8f, 0.8f);
	shader.setVec3("pointLights[2].specular", 1.0f, 1.0f, 1.0f);
	shader.setFloat("pointLights[2].constant", 1.0f);
	shader.setFloat("pointLights[2].linear", 0.09f);
	shader.setFloat("pointLights[2].quadratic", 0.032f);
	// point light 4
	shader.setVec3("pointLights[3].position", pointLightPositions[3]);
	shader.setVec3("pointLights[3].ambient", 0.05f, 0.05f, 0.05f);
	shader.setVec3("pointLights[3].diffuse", 0.8f, 0.8f, 0.8f);
	shader.setVec3("pointLights[3].specular", 1.0f, 1.0f, 1.0f);
	shader.setFloat("pointLights[3].constant", 1.0f);
	shader.setFloat("pointLights[3].linear", 0.09f);
	shader.setFloat("pointLights[3].quadratic", 0.032f);

	window.processInput(camera, deltaTime);

	renderer.clear();

	glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), static_cast<float>(SCREEN_WIDTH) / static_cast<float>(SCREEN_HEIGHT), 0.1f, 100.0f);
	glm::mat4 view = camera.GetViewMatrix();
	shader.use();
	shader.setMat4("projection", projection);
	shader.setMat4("view", view);

	if (window.renderMesh) {
	  renderer.renderMesh(shader, mesh);
	}

	lightShader.use();
	lightShader.setMat4("projection", projection);
	lightShader.setMat4("view", view);

	if (window.renderLights) {
	  for (auto pos : pointLightPositions) {
		renderer.renderCube(lightShader, pos);
	  }
	}

	if (window.renderPoints) {
	  renderer.renderPoints(lightShader, cloud);
	}


	glfwSwapBuffers(window.handle());
	glfwPollEvents();
  }

  glfwTerminate();
  return 0;
}
