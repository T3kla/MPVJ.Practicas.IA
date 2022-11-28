#include <stdafx.h>

#include "params.h"
#include "pathfinder.h"
#include <stack>

static vector<Node> Nodes;
static vector<Node*> Path;
static vector<USVec2D> PathPos;

static Node NodeStart;
static Node NodeDestination;
static Node* NodeStartPtr = &NodeStart;
static Node* NodeDestinationPtr = &NodeDestination;

static NavMesh Nav;

Pathfinder::Pathfinder()
    : MOAIEntity2D()
{   
    RTTI_BEGIN
        RTTI_EXTEND(MOAIEntity2D)
    RTTI_END
}

Pathfinder::~Pathfinder()
{
}

void Pathfinder::UpdatePath()
{
    Nodes.clear();
    Path.clear();
    PathPos.clear();

    // Read file

    Nav = ReadNavMesh("navmesh_complex.xml");

    // Generate nodes

    for (auto&& poly : Nav.polygons)
    {
        auto len = poly.vertex.size();

        for (size_t i = 0; i < len; i++)
        {
            auto a = poly.vertex[i];
            auto b = poly.vertex[i == len - 1 ? 0 : i + 1];

            auto pos = (b - a) / 2.f + a;

            Node newNode;
            newNode.id = poly.id;
            newNode.pos.mX = pos.mX;
            newNode.pos.mY = pos.mY;

            Nodes.push_back(newNode);
        }
    }

    // Connect nodes when they are in the same poly

    for (auto&& a : Nodes)
    for (auto&& b : Nodes)
    {
        if (a == b)
            continue;

        if (a.id == b.id)
            a.connected.emplace_back(&b);
    }
    
    // Connect nodes when they are linked

    for (auto&& link : Nav.links)
    {
        // Get nodes with the same id as the current link

        vector<Node*> aNodes;
        vector<Node*> bNodes;

        for (auto&& node : Nodes)
        {
            if (node.id == link.a.id)
                aNodes.emplace_back(&node);
            if (node.id == link.b.id)
                bNodes.emplace_back(&node);
        }

        // Calculate where the nodes should be

        auto aPoint0 = Nav.polygons[link.a.id].vertex[link.a.str];
        auto aPoint1 = Nav.polygons[link.a.id].vertex[link.a.end];
        auto bPoint0 = Nav.polygons[link.b.id].vertex[link.b.str];
        auto bPoint1 = Nav.polygons[link.b.id].vertex[link.b.end];

        auto aPoint = (aPoint1 - aPoint0) / 2.f + aPoint0;
        auto bPoint = (bPoint1 - bPoint0) / 2.f + bPoint0;

        // Match each proyected node with the existing ones

        Node* aClosest = aNodes.at(ClosestNodeTo(aNodes, aPoint));
        Node* bClosest = bNodes.at(ClosestNodeTo(bNodes, bPoint));

        // Connect the nodes

        aClosest->connected.push_back(bClosest);
        bClosest->connected.push_back(aClosest);
    }

    // Connect start and destination nodes

    NodeStart.connected.clear();
    NodeDestination.connected.clear();

    auto* aNode = &Nodes.at(ClosestNodeTo(Nodes, NodeStart.pos));
    auto* bNode = &Nodes.at(ClosestNodeTo(Nodes, NodeDestination.pos));
    aNode->connected.push_back(NodeStartPtr);
    NodeStart.connected.push_back(aNode);
    bNode->connected.push_back(NodeDestinationPtr);
    NodeDestination.connected.push_back(bNode);

    // Add start and destination nodes for drawing

    Nodes.push_back(NodeStart);
    Nodes.push_back(NodeDestination);

    // Update path

    Path = CalculatePath();

    // Update path for stearing

    for (auto &&node : Path)
        PathPos.push_back(node->pos);
}

