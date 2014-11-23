
#pragma once

// #include <Core/Core.h>

void AddChildToParent(Pimp::Node* nodeChild, Pimp::Node* nodeParent);
void RemoveNodeFromWorld(Pimp::Node* node);
void DuplicateTransformTransformedHierarchy(Pimp::World* world, Pimp::Node* from, Pimp::Node* to);

void DrawLoadProgress(Pimp::World &world, float progress);
