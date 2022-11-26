#include <stdafx.h>

#include "pathfinder.h"
#include <stack>

static constexpr int MapSide = 20;
static constexpr int MapCarriage = 2;
static constexpr int MapSideTotal = MapSide + MapCarriage;
static constexpr int MapSize = MapSide * MapSideTotal;
static char MapBuffer[MapSide][MapSideTotal];
static Node NodeMap[MapSide][MapSide];
static size_t MapRawSize = 0;

static const USVec2D GridPos = USVec2D(-300.f, -300.f);
static const float GridSpc = 30.f;

static vector<Node> Path;

Pathfinder::Pathfinder()
    : MOAIEntity2D(){RTTI_BEGIN RTTI_EXTEND(MOAIEntity2D) RTTI_END}

      Pathfinder::~Pathfinder()
{
}

void Pathfinder::UpdatePath()
{
    // Read file

    memset(MapBuffer, 0, MapSide * MapSideTotal);
    memset(NodeMap, 0, MapSide * MapSide * sizeof(Node));

    if (auto file = fopen("../sample/map.txt", "rb"))
        MapRawSize = fread(MapBuffer, 1, MapSize, file);

    // Update map

    USVec2D TL = GridPos + USVec2D(GridSpc, GridSpc) / 2.f;
    float Spc = GridSpc / 2.f;

    USIntVec2D CoordsStart, CoordsEnd;

    for (int y = 0; y < MapSideTotal; y++)
        for (int x = 0; x < MapSideTotal; x++)
        {
            if (x >= 20 || y >= 20)
                continue;

            char chr = MapBuffer[y][x];

            if (chr == 'S')
            {
                m_StartPosition = {TL.mX + GridSpc * x, TL.mY + GridSpc * y};
                CoordsStart = {x, y};
            }

            if (chr == 'E')
            {
                m_EndPosition = {TL.mX + GridSpc * x, TL.mY + GridSpc * y};
                CoordsEnd = {x, y};
            }

            Node newNode;
            newNode.ch = chr;
            newNode.x = x;
            newNode.y = y;

            NodeMap[y][x] = newNode;
        }

    // Update path

    Path = CalculatePath(NodeMap[CoordsStart.mY][CoordsStart.mX], NodeMap[CoordsEnd.mY][CoordsEnd.mX]);
}

void Pathfinder::DrawDebug()
{
    MOAIGfxDevice &gfxDevice = MOAIGfxDevice::Get();
    auto Grey = 0.3f;
    gfxDevice.SetPenColor(Grey, Grey, Grey, Grey);

    // Draw grid

    USVec2D x0 = GridPos;
    USVec2D x1 = GridPos + USVec2D(GridSpc * MapSide, 0);

    for (size_t i = 0; i < MapSide + 1; i++)
    {
        x0.mY = GridPos.mY + GridSpc * i;
        x1.mY = GridPos.mY + GridSpc * i;
        MOAIDraw::DrawLine(x0, x1);
    }

    USVec2D y0 = GridPos;
    USVec2D y1 = GridPos + USVec2D(0, GridSpc * MapSide);

    for (size_t i = 0; i < MapSide + 1; i++)
    {
        y0.mX = GridPos.mX + GridSpc * i;
        y1.mX = GridPos.mX + GridSpc * i;
        MOAIDraw::DrawLine(y0, y1);
    }

    // Draw map

    USVec2D TL = GridPos + USVec2D(GridSpc, GridSpc) / 2.f;
    float Spc = GridSpc / 2.f;

    for (size_t y = 0; y < MapSide; y++)
        for (size_t x = 0; x < MapSide; x++)
        {
            if (x >= 20 || y >= 20)
                continue;

            Node node = NodeMap[y][x];
            char chr = node.ch;

            Grey = 1.0f;
            gfxDevice.SetPenColor(Grey, Grey, Grey, Grey);
            if (node.ch == '#')
                MOAIDraw::DrawRectFill(TL.mX + GridSpc * x - Spc, TL.mY + GridSpc * y - Spc, TL.mX + GridSpc * x + Spc,
                                       TL.mY + GridSpc * y + Spc);

            Grey = 0.2f;
            gfxDevice.SetPenColor(Grey, Grey, Grey, Grey);
            if (chr == 'A')
                MOAIDraw::DrawRectFill(TL.mX + GridSpc * x - Spc, TL.mY + GridSpc * y - Spc, TL.mX + GridSpc * x + Spc,
                                       TL.mY + GridSpc * y + Spc);

            Grey = 0.3f;
            gfxDevice.SetPenColor(Grey, Grey, Grey, Grey);
            if (chr == 'B')
                MOAIDraw::DrawRectFill(TL.mX + GridSpc * x - Spc, TL.mY + GridSpc * y - Spc, TL.mX + GridSpc * x + Spc,
                                       TL.mY + GridSpc * y + Spc);

            Grey = 0.5f;
            gfxDevice.SetPenColor(Grey, Grey, Grey, Grey);
            if (chr == 'C')
                MOAIDraw::DrawRectFill(TL.mX + GridSpc * x - Spc, TL.mY + GridSpc * y - Spc, TL.mX + GridSpc * x + Spc,
                                       TL.mY + GridSpc * y + Spc);

            Grey = 0.6f;
            gfxDevice.SetPenColor(Grey, Grey, Grey, Grey);
            if (chr == 'D')
                MOAIDraw::DrawRectFill(TL.mX + GridSpc * x - Spc, TL.mY + GridSpc * y - Spc, TL.mX + GridSpc * x + Spc,
                                       TL.mY + GridSpc * y + Spc);

            gfxDevice.SetPenColor(0.3f, 0.7f, 0.3f, 1.0f);
            if (chr == 'S')
                MOAIDraw::DrawRectFill(TL.mX + GridSpc * x - Spc, TL.mY + GridSpc * y - Spc, TL.mX + GridSpc * x + Spc,
                                       TL.mY + GridSpc * y + Spc);

            gfxDevice.SetPenColor(0.7f, 0.3f, 0.3f, 1.0f);
            if (chr == 'E')
                MOAIDraw::DrawRectFill(TL.mX + GridSpc * x - Spc, TL.mY + GridSpc * y - Spc, TL.mX + GridSpc * x + Spc,
                                       TL.mY + GridSpc * y + Spc);
        }

    // Draw path

    auto pathSize = Path.size();
    gfxDevice.SetPenColor(0.3f, 0.7f, 0.7f, 1.0f);

    for (size_t i = 0; i < pathSize; i++)
    {
        if (i >= pathSize - 1)
            break;

        Node a = Path[i], b = Path[i + 1];

        MOAIDraw::DrawLine(TL.mX + GridSpc * a.x, TL.mY + GridSpc * a.y, TL.mX + GridSpc * b.x, TL.mY + GridSpc * b.y);
    }
}