void Pathfinder::SetStartPosition(float x, float y)
{
    NodeStart.pos = USVec2D(x, y);
    UpdatePath();
}

void Pathfinder::SetEndPosition(float x, float y)
{
    NodeDestination.pos = USVec2D(x, y);
    UpdatePath();
}

void Pathfinder::DrawDebug()
{
    MOAIGfxDevice &gfxDevice = MOAIGfxDevice::Get();
    size_t len = 0;

    // Draw polys

    for (auto&& poly : Nav.polygons)
    {
        gfxDevice.SetPenColor(0.1f, 0.1f, 1.0f, 0.3f);
        MOAIDraw::DrawPolygonFilled(poly.vertex);
        gfxDevice.SetPenColor(0.0f, 0.0f, 1.0f, 1.0f);
        MOAIDraw::DrawPolygon(poly.vertex);
    }

    // Draw links between polys

    gfxDevice.SetPenColor(1.0f, 0.0f, 1.0f, 1.0f);
    for (auto&& link : Nav.links)
    {
        auto aPoly = Nav.polygons[link.a.id];
        auto bPoly = Nav.polygons[link.b.id];
        auto aVertex0 = aPoly.vertex[link.a.str];
        auto aVertex1 = aPoly.vertex[link.a.end];
        auto bVertex0 = bPoly.vertex[link.b.str];
        auto bVertex1 = bPoly.vertex[link.b.end];

        USVec2D a = (aVertex1 - aVertex0) / 2.f + aVertex0;
        USVec2D b = (bVertex1 - bVertex0) / 2.f + bVertex0;

        gfxDevice.SetPenColor(0.1f, 0.1f, 1.0f, 0.3f);
        MOAIDraw::DrawEllipseFill(a.mX, a.mY, 6.f, 6.f, 22);
        MOAIDraw::DrawEllipseFill(b.mX, b.mY, 6.f, 6.f, 22);
        gfxDevice.SetPenColor(0.0f, 0.0f, 1.0f, 1.0f);
        MOAIDraw::DrawEllipseOutline(a.mX, a.mY, 6.f, 6.f, 22);
        MOAIDraw::DrawEllipseOutline(b.mX, b.mY, 6.f, 6.f, 22);
        MOAIDraw::DrawLine(a, b);
    }

    // Draw connections between nodes

    len = Nodes.size();
    for (size_t i = 0; i < len; i++)
    {
        auto a = USVec2D(Nodes[i].pos.mX, Nodes[i].pos.mY);

        for (auto&& conn : Nodes[i].connected)
        {
            auto b = USVec2D(conn->pos.mX, conn->pos.mY);

            gfxDevice.SetPenColor(0.8f, 0.1f, 0.1f, 1.0f);
            MOAIDraw::DrawLine(a, b);
        }
    }

    // Draw nodes

    for (size_t i = 0; i < len; i++)
    {
        auto a = USVec2D(Nodes[i].pos.mX, Nodes[i].pos.mY);

        gfxDevice.SetPenColor(0.1f, 1.0f, 0.1f, 0.3f);
        MOAIDraw::DrawEllipseFill(a.mX, a.mY, 4.f, 4.f, 22);
        gfxDevice.SetPenColor(0.0f, 0.4f, 0.0f, 1.0f);
        MOAIDraw::DrawEllipseOutline(a.mX, a.mY, 4.f, 4.f, 22);
    }
    
    // Draw path

    gfxDevice.SetPenColor(1.0f, 1.0f, 1.0f, 1.0f);
    len = Path.size();
    for (size_t i = 0; i < len; i++)
    {
        if (i >= len - 1)
            break;

        Node* a = Path[i], *b = Path[i + 1];
        MOAIDraw::DrawLine(a->pos, b->pos);
    }
}

bool Pathfinder::IsDestination(Node node)
{
    return node == NodeStart;
}

