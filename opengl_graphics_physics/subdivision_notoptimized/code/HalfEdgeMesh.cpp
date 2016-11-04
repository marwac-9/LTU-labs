#include <vector>
#include <map>
#include "HalfEdgeMesh.h"
#include "OBJ.h"
#include "Vertex.h"
#include "Edge.h"
#include "Face.h"

using namespace mwm;

HalfEdgeMesh::HalfEdgeMesh()
{

}

HalfEdgeMesh::~HalfEdgeMesh()
{
}

void HalfEdgeMesh::Construct(OBJ &object)
{

	//copy all data from vectors to half edge mesh vectors
	//vertice data
	for (size_t i = 0; i < object.indexed_vertices.size(); i++)
	{
		Vertex* newVertex = new Vertex();
		newVertex->pos = object.indexed_vertices.at(i);
		newVertex->normal = object.indexed_normals.at(i);
		newVertex->tex = object.indexed_uvs.at(i);
		vertices.push_back(newVertex);
	}

	//connecting
	for (size_t i = 0; i < object.indices.size(); i += 3)
	{
		//get vertex index from element array containing indices
		int vertexIndex = object.indices.at(i);
		int vertexIndex2 = object.indices.at(i + 1);
		int vertexIndex3 = object.indices.at(i + 2);
		//let's get vertices we wanna work with
		Vertex* vertice1 = vertices.at(vertexIndex);
		Vertex* vertice2 = vertices.at(vertexIndex2);
		Vertex* vertice3 = vertices.at(vertexIndex3);


		//create new edges
		Edge* newEdge1 = new Edge();
		Edge* newEdge2 = new Edge();
		Edge* newEdge3 = new Edge();

		//connect vertices to edges
		newEdge1->vertex = vertice1;
		newEdge2->vertex = vertice2;
		newEdge3->vertex = vertice3;

		//connect edges to vertices
		vertice1->edge = newEdge1;
		vertice2->edge = newEdge2;
		vertice3->edge = newEdge3;

		//set pair and mid to nullptr
		newEdge1->pair = nullptr;
		newEdge2->pair = nullptr;
		newEdge3->pair = nullptr;
		newEdge1->midVertex = nullptr;
		newEdge2->midVertex = nullptr;
		newEdge3->midVertex = nullptr;

		//connect edges with next
		newEdge1->next = newEdge2;
		newEdge2->next = newEdge3;
		newEdge3->next = newEdge1;

		edges.push_back(newEdge1);
		edges.push_back(newEdge2);
		edges.push_back(newEdge3);

	}

	//connect edges to faces and faces to edges
	for (size_t i = 0; i < edges.size(); i+=3)
	{
		Face* newFace = new Face();
		newFace->edge = edges.at(i);

		//edges.at(i).face = &faces.at(i);
		//edges.at(i+1).face = &faces.at(i);
		//edges.at(i+2).face = &faces.at(i);

		edges.at(i)->face = newFace;
		edges.at(i)->next->face = newFace;
		edges.at(i)->next->next->face = newFace;

		faces.push_back(newFace);
	}

	//find pairs
	//first we need to find all edges that have source vertex same as edge->next->vertex
	//vecOfEdges
	std::vector<std::vector<Edge*>> pairCandidatesForEachEdge;
	//edges with same source(edge->vertex) as edge->next->vertex
	std::vector<Edge*> edgesWithSameSource;
	for (size_t i = 0; i < edges.size(); i++)
	{	
		for (size_t j = 0; j < edges.size(); j++)
		{
			//printf("edge J %f and edgeNext i %f\n", edges.at(j)->vertex->pos.vect[0], edges.at(i)->next->vertex->pos.vect[0]);
			if (checkIfSameVect(edges.at(j)->vertex->pos, edges.at(i)->next->vertex->pos))
			{
				//we might try to avoid adding an edge that is next for the edges.at(i) here as it's not allowed to be a pair
				edgesWithSameSource.push_back(edges.at(j));
			}
		}
		pairCandidatesForEachEdge.push_back(edgesWithSameSource);
		edgesWithSameSource.clear();
	}

	//now when we added candidates we need to test them if their next source is the same, if true then we got a pair
	for (size_t i = 0; i < edges.size(); i++)
	{
		for (size_t j = 0; j < pairCandidatesForEachEdge.at(i).size(); j++)
		{
			if (checkIfSameVect(pairCandidatesForEachEdge.at(i).at(j)->next->vertex->pos, edges.at(i)->vertex->pos))
			{
				edges.at(i)->pair = pairCandidatesForEachEdge.at(i).at(j);
				//edges.at(j)->pair = edges.at(i);
			}
		}
	}

	for (size_t i = 0; i < edges.size(); i++)
	{
		if (edges.at(i)->next == edges.at(i)->pair)
		{
			printf("error %d \n", i);
		}
	}
}
bool HalfEdgeMesh::checkIfSameVect(Vector3 &vect1, Vector3 &vect2)
{
	if (vect1.vect[0] == vect2.vect[0] && vect1.vect[1] == vect2.vect[1] && vect1.vect[2] == vect2.vect[2]) {
		return true;
	}
	else{
		return false;
	}
}

