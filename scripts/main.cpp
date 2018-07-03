
#include "data.h"
# include "rimls.h"


int main() 
{	

	// std::vector< glm::vec3 > vertices;
	// std::vector< glm::vec3 > normals; 

	// bool res = loadOBJ("/home/adrien/Projets/3D/X/data/fandisk.obj", vertices, normals);

 //    std::cout << vertices[0].x << " " << vertices[0].y << " " << vertices[0].z  << std::endl;

 //    Data test(vertices[0], normals[0]);

 //    glm::vec3 X = test.p();

 //    std::cout << X.x << " " << X.y << " " << X.z  << std::endl;
	glm::vec3 n(1.0, 1.0, 1.0);

	std::vector<Data> v = {
	Data(glm::vec3(2.0, 2.0, 2.0), n), 
	Data(glm::vec3(6.0, 2.0, 2.0), n),
	Data(glm::vec3(6.0, 6.0, 2.0), n),
	Data(glm::vec3(2.0, 6.0, 2.0), n),
	Data(glm::vec3(2.0, 2.0, 6.0), n),
	Data(glm::vec3(6.0, 2.0, 6.0), n),
	Data(glm::vec3(6.0, 6.0, 6.0), n),
	Data(glm::vec3(2.0, 6.0, 6.0), n),
	Data(glm::vec3(2.2, 2.0, 2.0), n)
	};

	Cube C(v);

	// printf("building tree\n");

	// OctTree<Data>* OT = makeTree(v, C);

	// printf("done\n");

	// Data target(glm::vec3(2.1, 2.0, 2.0), n);

	// std::vector<Data> results;
	// int counter = 0;
	// float r = 1.0;

	// printf("searching\n");

	// find_neighbors(OT, target, r, results, C, false, counter);

	// printf("done\n");

	//  for(std::vector<Data>::const_iterator it=results.begin(); it!=results.end(); it++){

	//  	std::cout << (*it).p()[0] << " " << (*it).p()[1] << " " << (*it).p()[2] << std::endl;
	//  }

	std::vector<Cube> grid;
	float radius = 2.0;
	float grid_step = 1.0;
	float sigma_r = 0.5;
	float sigma_n = 1;
	int max_iter = 3;

	printf("rimls\n");

	rimls(v, grid, radius, grid_step, sigma_r, sigma_n, max_iter);

	printf("done\n");

	std::cout << grid.size() << std::endl;

	//delete OT;

    return 0;
}