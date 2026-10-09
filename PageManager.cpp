#include "PageManager.h"

#include <ALEdge.h>
#include <globals.h>
#include <QtQml>
#include <QColorDialog>

void PageManager::CreateMeetPage(const bool) {

    SETTINGS = Settings();

    if (timer) {
        timer->stop();
        timer->deleteLater();
        timer = nullptr;
    }
    canvas_widget = nullptr;

    if (current_view == nullptr) {
        current_view = new QQuickView();
        current_view->engine()->rootContext()->setContextProperty("manager", this);
        current_view->setSource(QUrl::fromLocalFile("../qml/main.qml"));
    }

    current_view->resize(550, 600);
    current_view->setMaximumSize(current_view->size());
    current_view->setMinimumSize(current_view->size());

    pic_params = {800, 800, 100, 100};

    current_view->show();
}

void PageManager::Initialize() {
    this->CreateMeetPage(true);
}

PageManager::~PageManager() {
    delete graph;
}

bool PageManager::ValidateInput(const QString &input) {
    return !input.trimmed().isEmpty();
}

bool PageManager::ValidateFile(QString file_path) {
    file_path.remove(0, 8);
    QFile file(file_path);
    if (!file.open(QIODevice::ReadOnly)) return false;
    file.close();
    return true;
}

void PageManager::CenterScreen() const {
    const QRect screen_geometry = this->current_view->screen()->geometry();
    const int x = (screen_geometry.width() - this->current_view->width()) / 2;
    const int y = (screen_geometry.height() - this->current_view->height()) / 2;
    this->current_view->setPosition(QPoint(x, y));

}

void PageManager::SetInputAreaPageProperties() const {
    this->current_view->setMaximumSize(QSize(10000, 10000));
    this->current_view->setMinimumSize(QSize(200, 400));
}

void PageManager::SetMeetPageProperties() const {
    this->current_view->resize(550, 600);
    this->current_view->setMaximumSize(this->current_view->size());
    this->current_view->setMinimumSize(this->current_view->size());
}

void PageManager::SetMainPageProperties() const {
    this->current_view->setMaximumSize(QSize(10000, 1000));
    this->current_view->setMinimumSize(QSize(0, 0));
    this->current_view->resize(1600, 900);

    this->CenterScreen();
}

QColor PageManager::GetColor(QColor initial_color = Qt::white) {
    auto *widget = dynamic_cast<QWidget *>(this);
    return QColorDialog::getColor(initial_color, widget, "Choose a color");
}

VertexLayout *PageManager::GetNodes() {
    if (!layout.empty())
        return &layout;

    if (this->topology_type == "Circulant" ||
        this->topology_type == "Spidergon" ||
        this->topology_type == "Ring" ||
        this->topology_type == "Paley" ||
        this->topology_type == "Multiplicative Circulant" ||
        this->topology_type == "HyperCirculant") {
        layout = MakeCircle(*(this->graph), pic_params);
    } else if (this->topology_type == "Mesh" || this->topology_type == "Torus" || this->topology_type == "TMesh" ||
               this->topology_type == "DMeshDoubleLinked" || this->topology_type == "DMesh" ||
               this->topology_type == "ZMesh" || this->topology_type == "DCM" || this->topology_type == "CBPMesh" ||
               this->topology_type == "StarMesh" || this->topology_type == "L2Star") {
        layout = MakeGrid(*dynamic_cast<Mesh *>(this->graph), pic_params);
    } else if (this->topology_type == "MCC") {
        layout = MakeGrid(*dynamic_cast<Mesh *>(this->graph), pic_params);
        auto subgrid = MakeMCCSubgrid(*dynamic_cast<MCC *>(this->graph), layout);
        layout.insert(layout.end(), subgrid.begin(), subgrid.end());
    } else if (this->topology_type == "XNetwork") {
        layout = MakeGrid(*dynamic_cast<Mesh *>(this->graph), pic_params);
        auto subgrid = MakeXNSubgrid(*dynamic_cast<XNetwork *>(this->graph), layout);
        layout.insert(layout.end(), subgrid.begin(), subgrid.end());
    } else if (this->topology_type == "FatTree" || this->topology_type == "XTree") {
        layout = MakeTree(*dynamic_cast<TreeBase *>(this->graph), pic_params);
    } else {
        Circulant circulant(5, {1, 2});
        layout = MakeCircle(circulant, pic_params);
    }

    for (auto &node : layout) {
        node.x_origin = node.x;
        node.y_origin = node.y;
    }

    return &layout;
}