void HalfEdgeMesh::Subdivide()
{
	CalculateOldPosition();
	CalculateMidpointPosition();
	SplitHalfEdges();
	UpdateConnections();
	UpdateVertexPositions();
	//we could do the new recalculation of normals 
	//for each vertex we find all neighbours
	//then we calculate new normals from them
	//traversing code is already existent in CalculateOldPosition function
}


void HalfEdgeMesh::CalculateOldPosition()
{
	for (size_t i = 0; i < edges.size(); i++)
	{
		if (edges[i]->next == edges[i]->pair)
		{
			printf("\nFEL\n");
		}
	}

	for (unsigned int i = 0; i < vertices.size(); i++){
		//Get the vertex to which we will calculate its new position
		Vertex* vertex = vertices.at(i);
		//Get the half-edge of the vertex we are working on
		Edge* halfedge = vertices.at(i)->edge;

		//Sum of all neighbour vertices
		Vector3 neighbourSum;
		//Amount of neighbors around the vertex
		float n = 0;

		//we find here all neighboring vertices of current vertex of current edge, we can use this traversing for better calculation of normals after subdivision 
		//starting edge, its vertex
		Edge* currentEdge = halfedge->next;
		//traverse around vertice sum neighboring vertices
		do
		{
			neighbourSum = neighbourSum + currentEdge->vertex->pos;
			n++;

			//get next neighbour
			currentEdge = currentEdge->next->pair->next;
		} while (currentEdge != halfedge->next);

		//Calculate b which is a function used by the equation which calculates the new position
		//Using  Warren and Weimer's equation which doesnt have expensive trigonometric functions
		float b = 3.0f / (n*(n + 2.0f));

		vertex->newPos = (1.0f - n*b)*vertex->pos + b*neighbourSum;
	}
}

void HalfEdgeMesh::CalculateMidpointPosition()
{
	/*
	Iterate edges calculate the midpoint vertex
	*/

	/*Iterate HE around face . do while until same HE then pair.next.next.origin. after do while jaha. then got all 4 vertices*/
	size_t halfEdgeListSize = edges.size();
	for (size_t i = 0; i < halfEdgeListSize; i++){
		//Get the halfedge to which we will calculate midpoint position of the new vertex
		Edge* halfedge = edges.at(i);

		if (halfedge->midVertex == nullptr){
			//to calculate midpoint position i need first all involved vertices
			Vertex* v1 = halfedge->vertex;
			Vertex* v2 = halfedge->next->vertex;
			Vertex* v3 = halfedge->next->next->vertex;
			Vertex* v4 = halfedge->pair->next->next->vertex;

			Vector3 vertexPosSum = 3 * (v1->pos + v2->pos) + v3->pos + v4->pos;

			//we only calculate midpoint and assign it to the edge and it's pair
			//we are not performing any splitting or reconnections here
			Vertex* midpoint = new Vertex;
			Vector3 calculatedPosition = vertexPosSum / 8.0f;
			Vector2 calculatedUVs = (v1->tex + v2->tex) / 2.0f;
			Vector3 calculatedNormal = (v1->normal + v2->normal + v3->normal + v4->normal) / 4.0f;
			calculatedNormal.vectNormalize();
			midpoint->pos = calculatedPosition; // there is no reason to set this as we copy newPos to pos later on
			midpoint->tex = calculatedUVs;
			midpoint->normal = calculatedNormal;
			midpoint->newPos = calculatedPosition;
			halfedge->midVertex = midpoint;
			halfedge->pair->midVertex = midpoint;


			vertices.push_back(midpoint);
		}
	}
}

void HalfEdgeMesh::SplitHalfEdges()
{
	//here we want to split edges and connect pairs
	size_t hEdgesSize = edges.size();
	//iterate trought edge list and split edge if not splitted 
	for (size_t i = 0; i < hEdgesSize; i++){
		//Create new half-edge
		Edge* newHEdge = new Edge;
		newHEdge->vertex = edges.at(i)->midVertex;
		newHEdge->vertex->edge = newHEdge;
		newHEdge->next = edges.at(i)->next;
		newHEdge->face = edges.at(i)->face;

		edges.at(i)->next = newHEdge;
		newHEdge->pair = edges.at(i)->pair;

		edges.push_back(newHEdge);
	}

	for (size_t i = hEdgesSize; i < edges.size(); i++){
		edges.at(i)->pair->pair = edges.at(i);
	}
}

void HalfEdgeMesh::UpdateVertexPositions()
{
	//we copy new positions into original positions
	for (unsigned int i = 0; i < vertices.size(); i++){
		vertices.at(i)->pos = vertices.at(i)->newPos;
	}
}


