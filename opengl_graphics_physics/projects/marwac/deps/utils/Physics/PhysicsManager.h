#pragma once
#include <vector>
#include "Object.h"
#include <unordered_set>
#include "OverlapPair.h"
#include "ObjectPoint.h"
#include "Vector3.h"

struct Contact
{
	mwm::Vector3 contactPoint;
	mwm::Vector3 contactNormal;
	float penetration = -1;
	Object* one = nullptr;
	Object* two = nullptr;

	bool operator<(const Contact& rhs) const
	{
		return penetration < rhs.penetration;  
	}
};

//std specialization
namespace std
{
	template <>
	class hash < Contact* >
	{
	public:		
		size_t operator()(const Contact* val) const {
			return val->contactPoint.a ^ val->contactPoint.b ^ val->contactPoint.c;
			//return val->contactPoint.squareMag();//works

		}
	};

	template <>
	class equal_to < Contact* >
	{
	public:
		bool operator()(const Contact* lhs, const Contact* rhs) const
		{
			return (lhs->contactPoint == rhs->contactPoint);
		}
	};
}

class PhysicsManager
{
public:
	static PhysicsManager* Instance();

	enum axis
	{
		x,
		y,
		z
	};

	
	void AddObject(Object* obj);
	void SortAndSweep();
	void NarrowTestSAT(float deltaTime);
	
	mwm::Vector3 defAabbColor;
	mwm::Vector3 defObbColor;
	std::vector<ObjectPoint*> xAxis;
	std::vector<ObjectPoint*> yAxis;
	std::vector<ObjectPoint*> zAxis;
	std::unordered_set<OverlapPair> fullOverlaps;
	std::unordered_set<OverlapPair> satOverlaps;

	mwm::Vector3 gravity = mwm::Vector3(0.f, -9.f, 0.f);

	
	void Clear();
private:
	PhysicsManager();
	~PhysicsManager();
	//copy
	PhysicsManager(const PhysicsManager&);
	//assign
	PhysicsManager& operator=(const PhysicsManager&);
	void ProcessContact(Contact* contact, mwm::Vector3&vel1, mwm::Vector3& ang_vel1, mwm::Vector3&vel2, mwm::Vector3& ang_vel2, float dtInv);
	void CalcFaceVertices(const mwm::Vector3& pos, mwm::Vector3* vertices, const mwm::Vector3& axis, const mwm::Matrix3& model, const mwm::Vector3& halfExtents, bool counterClockwise = true);
	void GenerateContactPointToFace2(mwm::Vector3 &toCentre, std::unordered_set<Contact*> &contacts, mwm::Vector3& smallestAxis, float smallestPen, Object* oneObj, Object* twoObj, int typeOfCollision, mwm::Vector3 * reference_face, mwm::Vector3 * incident_face);

	size_t DrawPlaneClipContacts(std::vector<mwm::Vector3> &contacts, const mwm::Vector3& normal, size_t vertCount, const mwm::Vector3& normalColor);

	void SortAxis(std::vector<ObjectPoint*>& axisList, axis axisToSort);
	bool CheckBoundingBoxes(Object* obj1, Object* obj2);
	void FlipMTVTest(mwm::Vector3 &mtv, const mwm::Vector3 &toCentre);

	void FilterContactsAgainstReferenceFace(std::vector<mwm::Vector3> &contacts, const mwm::Vector3& refNormal, float pos_offsett, std::unordered_set<Contact*> &contact_points_out, float penetration, Object* one, Object* two);
	
	void ClaculateIncidentAxis(mwm::Vector3& incident_axis, const mwm::Matrix3 &two, mwm::Vector3 smallestAxis);

	void CreateSidePlanesOffsetsAndNormals(const mwm::Vector3& onePosition, int typeOfCollision, mwm::Vector3 &normal1, const mwm::Matrix3 &one, mwm::Vector3 &normal2, float &neg_offset1, mwm::Vector3 oneHalfSize, float &pos_offset1, float &neg_offset2, float &pos_offset2);

	std::vector<mwm::Vector3> ClipFaceToSidePlane(std::vector<mwm::Vector3>& clipPolygon, const mwm::Vector3& normal, float plane_offset);
	void DrawCollisionNormal(Contact* contact);
	void DrawReferenceNormal(Contact* contact, int typeOfCollision);
	void DrawSidePlanes(const mwm::Vector3& normal1, const mwm::Vector3& normal2, const mwm::Vector3& onePosition, int index1, int index2, const mwm::Vector3& oneHalfSize);
	void PositionalCorrection(Object* one, Object* two, float penetration, mwm::Vector3& normal);
	void PositionalImpulseCorrection(Object* one, Object* two, Contact* contact);
	bool IntersectionTest(const Object* oneObj, const Object* twoObj, float& smallestPenetration, mwm::Vector3& smallestAxis, mwm::Vector3& toCentre, int& axisNumRes, int& bestSingleAxis);
	bool overlapOnAxis(const Object* oneObj, const Object* twoObj, const mwm::Vector3 &axis, const int axisNum, int& resAxisNum, const mwm::Vector3 &toCentre, float& smallestPenetration, mwm::Vector3& smallestAxis);
	float penetrationOnAxis(const Object* oneObj, const Object* twoObj, const mwm::Vector3 &axis, const mwm::Vector3 &toCentre);
	float transformToAxis(const mwm::Matrix3 &boxModel, const mwm::Vector3 &axis, const mwm::Vector3 &boxHalfSize);
	void GenerateContacts(mwm::Vector3& MTV, const float& penetration, mwm::Vector3& toCentre, std::unordered_set<Contact*> &contacts, Object* oneObj, Object* twoObj, int axisNumRes, int& bestSingleAxis);

