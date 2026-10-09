#pragma once

#include "ALTypes.h"
#include "GraphTopologies.h"

VertexLayout MakeGrid(const Mesh &graph, PictureParameters pic);

VertexLayout MakeCircle(const Graph &graph, PictureParameters pic);

VertexLayout MakeMCCSubgrid(const MCC &graph, const VertexLayout &main_grid);

VertexLayout MakeXNSubgrid(const XNetwork &graph, const VertexLayout &main_grid);

VertexLayout MakeTree(const TreeBase &graph, PictureParameters pic);

void Rescale(VertexLayout &layout, PictureParameters old_pic, PictureParameters new_pic);