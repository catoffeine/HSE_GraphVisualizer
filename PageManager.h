#pragma once

#include <QObject>
#include <QImage>
#include <QQuickWindow>
#include <QFileDialog>
#include <QWidget>
#include <QQuickView>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonDocument>
#include <QSvgGenerator>
#include <QPointer>
#include <QtConcurrent>

#include "CanvasWidget.h"
#include "ALVertex.h"
#include "Algorithms.h"
#include "AutoDetection.h"


struct Settings {
    bool show_label_text = false;
    bool dark_mode = true;
    bool is_outline_mode = false;
    bool const_font_size = false;

    qreal nodes_radius = 6;
    qreal edges_width = 2;

    uint8_t font_size = 15;
};

class PageManager : public QObject {
Q_OBJECT

    QQuickView *current_view{};

    QString data_type;

    Graph *graph{};

    QString topology_type;

    QPointer<CanvasWidget> canvas_widget;
    QPointer<QTimer> timer;

    PictureParameters pic_params;
    VertexLayout layout;
    std::vector<EdgeGroup> edges;

    int16_t user_groups_count;

public:
    explicit PageManager(QObject *parent = nullptr) : QObject(parent) {}

    void Initialize();

    [[nodiscard]] Settings GetSettings() const;

    Q_INVOKABLE bool ValidateInput(const QString &);

    Q_INVOKABLE bool ValidateFile(QString);

    Q_INVOKABLE void CreateMeetPage(bool);

    [[nodiscard]] VertexLayout *GetNodes();

    [[nodiscard]] std::vector<EdgeGroup> *GetEdges();

    Q_INVOKABLE QList<QString> GetTopologiesList();

    Q_INVOKABLE QList<QString> GetGroupNames();

    Q_INVOKABLE QList<int> GetSelectedEdgeVertices();

    Q_INVOKABLE QString GetSelectedEdgeGroupName();

    Q_INVOKABLE void DeselectEdges();

    Q_INVOKABLE void DeselectVertices();

    Q_INVOKABLE void AddGroup(int vertex_from, int vertex_to, const QString& group_to_add, QColor color, bool use_as_group);

    Q_INVOKABLE void AddGroupWithParameters(int vertex_from, int vertex_to, const QString& group_to_add, const QVariantMap& params, bool use_as_group);

    Q_INVOKABLE void AddToGroup(int vertex_from, int vertex_to, int target_idx, bool use_as_group);

    Q_INVOKABLE void ChangeColor(int vertex_from, int vertex_to, QColor color, bool use_as_group);

    Q_INVOKABLE void ChangeVertexColor(QColor color);

    Q_INVOKABLE void ChangeParameters(int vertex_from, int vertex_to, const QVariantMap& params, bool use_as_group);

    Q_INVOKABLE void SetInputDataFromTopology(const QString &new_data_type, const QString &data_value);

    Q_INVOKABLE void SetMainPageProperties() const;

    Q_INVOKABLE void SetInputAreaPageProperties() const;

    Q_INVOKABLE void SetMeetPageProperties() const;

    Q_INVOKABLE void ChangeEdgesWidth(qreal new_width);

    Q_INVOKABLE void ChangeNodesRadius(qreal new_radius);

    Q_INVOKABLE void ChangeNodesFontSize(qint8 new_font_size);

    Q_INVOKABLE void ResetPositions();

    Q_INVOKABLE void ToggleDarkMode();

    Q_INVOKABLE void ToggleShowLabelText();

    Q_INVOKABLE void ToggleConstTextFontSize();

    Q_INVOKABLE void ToggleOutlineMode();

    Q_INVOKABLE bool CheckVerticesExist(const QString &vertex_from, const QString &vertex_to);

    Q_INVOKABLE void DrawShortestPath(int vertex_from, int vertex_to, const QString &color);

    Q_INVOKABLE void SetCanvasWidgetItem(QQuickItem *canvas);

    Q_INVOKABLE void StartCanvasTimer();

