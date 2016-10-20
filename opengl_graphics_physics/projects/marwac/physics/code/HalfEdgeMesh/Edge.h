class Vertex;
class Face;

class Edge
{
public:
	Vertex *vertex, *midVertex;
	Edge *next;
	Edge *pair;
	Face *face;

	Edge();
	~Edge();

private:

};

