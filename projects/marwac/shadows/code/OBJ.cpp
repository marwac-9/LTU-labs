#define _CRT_SECURE_NO_DEPRECATE
#include "OBJ.h"
#include <algorithm>
using namespace mwm;
void OBJ::LoadAndIndexOBJ(char* path)
{
	// Read our .obj file
	std::vector<Vector3> vertices;
	std::vector<Vector2> uvs;
	std::vector<Vector3> normals; // Won't be used at the moment.
	bool res = LoadOBJ(path, vertices, uvs, normals);

	indexVBO(vertices, uvs, normals);
}

void OBJ::indexVBO(
	std::vector<Vector3> & in_vertices,
	std::vector<Vector2> & in_uvs,
	std::vector<Vector3> & in_normals
	)
{
	std::map<PackedVertex, unsigned int> VertexToOutIndex;

	// For each input vertex
	for (unsigned int i = 0; i<in_vertices.size(); i++){

		PackedVertex packed = { in_vertices[i], in_uvs[i], in_normals[i] };


		// Try to find a similar vertex in out_XXXX
		unsigned int index;
		bool found = getSimilarVertexIndex_fast(packed, VertexToOutIndex, index);

		if (found){ // A similar vertex is already in the VBO, use it instead !
			indices.push_back(index);
		}
		else{ // If not, it needs to be added in the output data.
			indexed_vertices.push_back(in_vertices[i]);
			indexed_uvs.push_back(in_uvs[i]);
			indexed_normals.push_back(in_normals[i]);
			unsigned int newindex = (unsigned int)indexed_vertices.size() - 1;
			indices.push_back(newindex);
			VertexToOutIndex[packed] = newindex;
		}
	}
	
	CalculateDimensions();
}

bool OBJ::getSimilarVertexIndex_fast(
	PackedVertex & packed,
	std::map<PackedVertex, unsigned int> & VertexToOutIndex,
	unsigned int & result
	){
	std::map<PackedVertex, unsigned int>::iterator it = VertexToOutIndex.find(packed);
	if (it == VertexToOutIndex.end()){
		return false;
	}
	else{
		result = it->second;
		return true;
	}
}

bool OBJ::LoadOBJ(
	const char * path,
	std::vector<Vector3> & out_vertices,
	std::vector<Vector2> & out_uvs,
	std::vector<Vector3> & out_normals
	)
{
	printf("\nLoading OBJ file %s...\n", path);

	std::vector<unsigned int> vertexIndices, uvIndices, normalIndices;
	std::vector<Vector3> temp_vertices;
	std::vector<Vector2> temp_uvs;
	std::vector<Vector3> temp_normals;


	FILE * file;
	file = fopen(path, "r");
	if (file == NULL){
		printf("Impossible to open the file ! Are you in the right path ? See Tutorial 1 for details\n");
		getchar();
		return false;
	}
	while (1){

		char lineHeader[128];
		// read the first word of the line
		int res = fscanf(file, "%s", lineHeader);
		if (res == EOF)
			break; // EOF = End Of File. Quit the loop.

		// else : parse lineHeader

		if (strcmp(lineHeader, "v") == 0){
			Vector3 vertex;
			fscanf(file, "%f %f %f\n", &vertex.vect[0], &vertex.vect[1], &vertex.vect[2]);
			temp_vertices.push_back(vertex);
		}
		else if (strcmp(lineHeader, "vt") == 0){
			Vector2 uv;
			fscanf(file, "%f %f\n", &uv.vect[0], &uv.vect[1]);
			uv.vect[1] = -uv.vect[1]; // Invert V coordinate since we will only use DDS texture, which are inverted. Remove if you want to use TGA or BMP loaders.
			temp_uvs.push_back(uv);
		}
		else if (strcmp(lineHeader, "vn") == 0){
			Vector3 normal;
			fscanf(file, "%f %f %f\n", &normal.vect[0], &normal.vect[1], &normal.vect[2]);
			temp_normals.push_back(normal);
		}
		else if (strcmp(lineHeader, "f") == 0){
			std::string vertex1, vertex2, vertex3;
			unsigned int vertexIndex[3], uvIndex[3], normalIndex[3];
			int matches = fscanf(file, "%d/%d/%d %d/%d/%d %d/%d/%d\n", &vertexIndex[0], &uvIndex[0], &normalIndex[0], &vertexIndex[1], &uvIndex[1], &normalIndex[1], &vertexIndex[2], &uvIndex[2], &normalIndex[2]);
			if (matches != 9){
				printf("File can't be read by our simple parser :-( Try exporting with other options\n");
				return false;
			}
			vertexIndices.push_back(vertexIndex[0]);
			vertexIndices.push_back(vertexIndex[1]);
			vertexIndices.push_back(vertexIndex[2]);
			uvIndices.push_back(uvIndex[0]);
			uvIndices.push_back(uvIndex[1]);
			uvIndices.push_back(uvIndex[2]);
			normalIndices.push_back(normalIndex[0]);
			normalIndices.push_back(normalIndex[1]);
			normalIndices.push_back(normalIndex[2]);
		}
		else{
			// Probably a comment, eat up the rest of the line
			char stupidBuffer[1000];
			fgets(stupidBuffer, 1000, file);
		}

	}

	// For each vertex of each triangle
	for (unsigned int i = 0; i<vertexIndices.size(); i++){

		// Get the indices of its attributes
		unsigned int vertexIndex = vertexIndices[i];
		unsigned int uvIndex = uvIndices[i];
		unsigned int normalIndex = normalIndices[i];

		// Get the attributes thanks to the index
		Vector3 vertex = temp_vertices[vertexIndex - 1];
		Vector2 uv = temp_uvs[uvIndex - 1];
		Vector3 normal = temp_normals[normalIndex - 1];

		// Put the attributes in buffers
		out_vertices.push_back(vertex);
		out_uvs.push_back(uv);
		out_normals.push_back(normal);

	}
	printf("Finished loading OBJ");
	fclose(file);
	return true;
}

