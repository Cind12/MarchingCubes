#include <hpx/hpx_fwd.hpp>
#include <hpx/runtime/components/server/managed_component_base.hpp>
#include <hpx/runtime/components/server/locking_hook.hpp>
#include <hpx/runtime/actions/component_action.hpp>
#include "../lib/glm/glm.hpp"
#include "table.h"

struct serialVec {
   float x;
   float y;
   float z;

private:
    friend class boost::serialization::access;

    template <typename Archive>
    void serialize(Archive& ar, const unsigned int version)
    {
        ar & x & y & z;
    }
};

bool operator<(const serialVec a, const serialVec b){
	if (b.z <= a.z) return false;
	if (b.y <= a.y) return false;
	if (b.x <= a.x) return false;
	return true;
}

struct block_boilerplate {
    hpx::id_type GID;
    serialVec pos;

private:
    friend class boost::serialization::access;

    template <typename Archive>
    void serialize(Archive& ar, const unsigned int version)
    {
        ar & GID & pos;
    }
};

typedef glm::vec3 vec3;
typedef glm::vec2 vec2;

typedef vector<float> data;
typedef vector<float> cube;
typedef int ID;
typedef vec2 edge;
typedef tuple<serialVec, serialVec>  vertexPos;
typedef std::map<vertexPos, ID> vertexMap;
typedef vector<ID> face;

struct Vertex {
	ID   global_ID;
	vec3 coord;
	float  normalWeight;
	vec3 normal;
};

struct BlockInfo {
	vector<vertexPos> vertexPosList;
	vector<Vertex>	  vertexList;
	vector<face>	  faceList;
	int		  activeVertexCount;
};

int IDtoArrayPos(ID id, int count){
	if (id > 0) return id-1;
	if (id < 0) return count-1 - id;	
}

vec3 computeFaceNormal(Vertex one, Vertex two, Vertex three){
	vec3 U = two.coord - one.coord;
	vec3 V = three.coord - one.coord;
	vec3 result;
	result.x = (U.y*V.z) - (U.z*V.y);
	result.y = (U.z*V.x) - (U.x*V.z);
	result.z = (U.x*V.y) - (U.y*V.x);
	return result;
}

void adjustNormals(Vertex one, Vertex two, Vertex three){
	vec3 faceNormal = computeFaceNormal(one, two, three);

	vec3 a = (one.normalWeight*one.normal) + faceNormal;
	one.normalWeight++;
	one.normal = a* (1/one.normalWeight);

	a = (two.normalWeight*two.normal) + faceNormal;
	two.normalWeight++;
	two.normal = a* (1/two.normalWeight);

	a = (three.normalWeight*three.normal) + faceNormal;
	three.normalWeight++;
	three.normal = a* (1/three.normalWeight);
} 

#include <limits>
#include <cmath>

using namespace std;

double mandelbulb( const double z0[3], bool output = false )
{
    const double power = 8.0f;
    const int    iter  = 8;
   
    double z[3] = { z0[0], z0[1], z0[2] };    
    
    double r  = sqrt( z[0]*z[0]+z[1]*z[1]+z[2]*z[2] );
    
    if( r < numeric_limits<double>::epsilon() )
        return 0.0;
    
    double dr = 1.0;
    
    for( int i=0; i<iter && r<2.0; ++i ) 
    {     
        double zo = power * asin( z[2]/r );
        double zi = power * atan( z[1]/z[0] );
        double zr = pow( r, power-1.0f );

        dr = zr*dr*power + 1.0f;
        zr *= r;

        z[0] = z0[0] + zr * cos(zo)*cos(zi);
        z[1] = z0[1] + zr * cos(zo)*sin(zi);
        z[2] = z0[2] + zr * sin(zo);
                
        r = sqrt( z[0]*z[0]+z[1]*z[1]+z[2]*z[2] );
    }

    double v = 0.5*log(r)*r/dr;
    
    return v;
}

