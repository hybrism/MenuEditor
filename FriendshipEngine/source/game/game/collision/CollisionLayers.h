#pragma once
enum class eCollisionLayer
{
	Environment		= 0b000001,
	Player			= 0b000010,
	PlayerAttack	= 0b000100,
	Enemy			= 0b001000,
	EnemyAttack		= 0b010000,
	Event			= 0b100000,
	Everything		= 0xffff

};
