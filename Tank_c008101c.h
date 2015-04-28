#pragma once
#ifndef TANK_C008101C_H
#define TANK_C008101C_H

#include "BaseTank.h"
#include <SDL.h>
#include <algorithm>
#include <vector>
#include <map>
#include "Commons.h"

class Waypoint;

//---------------------------------------------------------------
struct EdgeCost  // Holds the cost of moving along an edge
{
	Waypoint* waypointFrom;
	Waypoint* waypointTo;
	double cost;
	EdgeCost(Waypoint* from, Waypoint* to, double newCost): waypointFrom(from), waypointTo(to), cost(newCost) {};
	~EdgeCost() { waypointFrom = nullptr; waypointTo = nullptr; };
};

struct Node // Holds the cost of moving to a waypoint, wraps the waypoint
{
	Waypoint* waypoint;
	Node* parent;
	double cost;
	Node(Waypoint* current, Node* parent, double newCost) : waypoint(current), parent(parent), cost(newCost) {};
	~Node() { waypoint = nullptr; parent = nullptr; };
};

class PathFinder
{
public:
	PathFinder() { SetEdgeCosts(); };
	~PathFinder();
	vector<Waypoint*> FindPath(Vector2D startPosition, Vector2D targetPosition);
private:
	vector<EdgeCost*> edgeCosts;
	vector<Node*> openNodes;
	vector<Node*> closedNodes;
	bool IsInList(vector<Node*> nodes, int waypointID);
	bool IsInList(vector<Node*> nodes, const Waypoint* const waypoint);
	double GetCostFromWaypoints(const Waypoint* const from, const Waypoint* const to);
	double GetHeuristictCost(const Vector2D& positionOne, const Vector2D& positionTwo);
	vector<Waypoint*> BuildPath(const Node* targetNode, const Vector2D& targetPosition);
	void SetEdgeCosts();
	Waypoint* GetNearestWaypoint(const Vector2D& position);
};

//---------------------------------------------------------------
enum State { patrol, pickups, attack, partesian, sudoku, _min=patrol, _max=sudoku };
enum Deceleration { sonic = 1, human = 2, snail = 3 };

//---------------------------------------------------------------
class Tank_c008101c;
typedef void (Tank_c008101c::* StateFuncPtr)();
//---------------------------------------------------------------
class FiniteStateMachine
{
public:
	FiniteStateMachine(Tank_c008101c* tank): obj(tank) {};
	~FiniteStateMachine() { obj = nullptr; enterFunctions.clear(); exitFunctions.clear(); };
	State GetState() { return currentState; };
	void ChangeState(State newState)
	{
		if (newState < State::_min || newState > State::_max)
		{
			std::cout << "New state out of bounds: " << newState << std::endl;
			return;
		}
		if (enterFunctions.empty() || exitFunctions.empty())  // || StateFuncPtrtions.empty())
		{
			std::cout << "Function maps empty." << std::endl;
			return;
		}
		auto it = exitFunctions.find(currentState);
		if (it != exitFunctions.end() && it->second != nullptr)
		{
			(obj->*(it->second))();
		}
		it = enterFunctions.find(newState);
		if (it != enterFunctions.end() && it->second != nullptr)
		{
			(obj->*(it->second))();
		}
		currentState = newState;
	};
	void SetEnterMap(map<State, StateFuncPtr> functionMap) { enterFunctions = functionMap; }
	void SetExitMap(map<State, StateFuncPtr> functionMap) { exitFunctions = functionMap; }
private:
	Tank_c008101c* obj = nullptr;
	State currentState;
	map<State, StateFuncPtr> enterFunctions;
	map<State, StateFuncPtr> exitFunctions;
};

//---------------------------------------------------------------
class Tank_c008101c :
	protected BaseTank
{
public:
	Tank_c008101c(SDL_Renderer* renderer, TankSetupDetails details);
	~Tank_c008101c();
	void CheckInput(SDL_Event e);

	void Render();
	void Update(float deltaTime, SDL_Event e);

private:
	bool mNewTarget = false;
	bool mMoveToClicked = false;
	unsigned mTargetIndex = 0;
	double mMaxSeeAhead = 80;
	BaseTank* mTargetTank = nullptr;
	Deceleration mDeceleration = human;
	PathFinder mPathFinder;
	FiniteStateMachine mFSM = FiniteStateMachine(this);
	STEERING_BEHAVIOUR mBehaviour = STEERING_INTRPOSE;
	Texture2D* mAheadTex = nullptr;
	Texture2D* mAhead2Tex = nullptr;
	Texture2D* mWaypointTex = nullptr;
	Vector2D mSteeringForce;
	Vector2D mTargetPosition;
	Vector2D mTexCenter;
	Vector2D mAhead;
	Vector2D mAhead2;
	vector<Waypoint*> mPath;
	Waypoint* mTargetWaypoint = nullptr;

	GameObject* MostThreateningObstacle();
	Vector2D Arrive(Vector2D targetPosition);
	Vector2D Seek(Vector2D targetPosition);
	Vector2D Flee(Vector2D targetPosition);
	Vector2D ObstacleAvoidance();
	Vector2D GetNearestPickup();
	Vector2D PathFinding(Vector2D targetPosition);
	void AttackTank();
	void Immolate();
	void MoveInHeadingDirection(float deltaTime);
	void PartesianRetreat();
	void Patrol();
	void Pickups();
	void SetStateBehaviours();
};

//---------------------------------------------------------------
#endif //TANK_C008101C_H