namespace server
{
  class block
    : public hpx::components::managed_component_base<block>
  {
  public:

    block(){
    }

    block(serialVec pos, boost::uint64_t size){
	position_ = vec3(pos.x, pos.y, pos.z);
	m_size = size;
	printf("Block created\n");
    }

    void initialize(){
	printf("loading data\n");	
	double bmin[3] = { -1.1, -1.1, -1.1 };
	double bmax[3] = {  1.1,  1.1,  1.1 };

	int res[3] = { m_size, m_size, m_size };

	// generate values on regular grid
	vector<float> values( res[0]*res[1]*res[2] );

	for( int k=0; k<res[2]; ++k )
	{
	    cerr << k << endl;
		
	    for( int j=0; j<res[1]; ++j )
	    {
	        for( int i=0; i<res[0]; ++i )
	        {
	            double z[3] = {
	                bmin[0] + (i+0.5) * (bmax[0] - bmin[0])/(res[0]-1),
 	                bmin[1] + (j+0.5) * (bmax[1] - bmin[1])/(res[1]-1),
	                bmin[2] + (k+0.5) * (bmax[2] - bmin[2])/(res[2]-1),
		    };
	    	values[i + res[0]*(j+k*res[1])] = mandelbulb( z );
	        }
	    }
	}
    	m_dat = values; 
	printf("data loaded\n");
    }

    block_boilerplate getBoilerplate(){
	block_boilerplate result;
	result.GID = this->get_gid();
	result.pos.x = position_.x;
	result.pos.y = position_.y;
	result.pos.z = position_.z;
	return result;	
    }

    //findet oder registriert ID für diese Position
    //IDs werden bei jeder registrierung hochgezählt
    //IDs passiver Positionen werden unabhängig negativ gezählt
    //arrayindex entspricht positiver ID - 1 oder getCount - 1 - 
    //negativer ID
    ID  mapGet(vertexPos pos){ 
	if (m_Map.count(pos)){
	  return m_Map.find(pos)->second;
	} else {
	  m_Info.activeVertexCount++;
	  m_Map.insert(std::pair<vertexPos, ID>(pos, m_Info.activeVertexCount));
	  return m_Info.activeVertexCount;
	}
    } 

    vector<vertexPos> mapMakeList(){ 
	vector<vertexPos> result;
	for (vertexMap::iterator it = m_Map.begin(); it != m_Map.end(); ++it){
		result.push_back(it->first);
	}
	printf("Making MapList with %u Vertices \n", result.size());
	return result;
    }  

    int getCount() { 
	return m_activeVertexCount; 
    }

    vec3 interpolate(vertexPos pos, float isovalue) { 
	vec3 first(std::get<0>(pos).x, std::get<0>(pos).y, std::get<0>(pos).z);
	int datapoint = first.z*(m_size*m_size) + first.y*(m_size) + first.x;
	float firstValue = m_dat.at(datapoint);
	
	vec3 second(std::get<1>(pos).x, std::get<1>(pos).y, std::get<1>(pos).z);
	datapoint = second.z*(m_size*m_size) + second.y*(m_size) + second.x;
	float secondValue = m_dat.at(datapoint);


	if (firstValue > secondValue){
		float a = firstValue - secondValue;
		float b = isovalue - secondValue;
		float ratioA = b/a;
		float ratioB = (a-b)/a;
		return second*ratioA + first*ratioB;
	} else {
		float a = secondValue - firstValue;
		float b = isovalue - firstValue;
		float ratioA = b/a;
		float ratioB = (a-b)/a;
		return first*ratioA + second*ratioB;
	}
    }

    int cubeCount(){
        int temp = m_size-1;
	return temp*temp*temp;
    }

    cube findCube(unsigned i){
	// return marching cube with number i
	vector<float> result(8);
	int temp = m_size-1;

	//cube space coordinates
	int z = (int)i/(temp*temp);
	int y = (int)(i % (temp*temp))/temp;
	int x = (int)(i % temp);

	int pz = z;
	int py = y;
	int px = x;

	int datapoint = pz*(m_size*m_size) + py*(m_size) + px;

	result[4] = m_dat.at(datapoint);
	px++;
	datapoint = pz*(m_size*m_size) + py*(m_size) + px;
	result[5] = m_dat.at(datapoint);
	py++;	
	datapoint = pz*(m_size*m_size) + py*(m_size) + px;
	result[6] = m_dat.at(datapoint);
	px--;
	datapoint = pz*(m_size*m_size) + py*(m_size) + px;
	result[7] = m_dat.at(datapoint);
	pz++;	
	datapoint = pz*(m_size*m_size) + py*(m_size) + px;
	result[3] = m_dat.at(datapoint);
	py--;
	datapoint = pz*(m_size*m_size) + py*(m_size) + px;
	result[0] = m_dat.at(datapoint);
	px++;
	datapoint = pz*(m_size*m_size) + py*(m_size) + px;
	result[1] = m_dat.at(datapoint);
	py++;
	datapoint = pz*(m_size*m_size) + py*(m_size) + px;
	result[2] = m_dat.at(datapoint);

	return result;
    } 

    vertexPos findCoords(int i, edge myEdge){
	int temp = m_size-1;

	//cube space coordinates
	int z = (int)i/(temp*temp);
	int y = (int)(i % (temp*temp))/temp;
	int x = (int)(i % temp);

	int px = x;
	int py = y;
	int pz = z;
	if (myEdge.x == 0){
		pz++;
	}
	if (myEdge.x == 1){
		pz++;
		px++;
	}
	if (myEdge.x == 2){
		px++;
		py++;
		pz++;
	}
	if (myEdge.x == 3){
		pz++;
		py++;
	}
	if (myEdge.x == 5){
		px++;
	}
	if (myEdge.x == 6){
		px++;
		py++;
	}
	if (myEdge.x == 7){
		py++;
	}
	serialVec first;
	first.x = px;
	first.y = py;
	first.z = pz;
	px = x;
	py = y;
	pz = z;
	if (myEdge.y == 0){
		pz++;
	}
	if (myEdge.y == 1){
		pz++;
		px++;
	}
	if (myEdge.y == 2){
		px++;
		py++;
		pz++;
	}
	if (myEdge.y == 3){
		pz++;
		py++;
	}
	if (myEdge.y == 5){
		px++;
	}
	if (myEdge.y == 6){
		px++;
		py++;
	}
	if (myEdge.y == 7){
		py++;
	}
	serialVec second;
	second.x = px;
	second.y = py;
	second.z = pz;
	return make_tuple(first, second);
    }

    int findInfo(float isovalue){
	//printf("Finding Info\n");
	for (int i = 0; (i < cubeCount()); i++){
	//	printf("Find Cube %i ...", i);
		cube currentCube = findCube(i);
	//	printf("Found Cube. \nExtracting Case ...");
		mcCase currentCase(8);
		for (unsigned char i = 0; (i < 8); i++) {
			currentCase[i] = (currentCube[i] >= isovalue);
		}
	//	printf("Found Case. \nLooking up Case...");

		vector<edge> mcEdgelist = m_table.lookup(currentCase);
		printf("Case looked up. Found %u edges.\n", mcEdgelist.size());
		int j = 0;
		face* currentFace;
		for (edge currentEdge : mcEdgelist){
			if (j % 3 == 0) currentFace = new face();
	//		printf("Finding Edge Coords...");
			vertexPos currentPos = findCoords(i, currentEdge);
	//		vec3 first(std::get<0>(currentPos).x, std::get<0>(currentPos).y, std::get<0>(currentPos).z);
	//		vec3 second(std::get<1>(currentPos).x, std::get<1>(currentPos).y, std::get<1>(currentPos).z);
	//		printf("Found Edge Coords %f/%f/%f - %f/%f/%f \nGetting ID from Map...", first.x, first.y, first.z, second.x, second.y, second.z);	
			ID currentID = mapGet(currentPos);
	//		printf("Got ID %i \n", currentID);
			(*currentFace)[j%3] = currentID;
			if (j % 3 == 2) { 
				m_Info.faceList.push_back(*currentFace);
				delete currentFace;
			} 
			j++;
	//		printf("Next iteration \n");
		}	
	}
	//printf("All iterations done \n");
	m_Info.vertexPosList = mapMakeList();
	printf("Info Found with %u faces \n", m_Info.faceList.size());
	return 0;
    }

    int computeBlock(float isovalue){
	printf("computing block\n");
	vector<Vertex> vertexList(m_Info.vertexPosList.size());

	for (int i = 0; (i < m_Info.vertexPosList.size()); i++) {
		vertexList[i].coord = interpolate(m_Info.vertexPosList[i], isovalue);
	}

	for (face currentFace : m_Info.faceList){
		Vertex& one = vertexList[IDtoArrayPos(currentFace[0],
						m_Info.activeVertexCount)];
		Vertex& two = vertexList[IDtoArrayPos(currentFace[1],
						m_Info.activeVertexCount)];
		Vertex& three = vertexList[IDtoArrayPos(currentFace[2],
						m_Info.activeVertexCount)];
		adjustNormals(one, two, three);
	}
	m_Info.vertexList = vertexList;
	printf("Block computed\n");
	writeOBJ(); 
	return 0;
    }
	
#include <fstream>

    void writeOBJ(){
	std::ofstream myfile;
	myfile.open("output.obj");
	printf("Writing %u Vertices to File \n", m_Info.vertexList.size());
	for (Vertex vertex : m_Info.vertexList){
		myfile << "v " << vertex.coord.x << " " << vertex.coord.y << " " <<
			vertex.coord.z << "\n";
	}
	for (Vertex vertex : m_Info.vertexList){
		myfile << "vn " << vertex.normal.x << " " << vertex.normal.y << " " <<
			vertex.normal.z << "\n";
	}
	printf("Writing %u Faces to File \n", m_Info.faceList.size());
	for (face currentFace : m_Info.faceList){
		myfile << "f " << currentFace[0] << "//" << currentFace[0] 
		       << " "  << currentFace[1] << "//" << currentFace[1] 
		       << " "  << currentFace[2] << "//" << currentFace[2] << "\n";
	}
    }

    int communicateGlobalIDs(std::vector<block_boilerplate> globalBlocks){
	printf("CommunicateGlobalIDs called \n");
	return 0;
    }	 

    int sendPassive(std::vector<block_boilerplate> globalBlocks){
	printf("sendPassive called \n");
	return 0;
    }

    int sendActive(std::vector<block_boilerplate> globalBlocks){
	printf("sendActive called \n");
	return 0;
    }

    HPX_DEFINE_COMPONENT_ACTION(block, initialize);
    HPX_DEFINE_COMPONENT_ACTION(block, getBoilerplate);
    HPX_DEFINE_COMPONENT_ACTION(block, findInfo);
    HPX_DEFINE_COMPONENT_ACTION(block, computeBlock);
    HPX_DEFINE_COMPONENT_ACTION(block, communicateGlobalIDs);
    HPX_DEFINE_COMPONENT_ACTION(block, sendPassive);
    HPX_DEFINE_COMPONENT_ACTION(block, sendActive);

  private:

  BlockInfo	m_Info;
  Table         m_table;
  vertexMap 	m_Map;
  data 	  	m_dat;
  int	  	m_activeVertexCount;
  vec3 		position_;
  int		m_size; 
  };
}