float Pathfinder::CalculateH(USVec2D a, USVec2D b)
{
    return sqrt((a.mX - b.mX) * (a.mX - b.mX) + (a.mY - b.mY) * (a.mY - b.mY));
}

void Pathfinder::Rec(Node* cur, vector<Node*> path, float pathFitness, vector<Node*> *bestPath, float *bestPathFitness)
{
    path.push_back(cur);

    // If reached destination, compare fitness and update best path

    if (*cur == *NodeDestinationPtr)
        if (pathFitness < *bestPathFitness)
        {
            *bestPath = path;
            *bestPathFitness = pathFitness;
        }

    // Search next node

    for (auto *next : cur->connected)
    {
        if (find(path.begin(), path.end(), next) != path.end())
            continue;

        // Shortens search by several hundreds

        if (pathFitness > *bestPathFitness)
            continue;

        Rec(next, path, pathFitness + CalculateH(cur->pos, next->pos), bestPath, bestPathFitness);
    }
}

vector<Node*> Pathfinder::CalculatePath()
{
    vector<Node*> bestPath;
    float bestPathFitness = FLT_MAX;

    Rec(&NodeStart, {}, 0.f, &bestPath, &bestPathFitness);

    return bestPath;
}

int Pathfinder::ClosestNodeTo(vector<Node*> vec, USVec2D pos)
{
    size_t index = 0;
    float distance = FLT_MAX;

    auto len = vec.size();
    for (size_t i = 0; i < len; i++)
    {
        auto b = vec[i];
        auto aPos = USVec2D(pos.mX, pos.mY);
        auto bPos = USVec2D(b->pos.mX, b->pos.mY);

        auto dis = sqrt((aPos.mX - bPos.mX) * (aPos.mX - bPos.mX) + (aPos.mY - bPos.mY) * (aPos.mY - bPos.mY));

        if (dis >= distance)
            continue;

        index = i;
        distance = dis;
    }

    return index;
}

int Pathfinder::ClosestNodeTo(vector<Node> vec, USVec2D pos)
{
    size_t index = 0;
    float distance = FLT_MAX;

    auto len = vec.size();
    for (size_t i = 0; i < len; i++)
    {
        auto b = vec[i];
        auto aPos = USVec2D(pos.mX, pos.mY);
        auto bPos = USVec2D(b.pos.mX, b.pos.mY);

        auto dis = sqrt((aPos.mX - bPos.mX) * (aPos.mX - bPos.mX) + (aPos.mY - bPos.mY) * (aPos.mY - bPos.mY));

        if (dis >= distance)
            continue;

        index = i;
        distance = dis;
    }

    return index;
}

vector<USVec2D> *Pathfinder::GetPath()
{
    return &PathPos;
}

// ----------------------------------------------------------------------- LUA

void Pathfinder::RegisterLuaFuncs(MOAILuaState &state)
{
    MOAIEntity::RegisterLuaFuncs(state);

    luaL_Reg regTable[] = {{"setStartPosition", _setStartPosition},
                           {"setEndPosition", _setEndPosition},
                           {"pathfindStep", _pathfindStep},
                           {NULL, NULL}};

    luaL_register(state, 0, regTable);
}

int Pathfinder::_setStartPosition(lua_State *L)
{
    MOAI_LUA_SETUP(Pathfinder, "U")

    float pX = state.GetValue<float>(2, 0.0f);
    float pY = state.GetValue<float>(3, 0.0f);
    self->SetStartPosition(pX, pY);
    return 0;
}

int Pathfinder::_setEndPosition(lua_State *L)
{
    MOAI_LUA_SETUP(Pathfinder, "U")

    float pX = state.GetValue<float>(2, 0.0f);
    float pY = state.GetValue<float>(3, 0.0f);
    self->SetEndPosition(pX, pY);
    return 0;
}

int Pathfinder::_pathfindStep(lua_State *L)
{
    MOAI_LUA_SETUP(Pathfinder, "U")

    return 0;
}
