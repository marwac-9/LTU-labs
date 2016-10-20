#include <vector>
#include <map>
#include "MyMathLib.h"
#include "BoundingBox.h"
#include <string>

class OBJ
{
public:
	std::vector<unsigned int> indices;
	std::vector<mwm::Vector3> indexed_vertices;
	std::vector<mwm::Vector2> indexed_uvs;
	std::vector<mwm::Vector3> indexed_normals;
	int ID;
	std::string name;
	bool LoadOBJ(
		const char * path,
		std::vector<mwm::Vector3> & out_vertices,
		std::vector<mwm::Vector2> & out_uvs,
		std::vector<mwm::Vector3> & out_normals
		);
	void indexVBO(
		std::vector<mwm::Vector3> & in_vertices,
		std::vector<mwm::Vector2> & in_uvs,
		std::vector<mwm::Vector3> & in_normals
		);
	void LoadAndIndexOBJ(char* path);
	
	mwm::Vector3 GetDimensions();
	mwm::Vector3 CenterOfMass();
	
	mwm::Vector3 dimensions;
	mwm::Vector3 center_of_mass;
	mwm::Vector3 CalcMaxValuesInWorld(const mwm::Matrix4& modelMatrix, const mwm::Vector3& position) const;
	MinMax CalcValuesInWorld(const mwm::Matrix4& modelMatrix, const mwm::Vector3& position) const;

	void CalculateDimensions();
private:
	struct PackedVertex{
		mwm::Vector3 position;
		mwm::Vector2 uv;
		mwm::Vector3 normal;
		bool operator<(const PackedVertex that) const{
			return memcmp((void*)this, (void*)&that, sizeof(PackedVertex))>0;
		};
	};
	static bool getSimilarVertexIndex_fast(
		PackedVertex & packed,
		std::map<PackedVertex, unsigned int> & VertexToOutIndex,
		unsigned int & result
		);
};