void OBJ::CalculateDimensions()
{
	Vector3 minValues;
	Vector3 maxValues;
	
	minValues = indexed_vertices[0];
	maxValues = indexed_vertices[0];
	for(size_t i = 0; i < indexed_vertices.size(); ++i)
	{
		Vector3 currentVertex = indexed_vertices[i];
		maxValues[0] = std::max(maxValues[0],currentVertex[0]);
		minValues[0] = std::min(minValues[0],currentVertex[0]);
		maxValues[1] = std::max(maxValues[1],currentVertex[1]);
		minValues[1] = std::min(minValues[1],currentVertex[1]);
		maxValues[2] = std::max(maxValues[2],currentVertex[2]);
		minValues[2] = std::min(minValues[2],currentVertex[2]);		
	}
	
	dimensions.vect[0] = maxValues[0]-minValues[0];
	dimensions.vect[1] = maxValues[1]-minValues[1];
	dimensions.vect[2] = maxValues[2]-minValues[2];
	
	this->center_of_mass = minValues + this->dimensions/2.f;

}

Vector3 OBJ::GetDimensions()
{
	return this->dimensions;
}

Vector3 OBJ::CenterOfMass()
{
	return this->center_of_mass;
}

Vector3 OBJ::CalcMaxValuesInWorld(const Matrix4& modelMatrix, const Vector3& position) const
{
	Matrix3 modelM = modelMatrix.ConvertToMatrix3();
	Vector3 maxValues = modelM * indexed_vertices[0];
	Vector3 currentVertex;
	for (size_t i = 0; i < indexed_vertices.size(); ++i)
	{
		currentVertex = modelM * indexed_vertices[i];
		maxValues[0] = std::max(maxValues[0], currentVertex[0]);
		maxValues[1] = std::max(maxValues[1], currentVertex[1]);
		maxValues[2] = std::max(maxValues[2], currentVertex[2]);
	}

	return maxValues + position;
}

MinMax OBJ::CalcValuesInWorld(const Matrix4& modelMatrix, const Vector3& position) const
{
	Matrix3 modelM = modelMatrix.ConvertToMatrix3();
	Vector3 maxValuesW = modelM * indexed_vertices[0];
	Vector3 minValuesW = modelM * indexed_vertices[0];
	Vector3 currentVertex;
	for (size_t i = 0; i < indexed_vertices.size(); ++i)
	{
		currentVertex = modelM * indexed_vertices[i];
		maxValuesW[0] = std::max(maxValuesW[0], currentVertex[0]);
		minValuesW[0] = std::min(minValuesW[0], currentVertex[0]);
		maxValuesW[1] = std::max(maxValuesW[1], currentVertex[1]);
		minValuesW[1] = std::min(minValuesW[1], currentVertex[1]);
		maxValuesW[2] = std::max(maxValuesW[2], currentVertex[2]);
		minValuesW[2] = std::min(minValuesW[2], currentVertex[2]);
	}
	MinMax mm;
	mm.max = maxValuesW + position;
	mm.min = minValuesW + position;
	return mm;
}
