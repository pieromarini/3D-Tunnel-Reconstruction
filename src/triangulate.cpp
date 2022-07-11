#include <iostream>
#include <limits>
#include <regex>
#include <string>
#include <tuple>
#include <vector>
#include "triangulate.h"


DelaunayTriangulation::DelaunayTriangulation() {
  for (int i = 0; i < INIT_VERTICES_COUNT; i++) {
	_AuxiliaryDots[i] = new Vector3D(
	  (i % 2 == 0 ? 1 : -1) * (i / 2 == 0 ? VECTOR_LENGTH : 0),
	  (i % 2 == 0 ? 1 : -1) * (i / 2 == 1 ? VECTOR_LENGTH : 0),
	  (i % 2 == 0 ? 1 : -1) * (i / 2 == 2 ? VECTOR_LENGTH : 0),
	  true,
	  0,
	  0,
	  0);
  }

  m_projectedDots = new std::vector<Vector3D*>();
  m_mesh = new std::vector<Triangle*>();

  for (int i = 0; i < sizeof(_Statistics) / sizeof(long); i++) {
	_Statistics[i] = 0;
  }
}

DelaunayTriangulation::~DelaunayTriangulation() {
  for (int i = 0; i < INIT_VERTICES_COUNT; i++) {
	delete _AuxiliaryDots[i];
  }

  for (auto itDots = m_projectedDots->begin(); itDots != m_projectedDots->end(); itDots++) {
	delete *itDots;
  }

  for (auto itMesh = m_mesh->begin(); itMesh != m_mesh->end(); itMesh++) {
	delete *itMesh;
  }

  delete m_projectedDots;
  delete m_mesh;
}

std::vector<TriangleList*> DelaunayTriangulation::GetTriangulationResult(std::vector<Vector3D*>& dots) {
  _Statistics[2] = clock();

  m_projectedDots->reserve(dots.size());

  // N random dots can form 8+(N-6)*2 triangles based on the algorithm
  m_mesh->reserve(8 + (dots.size() - 6) * 2);

  // project dots to a unit sphere for triangulation
  for (auto dot : dots) {
	Vector3D* projectedDot = new Vector3D((dot), VECTOR_LENGTH);
	m_projectedDots->push_back(dot);
  }

  // prepare initial convex hull with 6 vertices and 8 triangle faces
  BuildInitialHull(m_projectedDots);

  for (auto dot : *m_projectedDots) {
	if (!dot->IsVisited) {
	  InsertDot(dot);
	}
  }

  // remove trianges connected with auxiliary dots
  RemoveExtraTriangles();

  // generate output
  auto mesh = std::vector<TriangleList*>();
  std::vector<Triangle*>::iterator itMesh;
  for (auto triangle : *m_mesh) {
	mesh.push_back(new TriangleList(
	  triangle->Vertex[0],
	  triangle->Vertex[1],
	  triangle->Vertex[2]));
  }

  _Statistics[3] = clock();

  return mesh;
}

void DelaunayTriangulation::BuildInitialHull(std::vector<Vector3D*>* dots) {
  Vector3D* initialVertices[INIT_VERTICES_COUNT];
  Triangle* initialHullFaces[INIT_FACES_COUNT];

  for (int i = 0; i < INIT_VERTICES_COUNT; i++) {
	initialVertices[i] = _AuxiliaryDots[i];
  }

  // if close enough, use input dots to replace auxiliary dots so won't be removed in the end
  double minDistance[INIT_VERTICES_COUNT] = { 0, 0, 0, 0, 0, 0 };
  std::vector<Vector3D*>::iterator it;
  for (it = dots->begin(); it != dots->end(); it++) {
	double distance[INIT_VERTICES_COUNT];
	for (int i = 0; i < INIT_VERTICES_COUNT; i++) {
	  distance[i] = GetDistance(_AuxiliaryDots[i], *it);
	  if (minDistance[i] == 0 || distance[i] < minDistance[i]) {
		minDistance[i] = distance[i];
	  }
	}

	for (int i = 0; i < INIT_VERTICES_COUNT; i++) {
	  if (minDistance[i] == distance[i] && IsMinimumValueInArray(distance, INIT_VERTICES_COUNT, i)) {
		initialVertices[i] = *it;
	  }
	}
  }

  int vertex0Index[] = { 0, 0, 0, 0, 1, 1, 1, 1 };
  int vertex1Index[] = { 4, 3, 5, 2, 2, 4, 3, 5 };
  int vertex2Index[] = { 2, 4, 3, 5, 4, 3, 5, 2 };

  for (int i = 0; i < INIT_FACES_COUNT; i++) {
	Vector3D* v0 = initialVertices[vertex0Index[i]];
	Vector3D* v1 = initialVertices[vertex1Index[i]];
	Vector3D* v2 = initialVertices[vertex2Index[i]];

	Triangle* triangle = new Triangle(v0, v1, v2);
	initialHullFaces[i] = triangle;

	m_mesh->push_back(triangle);
  }

  int neighbor0Index[] = { 1, 2, 3, 0, 7, 4, 5, 6 };
  int neighbor1Index[] = { 4, 5, 6, 7, 0, 1, 2, 3 };
  int neighbor2Index[] = { 3, 0, 1, 2, 5, 6, 7, 4 };

  for (int i = 0; i < INIT_FACES_COUNT; i++) {
	Triangle* n0 = initialHullFaces[neighbor0Index[i]];
	Triangle* n1 = initialHullFaces[neighbor1Index[i]];
	Triangle* n2 = initialHullFaces[neighbor2Index[i]];
	initialHullFaces[i]->AssignNeighbors(n0, n1, n2);
  }

  // dot already in the mesh, avoid being visited by InsertDot() again
  for (int i = 0; i < INIT_VERTICES_COUNT; i++) {
	initialVertices[i]->IsVisited = true;
  }
}