	void DrawFaceDebug(std::unordered_set<Contact*> &contacts, mwm::Vector3 * reference_face, mwm::Vector3 * incident_face, int typeOfCollision);

	void DrawReferenceAndIncidentFace(mwm::Vector3 * reference_face, mwm::Vector3 * incident_face);

	void GenerateContactPointToFace(Object* one, Object* two, const mwm::Vector3 &toCentre, std::unordered_set<Contact*> &contacts, int axisNum, float smallestPen);
	void GenerateContactEdgeToEdge(const mwm::Vector3 &toCentre, std::unordered_set<Contact*> &contacts, mwm::Vector3& smallestAxis, float smallestPen, Object* oneObj, Object* twoObj, int axisNumRes, int& bestSingleAxis);
	mwm::Vector3 contactPoint(const mwm::Vector3 &pOne, const mwm::Vector3 &dOne, float oneSize, const mwm::Vector3 &pTwo, const mwm::Vector3 &dTwo, float twoSize, bool useOne) const;

};

inline bool PhysicsManager::overlapOnAxis(const Object* oneObj, const Object* twoObj, const mwm::Vector3 &axis, const int axisNum, int& resAxisNum, const mwm::Vector3 &toCentre, float& smallestPenetration, mwm::Vector3& smallestAxis)
{
	if (axis.squareMag() < 0.0001) return true;

	mwm::Vector3 axisn = axis.vectNormalize();

	float penetration = penetrationOnAxis(oneObj, twoObj, axisn, toCentre);

	if (penetration < 0) return false;
	if (penetration < smallestPenetration) {
		smallestPenetration = penetration;
		smallestAxis = axisn;
		resAxisNum = axisNum;
	}
	return true;
}


inline float PhysicsManager::penetrationOnAxis(const Object* oneObj, const Object* twoObj, const mwm::Vector3 &axis, const mwm::Vector3 &toCentre)
{
	// Project the half-size of one onto axis
	float oneProject = transformToAxis(oneObj->obb.model, axis, oneObj->obb.halfExtent);
	float twoProject = transformToAxis(twoObj->obb.model, axis, twoObj->obb.halfExtent);

	// Project this onto the axis
	float distance = abs(toCentre.dotAKAscalar(axis));
	//printf("\naxis %f %f %f pen: %f", axis.x, axis.y, axis.z, toCentre.dotAKAscalar(axis));
	// Return the overlap (i.e. positive indicates
	// overlap, negative indicates separation).
	float overlap = oneProject + twoProject - distance;
	return overlap;
}

inline float PhysicsManager::transformToAxis(const mwm::Matrix3 &boxModel, const mwm::Vector3 &axis, const mwm::Vector3 &boxHalfSize)
{
	return
		boxHalfSize.x * abs(axis.dotAKAscalar(boxModel.getAxis(0))) + //get not normalized axes otherwise you will get floating errors!
		boxHalfSize.y * abs(axis.dotAKAscalar(boxModel.getAxis(1))) +
		boxHalfSize.z * abs(axis.dotAKAscalar(boxModel.getAxis(2)));
}

inline mwm::Vector3 PhysicsManager::contactPoint(const mwm::Vector3 &pOne, const mwm::Vector3 &dOne, float oneSize, const mwm::Vector3 &pTwo, const mwm::Vector3 &dTwo, float twoSize, bool useOne) const
{
	// If useOne is true, and the contact point is outside
	// the edge (in the case of an edge-face contact) then
	// we use one's midpoint, otherwise we use two's.

	mwm::Vector3 toSt, cOne, cTwo;
	float dpStaOne, dpStaTwo, dpOneTwo, smOne, smTwo;
	float denom, mua, mub;

	smOne = dOne.squareMag();
	smTwo = dTwo.squareMag();
	dpOneTwo = dTwo.dotAKAscalar(dOne);

	toSt = pOne - pTwo;
	dpStaOne = dOne.dotAKAscalar(toSt);
	dpStaTwo = dTwo.dotAKAscalar(toSt);

	denom = smOne * smTwo - dpOneTwo * dpOneTwo;

	// Zero denominator indicates parrallel lines
	if (abs(denom) < 0.0001f) {
		return useOne ? pOne : pTwo;
	}

	mua = (dpOneTwo * dpStaTwo - smTwo * dpStaOne) / denom;
	mub = (smOne * dpStaTwo - dpOneTwo * dpStaOne) / denom;

	// If either of the edges has the nearest point out
	// of bounds, then the edges aren't crossed, we have
	// an edge-face contact. Our point is on the edge, which
	// we know from the useOne parameter.
	if (mua > oneSize ||
		mua < -oneSize ||
		mub > twoSize ||
		mub < -twoSize)
	{
		return useOne ? pOne : pTwo;
	}
	else
	{
		cOne = pOne + mua * dOne;
		cTwo = pTwo + mub * dTwo;

		return cOne * 0.5 + cTwo * 0.5;
	}
}


//half size is Dimensions / 2.f * obbScale

//it's 0.5 * scale
//it's max values * scale
//dimensions is something calculated once
//0.5 is something known for unit cube
//max values can be calculated or stored once