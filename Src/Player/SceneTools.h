#pragma once

#include <Core/Core.h>
#include "Settings.h"

void AddChildToParent(Pimp::Node* nodeChild, Pimp::Node* nodeParent);

//void GenerateAnimCurveForSoundtrackEvents(
//	Pandora::System* soundSystem,
//	Pimp::AnimCurve* curve, 
//	int channel, float attackTimeSeconds, float decayTimeSeconds, 
//	float outValueMin, float outValueMax, float predelay = 0.0f, bool incremental = false, int eventType = 0, int noteMin = 0, int noteMax = 127);


void RemoveNodeFromWorld(Pimp::Node* node);

void DuplicateTransformTransformedHierarchy(
	Pimp::World* w,
	Pimp::Node* source, Pimp::Node* dest);


void InitMaterialCompilationSystem();
void SetNumTotalMaterialCompilationJobs(int count);
void StartMaterialCompilationJob(const unsigned char* effectAscii, int effectAsciiSize, unsigned char** outCompiledEffectBuffer, int* compiledEffectLength);
void WaitForMaterialCompilationJobsToFinish();

void DrawLoadProgress(bool texturesLoaded);
