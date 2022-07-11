#pragma once

#include <array>
#include <vector>

#include <glm/glm.hpp>

struct Triangle : std::array<glm::vec3, 3> {
  // Calculate surface normal
  glm::vec3 normal() const {
	return glm::normalize(glm::cross((*this)[0] - (*this)[1], (*this)[0] - (*this)[2]));
  }
};

struct Point {
  Point(glm::vec3 p, glm::vec3 n) : pos{ p }, normal{ n } {}
  glm::vec3 pos;
  glm::vec3 normal;
};

std::vector<Triangle> reconstruct(const std::vector<Point>& points, float radius);
std::vector<Triangle> measuredReconstruct(const std::vector<Point>& points, float radius);
