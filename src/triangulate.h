#pragma once

#include <vector>
#include "structures.h"

#define INIT_VERTICES_COUNT 6 /* count of vertices in the initial hull */
#define INIT_FACES_COUNT 8 /* count of faces in the initial hull */
#define VECTOR_LENGTH 1 /* radius of unit sphere the dots projected into */


class DelaunayTriangulation {
 private:
  Vector3D* _AuxiliaryDots[INIT_VERTICES_COUNT];
  std::vector<Vector3D*>* m_projectedDots;
  std::vector<Triangle*>* m_mesh;

  // 0: triangle search operations
  // 1: local optimizations
  // 2: start time; 3: end time;
  long _Statistics[4];

  void BuildInitialHull(std::vector<Vector3D*>* dots);
  void InsertDot(Vector3D* dot);
  void RemoveExtraTriangles();
  void SplitTriangle(Triangle* triangle, Vector3D* dot);
  void FixNeighborhood(Triangle* target, Triangle* oldNeighbor, Triangle* newNeighbor);
  void DoLocalOptimization(Triangle* t0, Triangle* t1);
  bool TrySwapDiagonal(Triangle* t0, Triangle* t1);
  bool IsMinimumValueInArray(double arr[], int length, int index);
  double GetDistance(Vector3D* v0, Vector3D* v1);
  double GetDeterminant(Vector3D* v0, Vector3D* v1, Vector3D* v2);
  double GetDeterminant(double matrix[]);

 public:
  DelaunayTriangulation();
  ~DelaunayTriangulation();

  std::vector<TriangleList*> GetTriangulationResult(std::vector<Vector3D*>& dots);
  std::string GetStatistics();
};
