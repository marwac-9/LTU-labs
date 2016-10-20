#pragma once
#include "Object.h"

/*
//fast hash function for pointers
template<typename Tval>
struct MyTemplatePointerHash1 {
	size_t operator()(const Tval* val) const {
		static const size_t shift = (size_t)log2(1 + sizeof(Tval));
		return (size_t)(val) >> shift;
	}
};
*/

struct OverlapPair
{
	Object* ent1;
	Object* ent2;

	OverlapPair(Object* obj1, Object* obj2){ ent1 = obj1; ent2 = obj2; }
};

//std specialization
namespace std
{
	template <>
	class hash<OverlapPair>
	{
	public:
		size_t operator()(const OverlapPair &overlapPair) const{
			//Vector3* pos1 = &(overlapPair.ent1->GetPosition());
			//Vector3* pos2 = &(overlapPair.ent2->GetPosition());
			return overlapPair.ent1->ID ^ overlapPair.ent2->ID;//(pos1->a^pos1->b^pos1->c) + (pos2->a^pos2->b^pos2->c);
			//return overlapPair.ent1->ID + overlapPair.ent2->ID;
		}
	};

	template <>
	class equal_to<OverlapPair>
	{
	public:
		bool operator()(const OverlapPair& lhs, const OverlapPair& rhs) const
		{
			return (lhs.ent1 == rhs.ent1 && lhs.ent2 == rhs.ent2 ||
				lhs.ent1 == rhs.ent2 && lhs.ent2 == rhs.ent1);
		}
	};
}

//other way
/*
class OverlapPair_hash
{
public:
	size_t operator()(const OverlapPair &overlapPair) const{
		return overlapPair.ent1->ID + overlapPair.ent2->ID;
	}
};

class OverlapPair_equal_to
{
public:
	bool operator()(const OverlapPair& lhs, const OverlapPair& rhs) const
	{
		return (lhs.ent1 == rhs.ent1 && lhs.ent2 == rhs.ent2 ||
			lhs.ent1 == rhs.ent2 && lhs.ent2 == rhs.ent1);
	}
};

unordered_set<OverlapPair, OverlapPair_hash, OverlapPair_equal_to> fullOverlap;
*/