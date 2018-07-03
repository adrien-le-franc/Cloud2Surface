#include "data.h"


bool loadOBJ(
    const char * path,
    std::vector < glm::vec3 > & out_vertices,
    std::vector < glm::vec3 > & out_normals
){

	FILE * file = fopen(path, "r");
	if( file == NULL ){
    	printf("Impossible to open the file !\n");
    	return false;
	}

	while(1){

    char lineHeader[128];

    // read the first word of the line
    int res = fscanf(file, "%s", lineHeader);
    if (res == EOF)
        break; 

    // track vertex
    if ( strcmp( lineHeader, "v" ) == 0 ){

    	glm::vec3 vertex;
    	fscanf(file, "%f %f %f\n", &vertex.x, &vertex.y, &vertex.z );
    	out_vertices.push_back(vertex);
    }

	// track normals
    else if ( strcmp( lineHeader, "vn" ) == 0 ){
    	glm::vec3 normal;
    	fscanf(file, "%f %f %f\n", &normal.x, &normal.y, &normal.z );
    	out_normals.push_back(normal);
    }
	
	
	}
	
};


float scalar_product(glm::vec3 X, glm::vec3 Y){
    return X.x*Y.x + X.y*Y.y + X.z*Y.z;
}

float euclidean_norm(glm::vec3 X){
    return sqrt(scalar_product(X, X));
};

float euclidean_distance(glm::vec3 X, glm::vec3 Y){
    return euclidean_norm(X-Y);
}


Data::Data(glm::vec3 X, glm::vec3 N){
    point = X;
    normal = N;
}

Data::Data(const Data& D){
    point = D.point;
    normal = D.normal;
}

Cube::Cube(glm::vec3 X, float s){
    origin = X;
    scale = s;

    std::vector<float> temp (8, 0.0);
    scalar_field = temp;
}

Cube::Cube(const std::vector<Data>& v){
    glm::vec2 bound_x(v[0].p()[0], v[0].p()[0]);
    glm::vec2 bound_y(v[0].p()[1], v[0].p()[1]);
    glm::vec2 bound_z(v[0].p()[2], v[0].p()[2]);

    for(std::vector<Data>::const_iterator it=v.begin(); it!=v.end(); it++){
        float x = (*it).p()[0];
        float y = (*it).p()[1];
        float z = (*it).p()[2];

        if(x < bound_x[0])
            bound_x[0] = x;
        if(x > bound_x[1])
            bound_x[1] = x;

        if(y < bound_y[0])
            bound_y[0] = y;
        if(y > bound_y[1])
            bound_y[1] = y;

        if(z < bound_z[0])
            bound_z[0] = z;
        if(z > bound_z[1])
            bound_z[1] = z;
    }

    float len_x = bound_x.y - bound_x.x;
    float len_y = bound_y.y - bound_y.x;
    float len_z = bound_z.y - bound_z.x;

    scale = glm::max(len_z, glm::max(len_y, len_x));

    float center_x = (bound_x.x + bound_x.y) / 2.0;
    float center_y = (bound_y.x + bound_y.y) / 2.0;
    float center_z = (bound_z.x + bound_z.y) / 2.0;

    float origin_x = center_x - (scale / 2.0);
    float origin_y = center_y - (scale / 2.0);
    float origin_z = center_z - (scale / 2.0);

    origin = glm::vec3(origin_x, origin_y, origin_z);

    std::vector<float> temp (8, 0.0);
    scalar_field = temp;
}

Cube::Cube(const Cube& C){
    origin = C.origin;
    scale = C.scale;
    scalar_field = C.scalar_field;
}

int Cube::subcube(glm::vec3 X) const{
    float lx = X.x - origin.x;
    float ly = X.y - origin.y;
    float lz = X.z - origin.z;

    if(lx > scale || ly > scale || lz > scale || lx < 0.0 || ly < 0.0 || lz < 0.0){
        printf("ERROR: X is not in cube\n");
        return false;
    }

    float middle = scale / 2.0;

    if(lx < middle && ly < middle && lz < middle)
        return 0;

    if(lx >= middle && ly < middle && lz < middle)
        return 1;

    if(lx >= middle && ly >= middle && lz < middle)
        return 2;

    if(lx < middle && ly >= middle && lz < middle)
        return 3;

    if(lx < middle && ly < middle && lz >= middle)
        return 4;

    if(lx >= middle && ly < middle && lz >= middle)
        return 5;

    if(lx >= middle && ly >= middle && lz >= middle)
        return 6;

    if(lx < middle && ly >= middle && lz >= middle)
        return 7;

    else{
        printf("some misterious additional case\n");
        return false;
    }

}

