#pragma once

#include <iostream>
#include <vector>
#include <cstring>
#include <glm/glm.hpp>
#include "stdio.h"

#include "quadtree.h"



float scalar_product(glm::vec3 X, glm::vec3 Y);
float euclidean_norm(glm::vec3 X);
float euclidean_distance(glm::vec3 X, glm::vec3 Y);


// class to manage 3D points and normals
class Data{

protected:

	glm::vec3 point;
	glm::vec3 normal;

public:

	Data(glm::vec3 X, glm::vec3 N);
	Data(){}
    Data(const Data& D);
    
    inline glm::vec3 p() const { return point; }
    inline glm::vec3 n() const { return normal; }
    
    bool operator==(const Data &D) const { return point==D.point && normal==D.normal; }
    bool operator!=(const Data &D) const { return point!=D.point || normal!=D.normal; }
   
    float dist(const Data &D) const { return euclidean_distance(point, D.point); }
};


// function to load .obj file into a vector of Data
bool loadOBJ(
    const char * path,
    std::vector <Data> & point_cloud
);


// class for unit cubes to map the 3D space
// first define cube vertices wrt origine vertice
const std::vector<glm::vec3> cube_vertices = {
	glm::vec3(0.0, 0.0, 0.0), 
	glm::vec3(1.0, 0.0, 0.0),
	glm::vec3(1.0, 1.0, 0.0),
	glm::vec3(0.0, 1.0, 0.0),
	glm::vec3(0.0, 0.0, 1.0),
	glm::vec3(1.0, 0.0, 1.0),
	glm::vec3(1.0, 1.0, 1.0),
	glm::vec3(0.0, 1.0, 1.0)
};


class Cube{

public:

	glm::vec3 origin;
	float scale;
    std::vector<float> scalar_field;  // in case same cubes are used for space delimitation and marching cubes

	Cube() {};
    Cube(glm::vec3 X, float s);
    Cube(const Cube& C);
    Cube(const std::vector<Data>& v);

    int subcube(glm::vec3 X) const;
    void next_cube(int X);
    void previous_cube(int X);
    void add_field(int vertex, float value);
    // Return true iff this cube intersects with sphere defined by point P and radius r
    bool intersect_sphere(const glm::vec3& P, float r) const;
};


// function to load a vector of Data into an OctTree
OctNode<Data>* makeTree(const std::vector<Data>& V, Cube init_cube);


// recursive OctTree search
void find_neighbors(OctTree<Data>* O, const Data& D, float& r, std::vector<Data>& V, Cube C, const bool& best, int& counter);