std::vector<EdgeGroup> *PageManager::GetEdges() {
    if (!edges.empty())
        return &edges;

    if (this->topology_type == "Circulant" || this->topology_type == "Multiplicative Circulant" ||
        this->topology_type == "Paley") {
        edges = MakeEdges(*dynamic_cast<Circulant *>(this->graph));
    } else if (this->topology_type == "HyperCirculant") {
        edges = MakeEdges(*dynamic_cast<HyperCirculant *>(this->graph));
    } else if (this->topology_type == "Mesh") {
        edges = MakeEdges(*dynamic_cast<Mesh *>(this->graph));
    } else if (this->topology_type == "Spidergon") {
        edges = MakeEdges(*dynamic_cast<Spidergon *>(this->graph));
    } else if (this->topology_type == "Ring") {
        edges = MakeEdges(*dynamic_cast<Ring *>(this->graph));
    } else if (this->topology_type == "Torus") {
        edges = MakeEdges(*dynamic_cast<Torus *>(this->graph));
    } else if (this->topology_type == "FatTree") {
        edges = MakeEdges(*dynamic_cast<FatTree *>(this->graph));
    } else if (this->topology_type == "XTree") {
        edges = MakeEdges(*dynamic_cast<XTree *>(this->graph));
    } else if (this->topology_type == "TMesh") {
        edges = MakeEdges(*dynamic_cast<TMesh *>(this->graph));
    } else if (this->topology_type == "DMesh") {
        edges = MakeEdges(*dynamic_cast<DMesh *>(this->graph));
    } else if (this->topology_type == "DMeshDoubleLinked") {
        edges = MakeEdges(*dynamic_cast<DMeshDoubleLinked *>(this->graph));
    } else if (this->topology_type == "ZMesh") {
        edges = MakeEdges(*dynamic_cast<ZMesh *>(this->graph));
    } else if (this->topology_type == "DCM") {
        edges = MakeEdges(*dynamic_cast<DCM *>(this->graph));
    } else if (this->topology_type == "MCC") {
        edges = MakeEdges(*dynamic_cast<MCC *>(this->graph));
    } else if (this->topology_type == "XNetwork") {
        edges = MakeEdges(*dynamic_cast<XNetwork *>(this->graph));
    } else if (this->topology_type == "CBPMesh") {
        edges = MakeEdges(*dynamic_cast<CBPMesh *>(this->graph));
    } else if (this->topology_type == "StarMesh") {
        edges = MakeEdges(*dynamic_cast<StarMesh *>(this->graph));
    } else if (this->topology_type == "L2Star") {
        edges = MakeEdges(*dynamic_cast<L2Star *>(this->graph));
    } else {
        throw std::runtime_error("Wrong topology");
    }

    user_groups_count = edges.size();
    return &edges;
}

QList<QString> PageManager::GetTopologiesList() {
    return {"Circulant", "Multiplicative Circulant", "Paley", "HyperCirculant", "Mesh", "Spidergon", "Ring", "Torus",
            "FatTree", "XTree", "TMesh", "DMesh", "DMeshDoubleLinked", "ZMesh", "DCM",
            "MCC", "XNetwork", "CBPMesh", "StarMesh", "L2Star"};
}

QList<QString> PageManager::GetGroupNames() {
    QList<QString> res;
    for (const auto &group: edges)
        res.push_back(group.name.data());
    return res;
}

void PageManager::AddGroup(int vertex_from, int vertex_to, const QString& group_to_add, QColor color, bool use_as_group) {
    const auto idx = FindEdge(vertex_from, vertex_to);
    if (idx.first < 0)
        return;
    if (!use_as_group) {
        ExcludeEdgeFromGroup(idx.first, idx.second, group_to_add.toStdString());
        edges.back().is_default_color = false;
        edges.back().color = color;
    } else {
        edges[idx.first].name = group_to_add.toStdString();
        edges[idx.first].is_default_color = false;
        edges[idx.first].color = color;
    }
}

