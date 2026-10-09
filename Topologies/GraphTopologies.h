#pragma once

#include <cstdint>
#include <vector>
#include <QJsonObject>

enum NodeType : int8_t {
    NT_Node = 0,
    NT_Router = 1,
    NT_Crossbar = 2,
};

class Graph {
public:

    explicit Graph(uint16_t num_nodes);

    virtual ~Graph() = default;

    const std::vector<uint16_t> &GetNeighbors(uint16_t node) const;

    uint16_t GetNumVertices() const;

    bool IsConnected() const;

    std::vector<std::vector<uint16_t>> GetAdjacencyMatrix() const;

    const std::vector<std::vector<uint16_t>> &GetEdges() const;

    const std::vector<NodeType> &GetNodeTypes() const;

    bool SetNodeTypes(std::vector<NodeType> &&node_types);

    bool SetEdges(std::vector<std::vector<uint16_t>> &&edges);

    virtual QJsonObject toJsonExtra() const;
protected:
    std::vector<NodeType> nodes;

    void AddEdge(uint16_t node1, uint16_t node2, bool is_directed = false);

    void AddRouters(uint16_t count);

    void AddCrossbars(uint16_t count);

    std::vector<std::vector<uint16_t>> adjacency_list;
    uint16_t num_vertices;
};

class Mesh : public Graph {
public:

    explicit Mesh(uint16_t rows, uint16_t cols);

    uint16_t GetNumRows() const;

    uint16_t GetNumCols() const;

    QJsonObject toJsonExtra() const override;

protected:
    uint16_t num_rows;
    uint16_t num_cols;
};

class TMesh : public Mesh {
public:

    explicit TMesh(uint16_t rows, uint16_t cols);
};

class DMesh : public Mesh {
public:

    explicit DMesh(uint16_t rows, uint16_t cols);
};

class DMeshDoubleLinked : public Mesh {
public:

    explicit DMeshDoubleLinked(uint16_t rows, uint16_t cols);
};

class ZMesh : public Mesh {
public:

    explicit ZMesh(uint16_t rows, uint16_t cols);
};

class DCM : public Mesh {
public:

    explicit DCM(uint16_t rows, uint16_t cols);
};

class MCC : public Mesh {
public:

    explicit MCC(uint16_t rows, uint16_t cols);
};

class XNetwork : public Mesh {
public:

    explicit XNetwork(uint16_t rows, uint16_t cols);
};

class CBPMesh : public Mesh {
public:

    explicit CBPMesh(uint16_t rows, uint16_t cols);
};

class StarMesh : public Mesh {
public:

    explicit StarMesh(uint16_t rows, uint16_t cols);
};

class L2Star : public Mesh {
public:

    explicit L2Star(uint16_t rows, uint16_t cols);
};

class Torus : public Mesh {
public:

    explicit Torus(uint16_t rows, uint16_t cols);
};

class Ring : public Graph {
public:

    explicit Ring(uint16_t num_nodes);
};

class Spidergon : public Ring {
public:
    explicit Spidergon(uint16_t num_nodes);
};

class Circulant : public Graph {
public:
    explicit Circulant(uint16_t num_nodes, const std::vector<uint16_t> &generators);

    explicit Circulant(uint16_t num_nodes, std::vector<uint16_t> &&generators);

    const std::vector<uint16_t> &GetGenx() const;

    QJsonObject toJsonExtra() const override;

protected:
    uint8_t k;
    std::vector<uint16_t> genx;

private:
    void Init();
};

std::vector<uint16_t> GetPaleyGenx(uint16_t num_vertices, uint16_t degree);

std::vector<uint16_t> GetMultiplicativeCirculantGenx(uint16_t base, uint16_t degree);

class HyperCirculant : public Graph {
public:
    explicit HyperCirculant(uint16_t num_nodes, const std::vector<std::vector<uint16_t>> &generators);

    explicit HyperCirculant(uint16_t num_nodes, std::vector<std::vector<uint16_t>> &&generators);

    const std::vector<std::vector<uint16_t>> &GetGenx() const;

    QJsonObject toJsonExtra() const override;

private:
    void Init();

    uint16_t k;
    std::vector<std::vector<uint16_t>> genx;
};

class TreeBase : public Graph {
public:
    TreeBase(uint16_t num_nodes, uint16_t levels);

    uint16_t GetNumLevels() const;

    const std::vector<std::vector<uint16_t>> &GetLevels() const;

    virtual void Build() = 0;

    QJsonObject toJsonExtra() const override;

    void SetTreeLevels(std::vector<std::vector<uint16_t>> &&new_tree_levels);

protected:
    std::vector<std::vector<uint16_t>> tree_levels;

    uint16_t levels;
};

class FatTree : public TreeBase
{
public:
    FatTree(uint16_t concentration, uint16_t levels);

    void Build() override;

    QJsonObject toJsonExtra() const override;

private:
    uint16_t conc;
};

class XTree : public FatTree
{
public:
    XTree(uint16_t levels);

    void Build() override;
};