bool Pathfinder::PathfindStep()
{
    return true;
}

bool Pathfinder::IsValid(int x, int y)
{
    if (x * y > MapSide * MapSide)
        return false;

    char chr = NodeMap[y][x].ch;

    return chr != '#';
}

bool Pathfinder::IsDestination(int x, int y)
{
    char chr = NodeMap[y][x].ch;

    return chr == 'E';
}

float Pathfinder::CalculateH(int x, int y, Node dest)
{
    return sqrt((x - dest.x) * (x - dest.x) + (y - dest.y) * (y - dest.y));
}

float Pathfinder::AddedCost(char ch)
{
    if (ch == 'A')
        return 2.f;
    if (ch == 'B')
        return 4.f;
    if (ch == 'C')
        return 6.f;
    if (ch == 'D')
        return 8.f;
    return 1.f;
}

vector<Node> Pathfinder::CalculatePath(Node start, Node dest)
{
    vector<Node> empty;

    if (IsValid(dest.x, dest.y) == false)
    {
        cout << "Destination is an obstacle" << endl;
        return empty;
    }

    if (IsDestination(start.x, start.y))
    {
        cout << "You are the destination" << endl;
        return empty;
    }

    bool closedList[MapSide][MapSide];

    memset(closedList, false, MapSide * MapSide);

    int x = start.x;
    int y = start.y;
    NodeMap[y][x].fCost = 0.0;
    NodeMap[y][x].gCost = 0.0;
    NodeMap[y][x].hCost = 0.0;
    NodeMap[y][x].parentX = x;
    NodeMap[y][x].parentY = y;

    vector<Node> openList;

    openList.emplace_back(NodeMap[y][x]);
    bool destinationFound = false;

    while (!openList.empty() && openList.size() < MapSide * MapSide)
    {
        Node node;

        do
        {
            float temp = FLT_MAX;

            vector<Node>::iterator itNode;

            for (vector<Node>::iterator it = openList.begin(); it != openList.end(); it = next(it))
            {
                Node n = *it;

                if (n.fCost < temp)
                {
                    temp = n.fCost;
                    itNode = it;
                }
            }

            node = *itNode;
            openList.erase(itNode);

        } while (IsValid(node.x, node.y) == false);

        x = node.x;
        y = node.y;
        closedList[y][x] = true;

        for (int newY = -1; newY <= 1; newY++)
            for (int newX = -1; newX <= 1; newX++)
            {
                double gNew, hNew, fNew;

                if (!IsValid(x + newX, y + newY))
                    continue;

                if (IsDestination(x + newX, y + newY))
                {
                    // Destination found - make path
                    NodeMap[y + newY][x + newX].parentX = x;
                    NodeMap[y + newY][x + newX].parentY = y;
                    destinationFound = true;
                    return MakePath(dest);
                }

                if (closedList[y + newY][x + newX] == false)
                {
                    gNew = node.gCost + 1.0;
                    hNew = CalculateH(x + newX, y + newY, dest) + AddedCost(node.ch);
                    fNew = gNew + hNew;

                    if (NodeMap[y + newY][x + newX].fCost == FLT_MAX || NodeMap[y + newY][x + newX].fCost > fNew)
                    {
                        // Update the details of this neighbour node
                        NodeMap[y + newY][x + newX].fCost = fNew;
                        NodeMap[y + newY][x + newX].gCost = gNew;
                        NodeMap[y + newY][x + newX].hCost = hNew;
                        NodeMap[y + newY][x + newX].parentX = x;
                        NodeMap[y + newY][x + newX].parentY = y;
                        openList.emplace_back(NodeMap[y + newY][x + newX]);
                    }
                }
            }
    }

    if (destinationFound == false)
    {
        cout << "Destination not found" << endl;
        return empty;
    }
}

vector<Node> Pathfinder::MakePath(Node dest)
{
    try
    {
        auto map = NodeMap;

        cout << "Found a path" << endl;

        int x = dest.x;
        int y = dest.y;

        stack<Node> path;
        vector<Node> usablePath;

        while (!(map[y][x].parentX == x && map[y][x].parentY == y) && map[y][x].x != -1 && map[y][x].y != -1)
        {
            path.push(map[y][x]);
            int tempX = map[y][x].parentX;
            int tempY = map[y][x].parentY;
            x = tempX;
            y = tempY;
        }

        path.push(map[y][x]);

        while (!path.empty())
        {
            Node top = path.top();
            path.pop();
            usablePath.emplace_back(top);
        }

        return usablePath;
    }
    catch (const exception &e)
    {
        cout << e.what() << endl;
    }
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