void PageManager::AddGroupWithParameters(int vertex_from, int vertex_to, const QString& group_to_add,
                                         const QVariantMap &params, bool use_as_group) {
    const auto idx = FindEdge(vertex_from, vertex_to);
    if (idx.first < 0)
        return;
    int target_group_idx = idx.first;

    if (!use_as_group) {
        ExcludeEdgeFromGroup(idx.first, idx.second, group_to_add.toStdString());
        target_group_idx = edges.size() - 1;
    } else {
        edges[idx.first].name = group_to_add.toStdString();
    }


    int type = params["type"].toInt();

    switch (type) {
        case 0:
            edges[target_group_idx].type = EdgeType::ET_Line;
            break;
        case 1: // Arc
        {
            edges[target_group_idx].type = EdgeType::ET_Arc;
            auto ratio = params["ratio"].toFloat();
            edges[target_group_idx].edge_type_data = ArcData{ratio};
            break;
        }
        case 2: // BCurve
        {
            edges[target_group_idx].type = EdgeType::ET_BCurve;
            auto along1 = params["along1"].toFloat();
            auto across1 = params["across1"].toFloat();
            auto along2 = params["along2"].toFloat();
            auto across2 = params["across2"].toFloat();
            edges[target_group_idx].edge_type_data = BCurveData{along1, across1, along2, across2};
            break;
        }
    }
}

void PageManager::AddToGroup(int vertex_from, int vertex_to,
                             int target_group_idx, bool use_as_group) {
    const auto idx = FindEdge(vertex_from, vertex_to);
    if (idx.first < 0 || target_group_idx < 0 || target_group_idx >= static_cast<int>(edges.size()))
        return;
    if (!use_as_group) {
        ExcludeEdgeFromGroup(idx.first, idx.second, " ");
        MergeEdgeGroups(target_group_idx, edges.size() - 1);
    } else {
        MergeEdgeGroups(target_group_idx, idx.first);
    }
}

void PageManager::ChangeVertexColor(QColor color) {
    this->canvas_widget->ChangeVertexColor(color);
}

void PageManager::ChangeColor(int vertex_from, int vertex_to, QColor color, bool use_as_group) {
    const auto idx = FindEdge(vertex_from, vertex_to);
    if (idx.first < 0)
        return;
    if (!use_as_group) {
        ExcludeEdgeFromGroup(idx.first, idx.second);
        edges.back().is_default_color = false;
        edges.back().color = color;
    } else {
        edges[idx.first].is_default_color = false;
        edges[idx.first].color = color;
    }
}

void PageManager::ChangeParameters(int vertex_from, int vertex_to, const QVariantMap& params, bool use_as_group) {
    const auto idx = FindEdge(vertex_from, vertex_to);
    if (idx.first < 0)
        return;
    int target_group_idx = idx.first;

    canvas_widget->draw_points_set = false;

    if (!use_as_group)
    {
        ExcludeEdgeFromGroup(idx.first, idx.second);
        target_group_idx = edges.size() - 1;
    }


    int type = params["type"].toInt();

    switch (type) {
        case 0:
            edges[target_group_idx].type = EdgeType::ET_Line;
            break;
        case 1: // Arc
        {
            edges[target_group_idx].type = EdgeType::ET_Arc;
            auto ratio = params["ratio"].toFloat();
            edges[target_group_idx].edge_type_data = ArcData{ratio};
            break;
        }
        case 2: // BCurve
        {
            edges[target_group_idx].type = EdgeType::ET_BCurve;
            auto along1 = params["along1"].toFloat();
            auto across1 = params["across1"].toFloat();
            auto along2 = params["along2"].toFloat();
            auto across2 = params["across2"].toFloat();
            edges[target_group_idx].edge_type_data = BCurveData{along1, across1, along2, across2};
            break;
        }
    }
}

void PageManager::DeselectEdges() {
    this->canvas_widget->DeselectEdges();
}

void PageManager::DeselectVertices() {
    this->canvas_widget->DeselectVertices();
}


QList<int> PageManager::GetSelectedEdgeVertices() {
    const Edge *selected_edge = this->canvas_widget->GetSelectedEdge();
    if (!selected_edge)
        return {};
    return {selected_edge->from, selected_edge->to};
}

QString PageManager::GetSelectedEdgeGroupName() {
    return this->canvas_widget->GetSelectedEdgeGroupName();
}