void HalfEdgeMesh::UpdateConnections()
{
	size_t faceListSize = faces.size();
	for (size_t i = 0; i < faceListSize; i++){
		Face* leftFace = faces.at(i);
		Face* topFace = new Face;
		Face* rightFace = new Face;
		Face* middleFace = new Face;

		Edge* innerLeftEdge = new Edge;
		Edge* innerTopEdge = new Edge;
		Edge* innerRightEdge = new Edge;

		//New triangle, middle
		Edge* innerRightMidEdge = new Edge;
		Edge* innerLeftMidEdge = new Edge;
		Edge* innerBottomMidEdge = new Edge;

		Edge* oldLeftEdge = faces.at(i)->edge;
		Edge* newLeftEdge = faces.at(i)->edge->next;
		Edge* oldRightEdge = faces.at(i)->edge->next->next;
		Edge* newRightEdge = faces.at(i)->edge->next->next->next;
		Edge* oldBottomEdge = faces.at(i)->edge->next->next->next->next;
		Edge* newBottomEdge = faces.at(i)->edge->next->next->next->next->next;



		//Connectivity for inner edges
		//Bottom left face
		innerLeftEdge->vertex = newLeftEdge->vertex;  

		//connectivity
		innerLeftEdge->next = newBottomEdge;
		oldLeftEdge->next = innerLeftEdge;

		innerLeftEdge->face = leftFace; 

		//fixing pair for the inner middle face
		innerLeftEdge->pair = innerLeftMidEdge;
		innerLeftMidEdge->pair = innerLeftEdge; 

		//Top face
		topFace->edge = newLeftEdge; 
		innerTopEdge->vertex = newRightEdge->vertex; 

		//connectivity
		innerTopEdge->next = newLeftEdge; 
		oldRightEdge->next = innerTopEdge; 

		//setting all edges to the new top face
		newLeftEdge->face = topFace;
		oldRightEdge->face = topFace;
		innerTopEdge->face = topFace; 

		//fixing pair for the inner middle face
		innerTopEdge->pair = innerBottomMidEdge;
		innerBottomMidEdge->pair = innerTopEdge; 

		//Bottom right face
		rightFace->edge = newRightEdge;
		innerRightEdge->vertex = newBottomEdge->vertex; 

		innerRightEdge->next = newRightEdge; 
		oldBottomEdge->next = innerRightEdge; 

		//setting all edges to the new right face
		newRightEdge->face = rightFace;
		oldBottomEdge->face = rightFace;
		innerRightEdge->face = rightFace; 

		//fixing pair for the inner middle face
		innerRightEdge->pair = innerRightMidEdge;
		innerRightMidEdge->pair = innerRightEdge; 

		//Middle face
		middleFace->edge = innerRightMidEdge; 
		innerRightMidEdge->vertex = newRightEdge->vertex; 
		innerRightMidEdge->next = innerLeftMidEdge; 

		innerLeftMidEdge->vertex = newBottomEdge->vertex;
		innerLeftMidEdge->next = innerBottomMidEdge; 

		innerBottomMidEdge->vertex = newLeftEdge->vertex;  
		innerBottomMidEdge->next = innerRightMidEdge; 

		//setting all edges to the new middle face
		innerRightMidEdge->face = middleFace;
		innerLeftMidEdge->face = middleFace;
		innerBottomMidEdge->face = middleFace;

		//pushing new faces
		faces.push_back(topFace);
		faces.push_back(rightFace);
		faces.push_back(middleFace);

		//pushing new edges
		edges.push_back(innerLeftEdge);
		edges.push_back(innerTopEdge);
		edges.push_back(innerRightEdge);
		edges.push_back(innerRightMidEdge);
		edges.push_back(innerLeftMidEdge);
		edges.push_back(innerBottomMidEdge);
	}
}


void HalfEdgeMesh::ExportMeshToOBJ(HalfEdgeMesh* mesh, OBJ* newOBJ)
{
	//GENERATING RENDERABLE DATA FORMAT 

	//let's use map for vertex pointer and index 
	std::map<const Vertex*, int> indexMap;

	//for each vertice export position normal and uv
	for (size_t i = 0; i < mesh->vertices.size(); i++)
	{
		newOBJ->indexed_vertices.push_back(mesh->vertices.at(i)->pos);
		newOBJ->indexed_uvs.push_back(mesh->vertices.at(i)->tex);
		newOBJ->indexed_normals.push_back(mesh->vertices.at(i)->normal);
		indexMap[mesh->vertices.at(i)] = i;
	}

	for (size_t i = 0; i < mesh->faces.size(); i++){
		Edge* faceTraverser = mesh->faces.at(i)->edge;
		do
		{
			int index;
			//Iterator
			//we search for the same vertex in vertex index map
			std::map<const Vertex*, int>::iterator it = indexMap.find(faceTraverser->vertex);
			index = it->second;
			//then we push back it's index
			newOBJ->indices.push_back(index);

			faceTraverser = faceTraverser->next;
		} while (faceTraverser != mesh->faces.at(i)->edge);
	}
}