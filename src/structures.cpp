#include <string>

#include "structures.h"

Vector3D::Vector3D(double x, double y, double z, uint8_t r, uint8_t g, uint8_t b) {
  Id = GenerateRunningId();

  X = x;
  Y = y;
  Z = z;

  R = r;
  G = g;
  B = b;
}

Vector3D::Vector3D(double x, double y, double z, bool isAuxiliaryDot, uint8_t r, uint8_t g, uint8_t b) {
  Id = GenerateRunningId();

  IsAuxiliaryDot = isAuxiliaryDot;

  X = x;
  Y = y;
  Z = z;

  R = r;
  G = g;
  B = b;
}

Vector3D::Vector3D(Vector3D* dot, double lengthAfterProjection) {
  Id = dot->Id;
  IsVisited = dot->IsVisited;
  IsAuxiliaryDot = dot->IsAuxiliaryDot;

  double length = sqrt(pow(dot->X, 2) + pow(dot->Y, 2) + pow(dot->Z, 2));
  double scaleFactor = lengthAfterProjection / length;

  X = scaleFactor * dot->X;
  Y = scaleFactor * dot->Y;
  Z = scaleFactor * dot->Z;

  R = dot->R;
  G = dot->G;
  B = dot->B;
}

Vector3D::~Vector3D() {
}

int Vector3D::GenerateRunningId() {
  static int id = 0;
  return id++;
}

bool Vector3D::IsCoincidentWith(Vector3D* dot) {
  return (X == dot->X && Y == dot->Y && Z == dot->Z);
}

std::string Vector3D::ToString() {
  return "Dot ID: " + std::to_string(Id) + "; "
		 + std::to_string(X) + ", " + std::to_string(Y) + ", " + std::to_string(Z) + "; "
		 + std::to_string(R) + ", " + std::to_string(G) + ", " + std::to_string(B) + "; "
		 + "IsVisited: " + (IsVisited ? "true" : "false") + "; "
		 + "IsAuxiliaryDot: " + (IsAuxiliaryDot ? "true" : "false") + ";\n";
}


Triangle::Triangle(Vector3D* v0, Vector3D* v1, Vector3D* v2) {
  Id = GenerateRunningId();
  Vertex[0] = v0;
  Vertex[1] = v1;
  Vertex[2] = v2;
}

Triangle::~Triangle() {
}

int Triangle::GenerateRunningId() {
  static int id = 0;
  return id++;
}

bool Triangle::HasVertexCoincidentWith(Vector3D* dot) {
  return Vertex[0]->IsCoincidentWith(dot)
		 || Vertex[1]->IsCoincidentWith(dot)
		 || Vertex[2]->IsCoincidentWith(dot);
}

void Triangle::AssignNeighbors(Triangle* n0, Triangle* n1, Triangle* n2) {
  Neighbor[0] = n0;
  Neighbor[1] = n1;
  Neighbor[2] = n2;
}

std::string Triangle::ToString() {
  return "Triangle ID: " + std::to_string(Id) + ";\n"
		 + "Vertex[0]: " + Vertex[0]->ToString()
		 + "Vertex[1]: " + Vertex[1]->ToString()
		 + "Vertex[2]: " + Vertex[2]->ToString()
		 + "Neighbor[0] ID: " + std::to_string(Neighbor[0]->Id) + ", "
		 + "Neighbor[1] ID: " + std::to_string(Neighbor[1]->Id) + ", "
		 + "Neighbor[2] ID: " + std::to_string(Neighbor[2]->Id) + ";\n";
}
