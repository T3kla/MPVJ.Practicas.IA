#include <stdafx.h>

#include "params.h"
#include "pathfinder.h"
#include <stack>

static vector<Node> Nodes;
static vector<Node> Path;

static Node Start;
static Node Destination;

static NavMesh Nav;

Pathfinder::Pathfinder()
    : MOAIEntity2D(){RTTI_BEGIN RTTI_EXTEND(MOAIEntity2D) RTTI_END}

      Pathfinder::~Pathfinder()
{
}

void Pathfinder::UpdatePath()
{
    // Read file

    Nav = ReadNavMesh("navmesh.xml");

    // Update map


    // Update path
}

void Pathfinder::DrawDebug()
{
    MOAIGfxDevice &gfxDevice = MOAIGfxDevice::Get();
    auto Grey = 1.0f;
    gfxDevice.SetPenColor(Grey, Grey, Grey, Grey);

    // Draw polys

    for (auto&& poly : Nav.polygons)
    {
        gfxDevice.SetPenColor(0.1f, 0.1f, 1.0f, 0.3f);
        MOAIDraw::DrawPolygonFilled(poly.vertex);
        gfxDevice.SetPenColor(0.0f, 0.0f, 1.0f, 1.0f);
        MOAIDraw::DrawPolygon(poly.vertex);
    }

    // Draw links

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

        MOAIDraw::DrawEllipseFill(a.mX, a.mY, 5.f, 5.f, 22);
        MOAIDraw::DrawEllipseFill(b.mX, b.mY, 5.f, 5.f, 22);
        MOAIDraw::DrawLine(a, b);
    }

    // Draw path

    auto pathSize = Path.size();
    gfxDevice.SetPenColor(0.3f, 0.7f, 0.7f, 1.0f);

    for (size_t i = 0; i < pathSize; i++)
    {
        if (i >= pathSize - 1)
            break;

        Node a = Path[i], b = Path[i + 1];

        // MOAIDraw::DrawLine();
    }
}

bool Pathfinder::PathfindStep()
{
    return true;
}

bool Pathfinder::IsDestination(Node node)
{
    return node == Destination;
}

vector<Node> Pathfinder::CalculatePath(Node start, Node dest)
{
    vector<Node> empty;
    return empty;

    // return sqrt((x - dest.x) * (x - dest.x) + (y - dest.y) * (y - dest.y));
}

vector<Node> *Pathfinder::GetPath()
{
    return &Path;
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

    self->PathfindStep();
    return 0;
}