void PageManager::SetInputDataFromTopology(const QString &new_data_type, const QString &data_value) {
    this->data_type = new_data_type;
    delete graph;

    layout.clear();
    edges.clear();

    if (new_data_type == "topology_parameters") {
        QString data_value_t = data_value;
        data_value_t = data_value_t.replace("\\\"", "\"");
        QJsonObject parameters = QJsonDocument::fromJson(data_value_t.toUtf8()).object();

        QString topology = parameters.value("topology").toString();

        this->topology_type = topology;

        if (topology == "Circulant") {
            uint16_t nodes = parameters.value("nodes_number").toString().toInt();
            std::vector<uint16_t> generators;

            for (auto num: parameters.value("generators").toArray()) {
                generators.push_back(num.toString().toInt());
            }

            graph = new Circulant(nodes, generators);
        } else if (topology == "Paley") {
            const uint16_t nodes = parameters.value("nodes").toString().toInt();
            const uint16_t degree = parameters.value("degree").toString().toInt();

            graph = new Circulant(nodes, GetPaleyGenx(nodes, degree));
        } else if (topology == "Multiplicative Circulant") {
            const uint16_t base = parameters.value("nodes").toString().toInt();
            const uint16_t degree = parameters.value("degree").toString().toInt();

            auto genx = GetMultiplicativeCirculantGenx(base, degree);
            auto nodes = genx.back() * base;

            graph = new Circulant(nodes, std::move(genx));
        } else if (topology == "HyperCirculant") {
            uint16_t nodes = parameters.value("nodes_number").toString().toInt();
            std::vector<std::vector<uint16_t>> generators;

            QJsonArray outerArray = parameters.value("generators").toArray();

            for (const QJsonValue &outerValue: outerArray) {
                std::vector<uint16_t> innerVector;
                QJsonArray innerArray = outerValue.toArray();

                for (const QJsonValue &innerValue: innerArray) {
                    innerVector.push_back(innerValue.toString().toUInt());
                }

                if (!innerVector.empty()) {
                    generators.push_back(innerVector);
                }
            }

            graph = new HyperCirculant(nodes, generators);
        } else if (topology == "Mesh") {
            const uint16_t rows = parameters.value("rows").toString().toInt();
            const uint16_t cols = parameters.value("columns").toString().toInt();

            graph = new Mesh(rows, cols);
        } else if (topology == "Spidergon") {
            uint16_t nodes = parameters.value("nodes_number").toString().toInt();
            graph = new Spidergon(nodes);
        } else if (topology == "Ring") {
            uint16_t nodes = parameters.value("nodes_number").toString().toInt();
            graph = new Ring(nodes);
        } else if (topology == "Torus") {
            const uint16_t rows = parameters.value("rows").toString().toInt();
            const uint16_t cols = parameters.value("columns").toString().toInt();

            graph = new Torus(rows, cols);
        } else if (topology == "FatTree") {
            const uint16_t concentration = parameters.value("concentration").toString().toInt();
            const uint16_t levels = parameters.value("levels").toString().toInt();

            auto ft = new FatTree(concentration, levels);
            ft->Build();
            graph = ft;
        } else if (topology == "XTree") {
            const uint16_t levels = parameters.value("levels").toString().toInt();

            auto xt = new XTree(levels);
            xt->Build();
            graph = xt;
        } else if (topology == "TMesh") {
            const uint16_t rows = parameters.value("rows").toString().toInt();
            const uint16_t cols = parameters.value("columns").toString().toInt();

            graph = new TMesh(rows, cols);
        } else if (topology == "DMesh") {
            const uint16_t rows = parameters.value("rows").toString().toInt();
            const uint16_t cols = parameters.value("columns").toString().toInt();

            graph = new DMesh(rows, cols);
        } else if (topology == "DMeshDoubleLinked") {
            const uint16_t rows = parameters.value("rows").toString().toInt();
            const uint16_t cols = parameters.value("columns").toString().toInt();

            graph = new DMeshDoubleLinked(rows, cols);
        } else if (topology == "ZMesh") {
            const uint16_t rows = parameters.value("rows").toString().toInt();
            const uint16_t cols = parameters.value("columns").toString().toInt();

            graph = new ZMesh(rows, cols);
        } else if (topology == "DCM") {
            const uint16_t rows = parameters.value("rows").toString().toInt();
            const uint16_t cols = parameters.value("columns").toString().toInt();

            graph = new DCM(rows, cols);
        } else if (topology == "MCC") {
            const uint16_t rows = parameters.value("rows").toString().toInt();
            const uint16_t cols = parameters.value("columns").toString().toInt();

            graph = new MCC(rows, cols);
        } else if (topology == "XNetwork") {
            const uint16_t rows = parameters.value("rows").toString().toInt();
            const uint16_t cols = parameters.value("columns").toString().toInt();

            graph = new XNetwork(rows, cols);
        } else if (topology == "CBPMesh") {
            const uint16_t rows = parameters.value("rows").toString().toInt();
            const uint16_t cols = parameters.value("columns").toString().toInt();

            graph = new CBPMesh(rows, cols);
        } else if (topology == "StarMesh") {
            const uint16_t rows = parameters.value("rows").toString().toInt();
            const uint16_t cols = parameters.value("columns").toString().toInt();

            graph = new StarMesh(rows, cols);
        } else if (topology == "L2Star") {
            const uint16_t rows = parameters.value("rows").toString().toInt();
            const uint16_t cols = parameters.value("columns").toString().toInt();

            graph = new L2Star(rows, cols);
        }
    }
}