    Q_INVOKABLE QColor GetColor(QColor initial_color);

    Q_INVOKABLE void SaveCanvas(bool is_vector = false) {
        TempCanvas canvas;
        if (!is_vector) {
            QImage image = canvas.DrawCanvas();

            QString filePath = QFileDialog::getSaveFileName(nullptr, "Save Image", "", "Images (*.png *.jpg)");
            if (!filePath.isEmpty()) {
                if (!image.save(filePath)) {
                    qWarning() << "Failed to save image!";
                }
            }
        } else {
            QSvgGenerator generator;
            QString filePath = QFileDialog::getSaveFileName(nullptr, "Save Image", "", "Images (*.svg)");
            if (!filePath.isEmpty()) {
                generator.setFileName(filePath);
            }
            canvas.DrawCanvas(&generator);
        }
    }

    Q_INVOKABLE void ExcludeEdgeFromGroup(int group_idx, int edge_idx, const std::string &new_group_name = "") {
        if (group_idx < 0 || edge_idx < 0 || group_idx >= static_cast<int>(edges.size()) ||
            edge_idx >= static_cast<int>(edges[group_idx].edges.size()))
            return;

        auto edge = edges[group_idx].edges[edge_idx];
        edges[group_idx].edges.erase(edges[group_idx].edges.begin() + edge_idx);

        edges.emplace_back();
        auto &new_group = edges.back();
        new_group.edges.push_back(edge);
        new_group.type = edges[group_idx].type;
        new_group.edge_type_data = edges[group_idx].edge_type_data;
        new_group.is_default_color = edges[group_idx].is_default_color;
        new_group.color = edges[group_idx].color;
        if (new_group_name.empty())
            new_group.name = "user_group_" + std::to_string(user_groups_count++);
        else
            new_group.name = new_group_name;

        if (edges[group_idx].edges.empty())
            edges.erase(edges.begin() + group_idx);

        canvas_widget->draw_points_set = false;
    }

    Q_INVOKABLE void MergeEdgeGroups(int group_dest_idx, int group_src_idx) {
        if (group_dest_idx < 0 || group_src_idx < 0 || group_dest_idx == group_src_idx ||
            group_dest_idx >= static_cast<int>(edges.size()) || group_src_idx >= static_cast<int>(edges.size()))
            return;

        edges[group_dest_idx].edges.insert(edges[group_dest_idx].edges.end(),
                                           std::make_move_iterator(edges[group_src_idx].edges.begin()),
                                           std::make_move_iterator(edges[group_src_idx].edges.end()));
        edges.erase(edges.begin() + group_src_idx);

        canvas_widget->draw_points_set = false;
    }

    Q_INVOKABLE QPair<int, int> FindEdge(uint16_t from, uint16_t to) {
        for (size_t i = 0; i < edges.size(); ++i) {
            for (size_t j = 0; j < edges[i].edges.size(); ++j) {
                const auto &edge = edges[i].edges[j];
                if ((edge.from == from && edge.to == to) ||
                    (edge.from == to && edge.to == from && !edge.is_directed))
                {
                    return {static_cast<int>(i), static_cast<int>(j)};
                }
            }
        }
        return {-1, -1};
    }