void DelaunayTriangulation::InsertDot(Vector3D* dot) {
  double det[] = { 0, 0, 0 };

  std::vector<Triangle*>::iterator it;
  it = m_mesh->begin();
  Triangle* triangle = *it;

  while (it != m_mesh->end()) {
	_Statistics[0]++;

	det[0] = GetDeterminant(triangle->Vertex[0], triangle->Vertex[1], dot);
	det[1] = GetDeterminant(triangle->Vertex[1], triangle->Vertex[2], dot);
	det[2] = GetDeterminant(triangle->Vertex[2], triangle->Vertex[0], dot);

	// if this dot projected into an existing triangle, split the existing triangle to 3 new ones
	if (det[0] >= 0 && det[1] >= 0 && det[2] >= 0) {
	  if (!triangle->HasVertexCoincidentWith(dot)) {
		SplitTriangle(triangle, dot);
	  }

	  return;
	}

	// on one side, search neighbors
	else if (det[1] >= 0 && det[2] >= 0)
	  triangle = triangle->Neighbor[0];
	else if (det[0] >= 0 && det[2] >= 0)
	  triangle = triangle->Neighbor[1];
	else if (det[0] >= 0 && det[1] >= 0)
	  triangle = triangle->Neighbor[2];

	// cannot determine effectively
	else if (det[0] >= 0)
	  triangle = triangle->Neighbor[1];
	else if (det[1] >= 0)
	  triangle = triangle->Neighbor[2];
	else if (det[2] >= 0)
	  triangle = triangle->Neighbor[0];
	else
	  triangle = *it++;
  }
}

void DelaunayTriangulation::RemoveExtraTriangles() {
  std::vector<Triangle*>::iterator it;
  for (it = m_mesh->begin(); it != m_mesh->end();) {
	Triangle* triangle = *it;
	bool isExtraTriangle = false;
	for (int i = 0; i < 3; i++) {
	  if (triangle->Vertex[i]->IsAuxiliaryDot) {
		isExtraTriangle = true;
		break;
	  }
	}

	if (isExtraTriangle) {
	  delete *it;
	  it = m_mesh->erase(it);
	} else {
	  it++;
	}
  }
}

void DelaunayTriangulation::SplitTriangle(Triangle* triangle, Vector3D* dot) {
  Triangle* newTriangle1 = new Triangle(dot, triangle->Vertex[1], triangle->Vertex[2]);
  Triangle* newTriangle2 = new Triangle(dot, triangle->Vertex[2], triangle->Vertex[0]);

  triangle->Vertex[2] = triangle->Vertex[1];
  triangle->Vertex[1] = triangle->Vertex[0];
  triangle->Vertex[0] = dot;

  newTriangle1->AssignNeighbors(triangle, triangle->Neighbor[1], newTriangle2);
  newTriangle2->AssignNeighbors(newTriangle1, triangle->Neighbor[2], triangle);
  triangle->AssignNeighbors(newTriangle2, triangle->Neighbor[0], newTriangle1);

  FixNeighborhood(newTriangle1->Neighbor[1], triangle, newTriangle1);
  FixNeighborhood(newTriangle2->Neighbor[1], triangle, newTriangle2);

  m_mesh->push_back(newTriangle1);
  m_mesh->push_back(newTriangle2);

  // optimize triangles according to delaunay triangulation definition
  DoLocalOptimization(triangle, triangle->Neighbor[1]);
  DoLocalOptimization(newTriangle1, newTriangle1->Neighbor[1]);
  DoLocalOptimization(newTriangle2, newTriangle2->Neighbor[1]);
}

void DelaunayTriangulation::FixNeighborhood(Triangle* target, Triangle* oldNeighbor, Triangle* newNeighbor) {
  for (int i = 0; i < 3; i++) {
	if (target->Neighbor[i] == oldNeighbor) {
	  target->Neighbor[i] = newNeighbor;
	  break;
	}
  }
}