HPX_REGISTER_ACTION_DECLARATION(server::block::initialize_action, block_initialize_action);
HPX_REGISTER_ACTION_DECLARATION(server::block::getBoilerplate_action, block_getBoilerplate_action);
HPX_REGISTER_ACTION_DECLARATION(server::block::findInfo_action, block_findInfo_action);
HPX_REGISTER_ACTION_DECLARATION(server::block::computeBlock_action, block_computeBlock_action);
HPX_REGISTER_ACTION_DECLARATION(server::block::communicateGlobalIDs_action, block_communicateGlobalIDs_action);
HPX_REGISTER_ACTION_DECLARATION(server::block::sendPassive_action, block_sendPassive_action);
HPX_REGISTER_ACTION_DECLARATION(server::block::sendActive_action, block_sendActive_action);

#include <hpx/runtime/applier/apply.hpp>
#include <hpx/include/async.hpp>


#include <hpx/include/components.hpp>

class block : public hpx::components::client_base<block, server::block>
{
	typedef hpx::components::client_base<block, server::block> base_type;
public:
	block(){}
	block(hpx::future<hpx::naming::id_type> && gid)
	  : base_type(std::move(gid))
	{}

	int getBlub() const {
		return 5;
	}

	hpx::id_type getID() const {
		return this->get_gid();
	}