    Q_INVOKABLE void toJson() const {
        QJsonObject obj;

        obj["topology_type"] = topology_type;

        if (graph) {
            obj["num_vertices"] = graph->GetNumVertices();

            QJsonObject extra = graph->toJsonExtra();
            for (auto it = extra.begin(); it != extra.end(); ++it) {
                obj[it.key()] = it.value();
            }

            QJsonArray adjList;
            for (const auto& row : graph->GetEdges()) {
                QJsonArray rowArray;
                for (uint16_t v : row)
                    rowArray.append(int(v));
                adjList.append(rowArray);
            }
            obj["adjacency_list"] = adjList;
        }

        QJsonArray layoutArray;
        for (const Coordinate& c : layout) {
            QJsonObject coordObj;
            coordObj["x"] = c.x;
            coordObj["y"] = c.y;
            coordObj["type"] = int(c.type);
            coordObj["is_default_color"] = c.is_default_color;
            coordObj["color"] = c.color.name();
            layoutArray.append(coordObj);
        }
        obj["layout"] = layoutArray;

        QJsonArray edgesArray;
        for (const EdgeGroup& group : edges) {
            QJsonObject groupObj;
            groupObj["type"] = int(group.type);
            groupObj["is_default_color"] = group.is_default_color;
            groupObj["color"] = group.color.name();
            groupObj["name"] = group.name.data();

            QJsonArray edgesJson;
            for (const Edge& e : group.edges) {
                QJsonObject eObj;
                eObj["from"] = e.from;
                eObj["to"] = e.to;
                eObj["is_directed"] = e.is_directed;
                edgesJson.append(eObj);
            }
            groupObj["edges"] = edgesJson;

            QJsonObject dataObj;
            switch (group.type) {
                case EdgeType::ET_Arc: {
                    auto arc = std::get<ArcData>(group.edge_type_data);
                    dataObj["ratio"] = arc.ratio;
                    break;
                }
                case EdgeType::ET_BLine: {
                    auto bline = std::get<BLineData>(group.edge_type_data);
                    dataObj["segments_number"] = bline.segments_number;
                    QJsonArray segs;
                    for (const auto& seg : bline.ratio_segment) {
                        QJsonObject s;
                        s["along"] = seg.along;
                        s["across"] = seg.across;
                        segs.append(s);
                    }
                    dataObj["ratio_segment"] = segs;
                    break;
                }
                case EdgeType::ET_BCurve: {
                    auto bcurve = std::get<BCurveData>(group.edge_type_data);
                    dataObj["along1"] = bcurve.along1;
                    dataObj["across1"] = bcurve.across1;
                    dataObj["along2"] = bcurve.along2;
                    dataObj["across2"] = bcurve.across2;
                    break;
                }
                default:
                    break;
            }

            groupObj["edge_type_data"] = dataObj;
            edgesArray.append(groupObj);
        }
        obj["edges"] = edgesArray;

        QJsonDocument jsonDoc(obj);

        QString filePath = QFileDialog::getSaveFileName(nullptr, "Save Image", "", "(*.json)");
        if (!filePath.isEmpty()) {
            if (!filePath.endsWith(".json"))
                filePath.append(".json");
            QFile file(filePath);
            if (!file.open(QIODevice::ReadWrite)) {
                qWarning("Failed to save file!");
                return;
            }
            file.write(jsonDoc.toJson(QJsonDocument::Indented));
            file.close();
        }
    }

    Q_INVOKABLE void toMatrix() const {
        if (!graph) return;

        QString filePath = QFileDialog::getSaveFileName(nullptr, "Save Matrix", "", "Adjacency Matrix (*.txt)");
        if (filePath.isEmpty())
            return;

        if (!filePath.endsWith(".txt"))
            filePath.append(".txt");

        QFile file(filePath);
        if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
            qWarning("Failed to open file for writing.");
            return;
        }

        QTextStream out(&file);

        const auto& matrix = graph->GetAdjacencyMatrix();
        for (const std::vector<uint16_t>& row : matrix) {
            for (size_t i = 0; i < row.size(); ++i) {
                out << row[i];
                if (i != row.size() - 1)
                    out << " ";
            }
            out << "\n";
        }

