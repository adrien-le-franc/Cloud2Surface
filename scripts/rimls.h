#pragma once

#include "data.h"



float phi(float t, float h);
float dphi(float t, float h);

float rimls_step(const glm::vec3& point, const std::vector<Data>& neighbors, float h, float sigma_r, float sigma_n, int max_iter);

Cube rimls_regular(const Data& D, OctTree<Data>* OT, Cube init_cube, float radius, float grid_step, float sigma_r, float sigma_n, int max_neighbors, 
	int max_iter);

void rimls(const std::vector<Data>& V, std::vector<Cube>& grid, float radius, float grid_step, float sigma_r, float sigma_n, int max_neighbors, 
	int max_iter);