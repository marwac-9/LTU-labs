#include "config.h"
#ifdef __linux__ 
#include <sys/resource.h>
#elif _WIN32 || _WIN64
#include "windows.h"
#include "psapi.h"
#endif	
#include <stdio.h>
#include <cstring>
#include <time.h>
#include <stdlib.h>
#include <iostream>
#include "GraphicsManager.h"
#include "GraphicsStorage.h"
#include "Object.h"
#include "Mesh.h"
#include "OBJ.h"
#include "HalfEdgeMesh.h"
#include "Scene.h"
#include <chrono>
#include <ctime>
#include "PoolParty.h"

void Subdivide(HalfEdgeMesh* mesh, int count);
HalfEdgeMesh* ConstructHMesh(Object* HalfMesh, int modelIndex);


int
main(int argc, const char** argv)
{
  
  
    Object* Scene = Scene::Instance()->build();
    Object* HalfMeshObj = Scene::Instance()->addChild(Scene);

    //load all objs
    printf("\nLOADING OBJs\n");
    GraphicsManager::LoadOBJs("Resources/models.txt");
    printf("\nDONE\n");
	
	
    if(argc < 2)
    {
       std::cerr << "Usage:\n" << argv[0] << "numberOfSubdivisions\nor\nnumberOfSubdivisions modelName" << std::endl;
    }
    else if (argc < 3) {
	int i = atoi( argv[1] );
        ConstructHMesh(HalfMeshObj,5);
        Subdivide(reinterpret_cast<HalfEdgeMesh*>(HalfMeshObj->mesh),i);
    }
    else if(argc < 4)
    {
	int i = atoi( argv[1] );

	if(strcmp(argv[2],"tetra") == 0)
	{
	    ConstructHMesh(HalfMeshObj,0); // numbers depend on the order of models loaded from models.txt
	}
	else if(strcmp(argv[2],"pyramid") == 0)
	{
	    ConstructHMesh(HalfMeshObj,1); // numbers depend on the order of models loaded from models.txt
	}
	else if(strcmp(argv[2],"cube") == 0)
	{
	    ConstructHMesh(HalfMeshObj,3); // numbers depend on the order of models loaded from models.txt
	}
	else if(strcmp(argv[2],"sphere") == 0)
	{
	    ConstructHMesh(HalfMeshObj,4); // numbers depend on the order of models loaded from models.txt
	}
	else if(strcmp(argv[2],"icosphere") == 0)
	{
	    ConstructHMesh(HalfMeshObj,5); // numbers depend on the order of models loaded from models.txt
	}
	else
	{
	    printf("\nWRONG MODEL NAME\n");
	    return 0;
	}
	Subdivide(reinterpret_cast<HalfEdgeMesh*>(HalfMeshObj->mesh),i);
    }
    
    /*
    Vector3 vectors[100000];
    for(int i = 0; i < 100000; ++i)
    {
      vectors[i].vect[0]=5;
      vectors[i].vect[1]=5;
      vectors[i].vect[2]=5;      
    }
    
    
    std::chrono::time_point<std::chrono::high_resolution_clock> start, end;
    start = std::chrono::high_resolution_clock::now();
    
    
    for(int i = 0; i < 100000; ++i)
    {
      vectors[i].NormalizeSSE();
    }
    
    
    end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed_seconds = end-start;
    //std::time_t end_time = std::chrono::high_resolution_clock::to_time_t(end);
    
    std::cout << "\nNormalized in: " << elapsed_seconds.count() << "s\n";
    */
    return 0;
}


void Subdivide(HalfEdgeMesh* mesh, int count)
{
    for(int i = 0; i < count; ++i)
    {
	std::chrono::time_point<std::chrono::high_resolution_clock> start, end;
	start = std::chrono::high_resolution_clock::now();
        mesh->Subdivide();
	end = std::chrono::high_resolution_clock::now();
	std::chrono::duration<double> elapsed_seconds = end-start;
	//std::time_t end_time = std::chrono::high_resolution_clock::to_time_t(end);
    
	std::cout << "\nPass " << i+1 << " subdivided in: " << elapsed_seconds.count() << "s\n";

#ifdef __linux__ 
	struct rusage r_usage;

	getrusage(RUSAGE_SELF, &r_usage);

	printf("Memory usage: %ld kB\n", r_usage.ru_maxrss);
	}
#elif _WIN32 || _WIN64
	PROCESS_MEMORY_COUNTERS pmc;
	GetProcessMemoryInfo(GetCurrentProcess(), &pmc, sizeof(pmc));
	//SIZE_T virtualMemUsedByMe = pmc.PrivateUsage;
	SIZE_T physMemUsedByMe = pmc.WorkingSetSize;
	printf("Memory usage: %lu kB\n", physMemUsedByMe / 1024);

#endif	
    }
}

HalfEdgeMesh* ConstructHMesh(Object* HalfMesh, int modelIndex)
{
    printf("\nCreating half edge mesh\n");
    HalfEdgeMesh* newHMesh = new HalfEdgeMesh();
    newHMesh->Construct(*GraphicsStorage::objects[modelIndex]); // probably all i need
    HalfMesh->AssignMesh(newHMesh);
    printf("DONE\n");
    return newHMesh;
}