void DelaunayTriangulation::DoLocalOptimization(Triangle* t0, Triangle* t1) {
  _Statistics[1]++;

  for (int i = 0; i < 3; i++) {
	if (t1->Vertex[i] == t0->Vertex[0] || t1->Vertex[i] == t0->Vertex[1] || t1->Vertex[i] == t0->Vertex[2]) {
	  continue;
	}

	double matrix[] = {
	  t1->Vertex[i]->X - t0->Vertex[0]->X,
	  t1->Vertex[i]->Y - t0->Vertex[0]->Y,
	  t1->Vertex[i]->Z - t0->Vertex[0]->Z,

	  t1->Vertex[i]->X - t0->Vertex[1]->X,
	  t1->Vertex[i]->Y - t0->Vertex[1]->Y,
	  t1->Vertex[i]->Z - t0->Vertex[1]->Z,

	  t1->Vertex[i]->X - t0->Vertex[2]->X,
	  t1->Vertex[i]->Y - t0->Vertex[2]->Y,
	  t1->Vertex[i]->Z - t0->Vertex[2]->Z
	};

	if (GetDeterminant(matrix) <= 0) {
	  // terminate after optimized
	  break;
	}

	if (TrySwapDiagonal(t0, t1)) {
	  return;
	}
  }
}

bool DelaunayTriangulation::TrySwapDiagonal(Triangle* t0, Triangle* t1) {
  for (int j = 0; j < 3; j++) {
	for (int k = 0; k < 3; k++) {
	  if (t0->Vertex[j] != t1->Vertex[0] && t0->Vertex[j] != t1->Vertex[1] && t0->Vertex[j] != t1->Vertex[2] && t1->Vertex[k] != t0->Vertex[0] && t1->Vertex[k] != t0->Vertex[1] && t1->Vertex[k] != t0->Vertex[2]) {
		t0->Vertex[(j + 2) % 3] = t1->Vertex[k];
		t1->Vertex[(k + 2) % 3] = t0->Vertex[j];

		t0->Neighbor[(j + 1) % 3] = t1->Neighbor[(k + 2) % 3];
		t1->Neighbor[(k + 1) % 3] = t0->Neighbor[(j + 2) % 3];
		t0->Neighbor[(j + 2) % 3] = t1;
		t1->Neighbor[(k + 2) % 3] = t0;

		FixNeighborhood(t0->Neighbor[(j + 1) % 3], t1, t0);
		FixNeighborhood(t1->Neighbor[(k + 1) % 3], t0, t1);

		DoLocalOptimization(t0, t0->Neighbor[j]);
		DoLocalOptimization(t0, t0->Neighbor[(j + 1) % 3]);
		DoLocalOptimization(t1, t1->Neighbor[k]);
		DoLocalOptimization(t1, t1->Neighbor[(k + 1) % 3]);

		return true;
	  }
	}
  }

  return false;
}

bool DelaunayTriangulation::IsMinimumValueInArray(double arr[], int length, int index) {
  for (int i = 0; i < length; i++) {
	if (arr[i] < arr[index]) {
	  return false;
	}
  }

  return true;
}

double DelaunayTriangulation::GetDistance(Vector3D* v0, Vector3D* v1) {
  return sqrt(pow((v0->X - v1->X), 2) + pow((v0->Y - v1->Y), 2) + pow((v0->Z - v1->Z), 2));
}

double DelaunayTriangulation::GetDeterminant(Vector3D* v0, Vector3D* v1, Vector3D* v2) {
  double matrix[] = {
	v0->X, v0->Y, v0->Z, v1->X, v1->Y, v1->Z, v2->X, v2->Y, v2->Z
  };

  return GetDeterminant(matrix);
}

double DelaunayTriangulation::GetDeterminant(double matrix[]) {
  // inversed for left handed coordinate system
  double determinant = matrix[2] * matrix[4] * matrix[6]
					   + matrix[0] * matrix[5] * matrix[7]
					   + matrix[1] * matrix[3] * matrix[8]
					   - matrix[0] * matrix[4] * matrix[8]
					   - matrix[1] * matrix[5] * matrix[6]
					   - matrix[2] * matrix[3] * matrix[7];

  // adjust result based on float number accuracy, otherwise causing deadloop
  return abs(determinant) <= std::numeric_limits<double>::epsilon() ? 0 : determinant;
}

std::string DelaunayTriangulation::GetStatistics() {
  // display thousands separator
  std::regex regex("\\d{1,3}(?=(\\d{3})+$)");

  return "\nTriangle count: "
		 + regex_replace(std::to_string(m_mesh->size()), regex, "$&,")
		 + "\nTriangle search operations: "
		 + regex_replace(std::to_string(_Statistics[0]), regex, "$&,")
		 + "\nLocal optimizations: "
		 + regex_replace(std::to_string(_Statistics[1]), regex, "$&,")
		 + "\nTriangulation cost: "
		 + std::to_string(_Statistics[3] - _Statistics[2])
		 + "ms\n";
}
