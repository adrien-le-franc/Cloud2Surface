#include "rimls.h"

float phi(float t, float h){
	return pow(1.0 - t / pow(h, 2), 4);
};

float dphi(float t, float h){
	return -4.0 * pow(1.0 - t / pow(h, 2), 3) / pow(h, 2);
};

float rimls_step(const glm::vec3& point, const std::vector<Data>& neighbors, float h, float sigma_r, float sigma_n, int max_iter, int& n){

	float f;
	glm::vec3 grad_f;

	bool print = false;

	for(int k=0; k<max_iter; k++){

		float sum_w = 0.0;
		float sum_f = 0.0;
		float alpha;


		glm::vec3 sum_n(0.0, 0.0, 0.0);
		glm::vec3 sum_gw(0.0, 0.0, 0.0);
		glm::vec3 sum_gf(0.0, 0.0, 0.0);

		for(std::vector<Data>::const_iterator it=neighbors.begin(); it!=neighbors.end(); it++){

			glm::vec3 dx = point - (*it).p();
			float fx = scalar_product(dx, (*it).n());
			alpha = 1.0;
			if(k>0)
				alpha = std::exp(-pow((fx-f)/sigma_r, 2)) * std::exp(-pow(euclidean_norm((*it).n()-grad_f)/sigma_n, 2));

			float w = alpha * phi(pow(euclidean_norm(point-(*it).p()), 2), h);
			glm::vec3 grad_w = alpha * 2 * dx * dphi(pow(euclidean_norm(point-(*it).p()), 2), h);

			sum_w += w;
			sum_gw += grad_w;
			sum_f += w * fx;
			sum_gf += grad_w * fx;
			sum_n += w * (*it).n();

			//std::cout<< k << " " << alpha << std::endl;
			
		
		}

		if(print){

			// for(std::vector<Data>::const_iterator it=neighbors.begin(); it!=neighbors.end(); it++){
			// 	std::cout << (*it).p().x << " " << (*it).p().y << " " << (*it).p().z << std::endl;
			// }
			//std::cout << "neighbors " << neighbors.size() << " alpha " << alpha << std::endl;
		}

		f = sum_f / sum_w; 
		grad_f = (sum_gf - f*sum_gw + sum_n) / sum_w;


		if(std::isnan(f)){
			if(print==false){
				n++;
				std::cout << "n " << n << " k " << k << " sum_w " << sum_w << std::endl;

				if(k==0){
					std::cout << point.x << " " << point.y << " " << point.z << std::endl;
					for(std::vector<Data>::const_iterator it=neighbors.begin(); it!=neighbors.end(); it++){
						float what = phi(pow(euclidean_norm(point-(*it).p()), 2), h);
						std::cout << (*it).p().x << " " << (*it).p().y << " " << (*it).p().z << " " << "phi: " << what << std::endl;
					}
					printf("\n");
				}
			}
		 	print = true;
		// 	printf("NAN\n");
		//    	//std::cout << "NAN alpha: " << alpha << " k " << k << std::endl; //" dist " << euclidean_distance(neighbors[0].p(), point) << std::endl; 
		    }

		    
	
		// printf("NAN !!");
		// std::cout<< euclidean_distance(neighbors[0].p(), point) << std::endl;
		// printf("\n");
		// }

	
	}

	
	// for(std::vector<Data>::const_iterator it=neighbors.begin(); it!=neighbors.end(); it++){
	// 			std::cout << (*it).p().x << " " << (*it).p().y << " " << (*it).p().z << std::endl;
	// 		}
	//printf("\n");
	//printf("\n");
	return f;
}

Cube rimls_regular(const Data& D, OctTree<Data>* OT, Cube init_cube, float radius, float grid_step, float sigma_r, float sigma_n, int max_neighbors, 
	int max_iter, int& n){
	
	glm::vec3 origin = D.p() - grid_step*float(0.5)*glm::vec3(1.0, 1.0, 1.0);  // init cube centered on data point
	Cube cube(origin, grid_step);

	for(int k=0; k<8; k++){  // visiting cube's vertices and compute scalar field

		Data point(cube.origin + grid_step*cube_vertices[k], glm::vec3(0.0, 0.0, 0.0));
		int counter = 0;
		std::vector<Data> neighbors;

		find_neighbors(OT, point, radius, neighbors, init_cube, true, counter);



		if(neighbors.size() < 2){
			printf("radius too small\n");
			float max_radius = sqrt(3.0)*init_cube.scale;
			find_neighbors(OT, point, max_radius, neighbors, init_cube, true, counter);; // must be at least 2 neighbors to avoid underflow, 
			                                                                                  // could also skip point ?
		}

		else{ printf("radius ok\n");}

	
		float h = 0.0;
		counter = 0;

		std::vector<Data> nearest_neighbors;


		// for(std::vector<Data>::const_iterator it=neighbors.end(); it!=neighbors.begin(); it--){
		// 	h += point.dist(*it);
		// 	counter++;
		// 	nearest_neighbors.push_back(*it);

		// 	if(counter >= max_neighbors)
		// 		break;
		// }




		float hx = 0.0;
		float counterx = 0;
		int l = neighbors.size();

		std::vector<Data> testx;

		for(int i=0; i<l; i++){

			Data pt(neighbors[l-1-i]);
			hx += point.dist(pt);
			testx.push_back(pt);
			counterx ++;

			if(counterx >= max_neighbors)
				break;


		}
		//////////////////////////////////////
			// float maxi = 0.0;
   //      	float mini = 1.0;

		 //    for(std::vector<Data>::const_iterator it=neighbors.begin(); it !=neighbors.end(); it++){

   //          glm::vec3 pp = (*it).p();

   //          if(pp.x > maxi)
   //              maxi = pp.x;
   //          if(pp.y > maxi)
   //              maxi = pp.y;
   //          if(pp.z > maxi)
   //              maxi = pp.z;

   //          if(pp.x < mini)
   //              mini = pp.x;
   //          if(pp.y < mini)
   //              mini = pp.y;
   //          if(pp.z < mini)
   //              mini = pp.z;
   //      }

        //std::cout << "n " << "max " << maxi << " min " << mini << " h " << h << " hx " << hx << " counter " << counter << " counterx " << counterx << std::endl;
        ////////////////////////////////////////////////////

		h = h / float(counter) * 2.0;

		// std::cout << counter << std::endl;
		// printf("\n");

		cube.add_field(k, rimls_step(point.p(), testx, hx, sigma_r, sigma_n, max_iter, n));
	}

	return cube;
}

void rimls(const std::vector<Data>& V, std::vector<Cube>& grid, float radius, float grid_step, float sigma_r, float sigma_n, int max_neighbors, 
	int max_iter){

	Cube init_cube(V);
	OctTree<Data>* OT = makeTree(V, init_cube);

	int n = 0;

	for(std::vector<Data>::const_iterator it=V.begin(); it!=V.end(); it++){
		grid.push_back(rimls_regular(*it, OT, init_cube, radius, grid_step, sigma_r, sigma_n, max_neighbors, max_iter, n));
	}

	std::cout << "nb nan :" << n << " / " << V.size()*8 << std::endl;

	delete OT;
};