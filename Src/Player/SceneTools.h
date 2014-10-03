#pragma once

#include <Core/Core.h>
#include "Settings.h"

void AddChildToParent(Pimp::Node* nodeChild, Pimp::Node* nodeParent);

void RemoveNodeFromWorld(Pimp::Node* node);

void DuplicateTransformTransformedHierarchy(
	Pimp::World* w,
	Pimp::Node* source, Pimp::Node* dest);

void InitMaterialCompilationSystem();
void SetNumTotalMaterialCompilationJobs(int count);
void StartMaterialCompilationJob(const unsigned char* effectAscii, int effectAsciiSize, unsigned char** outCompiledEffectBuffer, int* compiledEffectLength);
void WaitForMaterialCompilationJobsToFinish();

void DrawLoadProgress(bool diskResourcesLoaded);

Pimp::AnimCurve* DuplicateAnimCurve(Pimp::World* world, Pimp::AnimCurve* curve, float delay);