void PageManager::StartCanvasTimer() {
    if (timer)
        return;

    this->timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &PageManager::UpdateCanvasFrame);

    if (!timer->isActive()) {
        timer->start(16);
    }
}

void PageManager::UpdateCanvasFrame() {
    if (!canvas_widget)
        return;

    if (this->canvas_widget->edges_selected_updated) {
        emit edgesSelected(this->canvas_widget->edges_selected);
        this->canvas_widget->edges_selected_updated = false;
    }
    if (this->canvas_widget->vertices_selected_updated) {
        emit verticesSelected(this->canvas_widget->vertices_selected);
        this->canvas_widget->vertices_selected_updated = false;
    }

    if (this->canvas_widget->vertices_moved_updated) {
        emit verticesMoved(this->canvas_widget->vertices_moved);
        this->canvas_widget->vertices_moved_updated = false;
    }


    emit canvas_widget->UpdateFrame();
}

void PageManager::SetCanvasWidgetItem(QQuickItem *root) {
    if (root->children().isEmpty())
        return;
    QObject *widget = root->children().at(0);
    this->canvas_widget = dynamic_cast<CanvasWidget *>(widget);
    if (canvas_widget)
        this->StartCanvasTimer();
}

void PageManager::DrawShortestPath(int vertex_from, int vertex_to, const QString &color) {
    auto path = find_shortest_path(graph->GetEdges(), vertex_from, vertex_to);
    for (size_t i = 0; i + 1 < path.size(); ++i) {
        auto idx = FindEdge(path[i], path[i + 1]);
        ExcludeEdgeFromGroup(idx.first, idx.second);
        edges.back().is_default_color = false;
        edges.back().color = QColor(color);
    }
    this->canvas_widget->CalculateDrawPoints();
}

bool PageManager::CheckVerticesExist(const QString &vertex_from, const QString &vertex_to) {
    const uint16_t from_node = vertex_from.toInt();
    const uint16_t to_node = vertex_to.toInt();

    size_t size = layout.size();

    return from_node < size && to_node < size;
}

void PageManager::ResetPositions() {
    this->canvas_widget->ResetNodes();
}

void PageManager::ToggleDarkMode() {
    SETTINGS.dark_mode = !SETTINGS.dark_mode;
}

void PageManager::ToggleShowLabelText() {
    SETTINGS.show_label_text = !SETTINGS.show_label_text;
}

void PageManager::ToggleConstTextFontSize() {
    SETTINGS.const_font_size = !SETTINGS.const_font_size;
}

void PageManager::ToggleOutlineMode() {
    SETTINGS.is_outline_mode = !SETTINGS.is_outline_mode;
}

Settings PageManager::GetSettings() const {
    return SETTINGS;
}

void PageManager::ChangeEdgesWidth(qreal new_width) {
    SETTINGS.edges_width = new_width;
}

void PageManager::ChangeNodesRadius(qreal new_radius) {
    SETTINGS.nodes_radius = new_radius;
}

void PageManager::ChangeNodesFontSize(qint8 new_font_size) {
    SETTINGS.font_size = new_font_size;
}