        file.close();
    }

    Q_INVOKABLE void SetInputDataFromJson(const QString& type, QString path) {
        Q_UNUSED(type);
        path = path.replace(0, 8, "");
        QFile file(path);
        if (!file.open(QIODevice::ReadOnly)) return;

        QJsonDocument doc = QJsonDocument::fromJson(file.readAll());
        QJsonObject obj = doc.object();
        topology_type = obj["topology_type"].toString();

        auto num_vertices = uint16_t(obj["num_vertices"].toInt());
        auto rows = uint16_t(obj["num_rows"].toInt(0));
        auto cols = uint16_t(obj["num_cols"].toInt(0));
        std::vector<uint16_t> circulant_generators;
        for (auto g : obj["generators"].toArray())
            circulant_generators.push_back(g.toInt());

        std::vector<std::vector<uint16_t>> hyper_generators;
        for (const auto& val : obj["hyper_generators"].toArray()) {
            std::vector<uint16_t> row;
            for (const auto& v : val.toArray())
                row.push_back(uint16_t(v.toInt()));
            hyper_generators.push_back(row);
        }

        uint16_t levels = uint16_t(obj["levels"].toInt(0));
        uint16_t conc = uint16_t(obj["concentration"].toInt(0));


        if (topology_type == "Circulant") {
            graph = new Circulant(num_vertices, circulant_generators);
        } else if (topology_type == "Paley") {
            graph = new Circulant(num_vertices, circulant_generators);
        } else if (topology_type == "Multiplicative Circulant") {
            graph = new Circulant(num_vertices, circulant_generators);
        } else if (topology_type == "HyperCirculant") {
            graph = new HyperCirculant(num_vertices, hyper_generators);
        } else if (topology_type == "Mesh") {
            graph = new Mesh(rows, cols);
        } else if (topology_type == "Spidergon") {
            graph = new Spidergon(num_vertices);
        } else if (topology_type == "Ring") {
            graph = new Ring(num_vertices);
        } else if (topology_type == "Torus") {
            graph = new Torus(rows, cols);
        } else if (topology_type == "FatTree") {
            graph = new FatTree(conc, levels);
        } else if (topology_type == "XTree") {
            graph = new XTree(levels);
        } else if (topology_type == "TMesh") {
            graph = new TMesh(rows, cols);
        } else if (topology_type == "DMesh") {
            graph = new DMesh(rows, cols);
        } else if (topology_type == "DMeshDoubleLinked") {
            graph = new DMeshDoubleLinked(rows, cols);
        } else if (topology_type == "ZMesh") {
            graph = new ZMesh(rows, cols);
        } else if (topology_type == "DCM") {
            graph = new DCM(rows, cols);
        } else if (topology_type == "MCC") {
            graph = new MCC(rows, cols);
        } else if (topology_type == "XNetwork") {
            graph = new XNetwork(rows, cols);
        } else if (topology_type == "CBPMesh") {
            graph = new CBPMesh(rows, cols);
        } else if (topology_type == "StarMesh") {
            graph = new StarMesh(rows, cols);
        } else if (topology_type == "L2Star") {
            graph = new L2Star(rows, cols);
        } else {
            return;
        }

        std::vector<std::vector<uint16_t>> adjacency_list;
        for (const auto& val : obj["adjacency_list"].toArray()) {
            std::vector<uint16_t> row;
            for (const auto& v : val.toArray())
                row.push_back(uint16_t(v.toInt()));
            adjacency_list.push_back(row);
        }

        graph->SetEdges(std::move(adjacency_list));

        layout.clear();
        for (const auto& val : obj["layout"].toArray()) {
            QJsonObject c = val.toObject();
            layout.push_back({
                                     uint16_t(c["x"].toInt()),
                                     uint16_t(c["y"].toInt()),
                                     NodeType(c["type"].toInt()),
                                     false,
                                     c["is_default_color"].toBool(),
                                     QColor(c["color"].toString())
                             });
        }

        edges.clear();
        for (const auto& val : obj["edges"].toArray()) {
            QJsonObject g = val.toObject();
            EdgeGroup group;
            group.type = EdgeType(g["type"].toInt());
            group.is_default_color = g["is_default_color"].toBool();
            group.color = QColor(g["color"].toString());
            group.name = g["name"].toString().toStdString();

            for (const auto& e : g["edges"].toArray()) {
                QJsonObject eObj = e.toObject();
                group.edges.push_back({
                                              uint16_t(eObj["from"].toInt()),
                                              uint16_t(eObj["to"].toInt()),
                                              eObj["is_directed"].toBool()
                                      });
            }

            QJsonObject data = g["edge_type_data"].toObject();
            switch (group.type) {
                case EdgeType::ET_Arc:
                    group.edge_type_data = ArcData{float(data["ratio"].toDouble())};
                    break;
                case EdgeType::ET_BLine: {
                    BLineData b;
                    b.segments_number = uint8_t(data["segments_number"].toInt());
                    for (const auto& s : data["ratio_segment"].toArray()) {
                        QJsonObject so = s.toObject();
                        b.ratio_segment.push_back({float(so["along"].toDouble()), float(so["across"].toDouble())});
                    }
                    group.edge_type_data = b;
                    break;
                }
                case EdgeType::ET_BCurve:
                    group.edge_type_data = BCurveData{
                            data["along1"].toDouble(),
                            data["across1"].toDouble(),
                            data["along2"].toDouble(),
                            data["across2"].toDouble()
                    };
                    break;
                default:
                    group.edge_type_data = std::monostate{};
                    break;
            }

            edges.push_back(group);
        }
    }

    Q_INVOKABLE void SetInputDataFromAdjacencyMatrixFile(const QString& type, QString path) {
        this->data_type = type;
        delete graph;

        layout.clear();
        edges.clear();

        path = path.replace(0, 8, "");
        QFile file(path);
        if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
            return;
        }
        QTextStream in(&file);
        std::vector<std::vector<uint16_t>> matrix;
        int expectedColumnCount = -1;
        int rowNumber = 0;

        while (!in.atEnd()) {
            QString line = in.readLine().trimmed();
            if (line.isEmpty()) continue;
            QStringList parts = line.split(QRegularExpression("[,\\s]+"), Qt::SkipEmptyParts);
            std::vector<uint16_t> row;
            for (const QString& part : parts) {
                bool ok = false;
                uint16_t value = part.toInt(&ok);
                if (!ok || value < 0) {
                    qWarning() << "Incorrect value in row " << rowNumber << ":" << part;
                    return;
                }
                row.push_back(value);
            }
            if (expectedColumnCount == -1) {
                expectedColumnCount = row.size();
            } else if (row.size() != expectedColumnCount) {
                qWarning() << "Incorrect number of columns: " << rowNumber;
                return;
            }
            matrix.push_back(row);
            rowNumber++;
        }
        file.close();

        if (matrix.size() != expectedColumnCount) {
            qWarning() << "Matrix is not correct";
            return;
        }

        std::vector<std::vector<uint16_t>> adjacencyList(matrix.size());
        for (uint16_t i = 0; i < matrix.size(); ++i) {
            for (uint16_t j = 0; j < matrix[i].size(); ++j) {
                if (i != j) {
                    uint16_t edgeCount = matrix[i][j];
                    for (uint16_t k = 0; k < edgeCount; ++k) {
                        adjacencyList[i].push_back(j);
                    }
                }
            }
        }

        DetectedTopology topology = detectTopology(adjacencyList);
        if (topology.first == "Other") {
            return;
        }
        topology_type = QString::fromStdString(topology.first);
        if (topology_type == "Mesh") {
            graph = new Mesh(topology.second[0], topology.second[1]);
        }
        else if (topology_type == "Spidergon") {
            graph = new Spidergon(topology.second[0]);
        }
        else if (topology_type == "Ring") {
            graph = new Ring(topology.second[0]);
        }
        else if (topology_type == "Torus") {
            graph = new Torus(topology.second[0], topology.second[1]);
        }
        else if (topology_type == "FatTree") {
            auto ft = new FatTree(topology.second[0], topology.second[1]);
            ft->Build();
            graph = ft;
        }
        else if (topology_type == "XTree") {
            auto xt = new XTree(topology.second[0]);
            xt->Build();
            graph = xt;
        }
        else if (topology_type == "TMesh") {
            graph = new TMesh(topology.second[0], topology.second[1]);
        }
        else if (topology_type == "DMesh") {
            graph = new DMesh(topology.second[0], topology.second[1]);
        }
        else if (topology_type == "DMeshDoubleLinked") {
            graph = new DMeshDoubleLinked(topology.second[0], topology.second[1]);
        }
        else if (topology_type == "ZMesh") {
            graph = new ZMesh(topology.second[0], topology.second[1]);
        }
        else if (topology_type == "DCM") {
            graph = new DCM(topology.second[0], topology.second[1]);
        }
        else if (topology_type == "MCC") {
            graph = new MCC(topology.second[0], topology.second[1]);
        }
        else if (topology_type == "XNetwork") {
            graph = new XNetwork(topology.second[0], topology.second[1]);
        }
        else if (topology_type == "Circulant") {
            auto nodes = topology.second[0];
            auto gens = topology.second;
            gens.erase(gens.begin());
            graph = new Circulant(nodes, gens);
        }
        else if (topology_type == "StarMesh") {
            graph = new StarMesh(topology.second[0], topology.second[1]);
        }
        else if (topology_type == "L2Star") {
            graph = new L2Star(topology.second[0], topology.second[1]);
        }
    }

    void CenterScreen() const;

    ~PageManager() override;


    Q_INVOKABLE void computeParameters() {
        if (!graph)
            return;

        QFuture<void> future = QtConcurrent::run([this]() {
            GraphParameters params;
            params.distances.reserve(graph->GetNumVertices());

            emit progressUpdated(0, "Calculating graph distances...");
            for (int i = 0; i < graph->GetNumVertices(); i++) {
                params.distances.push_back(BFSDistances(graph->GetEdges(), i));
            }

            emit progressUpdated(40, "Calculating diameter...");
            params.diameter = GraphDiameter(*graph, params.distances);

            emit progressUpdated(50, "Calculating average distance...");
            params.average_distance = GraphAveragePathLength(*graph, params.distances);

            emit progressUpdated(60, "Finding maximum vertex degree...");
            params.max_node_degree = MaxNodeDegree(*graph);

            emit progressUpdated(70, "Calculating network metrics...");
            params.global_network_cost = NetworkCost(*graph, params.distances);

            emit progressUpdated(80, "Calculating network metrics...");
            params.global_packing_density = PackingDensity(*graph, params.distances);

            emit progressUpdated(90, "Calculating network metrics...");
            params.modified_network_cost = ModifiedNetworkCost(*graph, params.distances);

            emit progressUpdated(100, "Calculating network metrics...");
            params.modified_packing_density = ModifiedPackingDensity(*graph, params.distances);
            emit computationFinished(params);
        });
    }

    Q_INVOKABLE bool isDark() const
    {
        return GetSettings().dark_mode;
    }

    Q_INVOKABLE void toBookSim() const
    {
        QString filePath = QFileDialog::getSaveFileName(nullptr, "Save File", "", "(*.net)");
        if (!filePath.isEmpty()) {
            if (!filePath.endsWith(".net"))
                filePath.append(".net");
            QFile file(filePath);
            if (!file.open(QIODevice::WriteOnly)) {
                qWarning("Failed to save file!");
                return;
            }

            for (int i = 0; i < graph->GetNumVertices(); i++)
            {
                file.write((std::string("router ") + std::to_string(i) + " ").c_str());
                for (auto x: graph->GetNeighbors(i))
                    file.write((std::string("router ") + std::to_string(x) + " ").c_str());

                if (graph->GetNodeTypes()[i] == NT_Node)
                    file.write((std::string("node ") + std::to_string(i)).c_str());

                file.write("\n");
            }

            file.close();
        }
    }

signals:
    void progressUpdated(int percent, const QString& message);
    void computationFinished(const GraphParameters& result);
    void themeChanged();
    void edgesSelected(int number);
    void verticesSelected(bool selection_status);
    void verticesMoved(bool move_status);

private slots:
    Q_INVOKABLE void UpdateCanvasFrame();
};
