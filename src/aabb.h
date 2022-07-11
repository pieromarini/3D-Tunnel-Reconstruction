#include <iostream>
#include <glm/glm.h>

class AABB {
public:
  AABB(glm::vec3 max, glm::vec3 min): vecMax{max}, vecMin{min} {}

  bool overlap(const AABB& tBox2) {
      return(vecMax.x > tBox2.vecMin.x &&
	     vecMin.x < tBox2.vecMax.x &&
	     vecMax.y > tBox2.vecMin.y &&
	     vecMin.y < tBox2.vecMax.y &&
	     vecMax.z > tBox2.vecMin.z &&
	     vecMin.z < tBox2.vecMax.z);
  }

private:
  glm::vec3 vecMax;
  glm::vec3 vecMin;
};
