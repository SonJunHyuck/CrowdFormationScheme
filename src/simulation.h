#pragma once

#include "common.h"

CLASS_PTR(Simulation)
class Simulation
{
private:
	Simulation();

	bool Init(const uint8_t& InNumGroups, const std::vector<uint32_t>& InNumAgents);

public:
	static SimulationUPtr Create(const uint8_t& InNumGroups, const std::vector<uint32_t>& InNumAgents);\
	~Simulation();

	void SetFormation(const Formation_t& InFormation, const uint8_t& InGroupId, const glm::vec3& InRotateAxis, const float& InScale);
	void DrawPath(const glm::vec3& Waypoint);

private:
	// NumGroups -> Group 1 Agents -> Group 2 Agents, ...m Group N Agents; -> NumAgents
	uint8_t NumGroups;  // MUST : input
	std::vector<class Group> Groups;

	uint32_t NumAgents;
	std::vector<class Agent> Agents;
	
	// Object, UPtr can't pre-declare (Only ptr because of runtime issue)
	std::unique_ptr<class Grid> GridField;

	float CollisionConstraintStiffness;

	void CalcStiffness(int n);
	void InitAgentDelta();
	void UpdatePredictedPosition();

private:
	// Pixed Scheme (Template Pattern)
	void PathFinding();
	void CalcPredictedPosition();
	void UpdateLocalInformation();
	void TriggerAvoidanceConstraint();
	void TriggerCollisionConstraint();
	void TriggerSRDConstraint();
	void UpdateFinalPosition();

public:
	// Simulation Parameters
	uint64_t StepNo { 1 };

	uint8_t IterateCount { 2 };
	bool bIsAvoidance = true;
	bool bIsFormation = true;
	//bool bIsFTL = false;
	bool bIsSteering = true;
	bool bIsCohesion = true;
	bool bIsSimulate = false;
	bool bIsDrawMode = false;

	uint8_t DrawPathGroupId { 0 };

public:
	void Update();
};