void Cube::next_cube(int X){
    glm::vec3 direction = cube_vertices[X];

    scale = scale / 2.0;
    origin += direction*scale;
}

void Cube::previous_cube(int X){
    glm::vec3 direction = cube_vertices[X];

    origin -= direction*scale;
    scale = scale * 2.0;

}

void Cube::add_field(int vertex, float value){
    scalar_field[vertex] = value;
}

bool Cube::intersect_sphere(const glm::vec3& P, float r) const{

    glm::vec3 nx(P.x+r, P.y, P.z);
    glm::vec3 sx(P.x-r, P.y, P.z);
    glm::vec3 ny(P.x, P.y+r, P.z);
    glm::vec3 sy(P.x, P.y-r, P.z);
    glm::vec3 nz(P.x, P.y, P.z+r);
    glm::vec3 sz(P.x, P.y, P.z-r); // north and south of sphere along axis

    bool intersect_on_xy = false;
    bool intersect_on_xz = false;
    bool intersect_on_yz = false;

    if((origin.x <= nx.x && origin.x+scale >= sx.x) && (origin.y <= ny.y && origin.y+scale >= sy.y))
        intersect_on_xy = true;

    if((origin.y <= ny.y && origin.y+scale >= sy.y) && (origin.z <= nz.z && origin.z+scale >= sz.z))
        intersect_on_yz = true;

    if((origin.x <= nx.x && origin.x+scale >= sx.x) && (origin.z <= nz.z && origin.z+scale >= sz.z))
        intersect_on_xz = true;

    return intersect_on_xz * intersect_on_yz * intersect_on_xy;
}


OctNode<Data>* makeTree(const std::vector<Data>& V, Cube init_cube){

    OctNode<Data>* OT = new OctNode<Data>(); // init: empty OctNode
    OctTree<Data>* rot = OT; // pointing at running Octree

    Cube C;
    Data* transitory;

    for(std::vector<Data>::const_iterator it=V.begin(); it!=V.end(); it++){
        C.origin = init_cube.origin; C.scale = init_cube.scale;
        int X = C.subcube((*it).p());

        while(true){                   // moving down the nodes
            if(rot->son(X)==nullptr)
                break;
            if(rot->son(X)->isLeaf())   // can be tested once son(X) is not nullptr
                break;
            rot = rot->son(X);
            C.next_cube(X);
            X = C.subcube((*it).p());
        }

        if(rot->son(X)==nullptr){
            rot->son(X) = new OctLeaf<Data>(*it);
            rot = OT;
            continue;
        } 

        if((rot->son(X))->isLeaf()){
            if(*it==(rot->son(X))->value()) // ignore point if already in tree
                continue;
            transitory = &(rot->son(X))->value(); // stock value in the Leaf before changing it into a Node
            rot->son(X) = new OctNode<Data>(); // replace Leaf by Node
            rot = rot->son(X); // move down
            C.next_cube(X);
            X = C.subcube((*it).p());
            while(true){
                if(X!=C.subcube((*transitory).p()))
                    break;
                rot->son(X) = new OctNode<Data>();
                rot = rot->son(X);
                C.next_cube(X);
                X = C.subcube((*it).p());
            }
            rot->son(X) = new OctLeaf<Data>(*it);
            rot->son(C.subcube((*transitory).p())) = new OctLeaf<Data>(*transitory);
            rot = OT;
        }
    }
    return OT;
}


void find_neighbors(OctTree<Data>* O, const Data& D, float& r, std::vector<Data>& V, Cube C, const bool& best, int& counter){

    for(int i=0; i<8; i++){
        if(O->son(i)==nullptr)
            continue;
        if(O->son(i)->isLeaf()){
            if(D.dist(O->son(i)->value())<=r){
                if(O->son(i)->value()==D){      // exclude self from search
                    continue;
                }
                if(best){                            // if searching nearest Data
                    r = D.dist(O->son(i)->value());
                    V.push_back(O->son(i)->value());
                }
                else
                    V.push_back(O->son(i)->value());   // else store results
            }
        }
        else{
            counter++;        // counts number of nodes encountered
            C.next_cube(i);
            if(C.intersect_sphere(D.p(), r))
                find_neighbors(O->son(i), D, r, V, C, best, counter);
            C.previous_cube(i);   // previous cube corresponding to the node whose children are being inspected in the loop
        }
    }
}