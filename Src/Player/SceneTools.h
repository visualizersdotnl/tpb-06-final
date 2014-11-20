
#pragma once

// #include <Core/Core.h>

void AddChildToParent(Pimp::Node* nodeChild, Pimp::Node* nodeParent);
void RemoveNodeFromWorld(Pimp::Node* node);
void DuplicateTransformTransformedHierarchy(Pimp::World* world, Pimp::Node* source, Pimp::Node* dest);

Pimp::AnimCurve* DuplicateAnimCurve(Pimp::World* world, Pimp::AnimCurve* curve, float delay);

void DrawLoadProgress(Pimp::World &world, float progress);
