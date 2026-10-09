#pragma once

#include "ALTypes.h"
#include "GraphTopologies.h"

std::vector<EdgeGroup> MakeEdges(const Mesh &graph);

std::vector<EdgeGroup> MakeEdges(const TMesh &graph);

std::vector<EdgeGroup> MakeEdges(const DMesh &graph);

std::vector<EdgeGroup> MakeEdges(const DMeshDoubleLinked &graph);

std::vector<EdgeGroup> MakeEdges(const ZMesh &graph);

std::vector<EdgeGroup> MakeEdges(const DCM &graph);

std::vector<EdgeGroup> MakeEdges(const MCC &graph);

std::vector<EdgeGroup> MakeEdges(const XNetwork &graph);

std::vector<EdgeGroup> MakeEdges(const CBPMesh &graph);

std::vector<EdgeGroup> MakeEdges(const StarMesh &graph);

std::vector<EdgeGroup> MakeEdges(const L2Star &graph);

std::vector<EdgeGroup> MakeEdges(const Torus &graph);

std::vector<EdgeGroup> MakeEdges(const Ring &graph);

std::vector<EdgeGroup> MakeEdges(const Spidergon &graph);

std::vector<EdgeGroup> MakeEdges(const Circulant &graph);

std::vector<EdgeGroup> MakeEdges(const HyperCirculant &graph);

std::vector<EdgeGroup> MakeEdges(const FatTree &graph);