	void initialize() const {
		HPX_ASSERT(this->get_gid());
		hpx::apply<server::block::initialize_action>(this->get_gid());
	}

	block_boilerplate getBoilerplate() const {
		HPX_ASSERT(this->get_gid());
		return hpx::async<server::block::getBoilerplate_action>(this->get_gid()).get();		
	}

	hpx::shared_future<int> findInfo(float isovalue) const {
		HPX_ASSERT(this->get_gid());
		return hpx::async<server::block::findInfo_action>(this->get_gid(), isovalue);		
	}

	hpx::shared_future<int> computeBlock(float isovalue) const {
		HPX_ASSERT(this->get_gid());
		return hpx::async<server::block::computeBlock_action>(this->get_gid(), isovalue);
    	}

    	hpx::shared_future<int> communicateGlobalIDs(std::vector<block_boilerplate> block_boilerplates) const {
		HPX_ASSERT(this->get_gid());
		return hpx::async<server::block::communicateGlobalIDs_action>(this->get_gid(), block_boilerplates);
    	}	 

    	hpx::shared_future<int> sendPassive(std::vector<block_boilerplate> block_boilerplates) const {
		HPX_ASSERT(this->get_gid());
		return hpx::async<server::block::sendPassive_action>(this->get_gid(), block_boilerplates);
    	}

    	hpx::shared_future<int> sendActive(std::vector<block_boilerplate> block_boilerplates) const {
		HPX_ASSERT(this->get_gid());
		return hpx::async<server::block::sendActive_action>(this->get_gid(), block_boilerplates);
    	}
};

