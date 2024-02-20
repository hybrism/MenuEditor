#pragma once


enum class eCollisionLayer
{
	Player = 0b1,
	Enemy = 0b10,
	Environment = 0b100,
	Everything = 0xffff

};

//enum class eCollisionForm
//{
//	Box,
//	Sphere,
//
//	Size
//};

enum class eCollisionType
{
	Collider,
	Trigger,

	Size
};


struct CollisionData
{
	eCollisionType type = eCollisionType::Size;
	eCollisionLayer layer = eCollisionLayer::Everything;

	void* myData;

	bool isColliding = false;
	bool isPrevFrameColliding = false;

};