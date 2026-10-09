#pragma once

#include <cstdint>
#include <vector>
#include <string>
#include <map>

using DetectedTopology = std::pair<std::string, std::vector<uint16_t>>;


DetectedTopology isMesh(const std::vector<std::vector<uint16_t>>& adj_list);
DetectedTopology isSpidergon(const std::vector<std::vector<uint16_t>>& adj_list);
DetectedTopology isRing(const std::vector<std::vector<uint16_t>>& adj_list);
DetectedTopology isTorus(const std::vector<std::vector<uint16_t>>& adj_list);
DetectedTopology isFatTree(const std::vector<std::vector<uint16_t>>& adj_list, std::map<uint16_t, uint16_t>& counter);
DetectedTopology isXTree(const std::vector<std::vector<uint16_t>>& adj_list);
DetectedTopology isTMesh(const std::vector<std::vector<uint16_t>>& adj_list);
DetectedTopology isDMesh(const std::vector<std::vector<uint16_t>>& adj_list);
DetectedTopology isDMeshDoubleLinked(const std::vector<std::vector<uint16_t>>& adj_list);
DetectedTopology isZMesh(const std::vector<std::vector<uint16_t>>& adj_list);
DetectedTopology isDCM(const std::vector<std::vector<uint16_t>>& adj_list);
DetectedTopology isMCC(const std::vector<std::vector<uint16_t>>& adj_list);
DetectedTopology isXNetwork(const std::vector<std::vector<uint16_t>>& adj_list);
DetectedTopology isCirculant(const std::vector<std::vector<uint16_t>>& adj_list);
DetectedTopology isStarMesh(const std::vector<std::vector<uint16_t>>& adj_list, std::map<uint16_t, uint16_t>& counter);

DetectedTopology detectTopology(const std::vector<std::vector<uint16_t>>& adj_list);
