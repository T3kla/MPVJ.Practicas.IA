#ifndef __PATHFINDER_H__
#define __PATHFINDER_H__

#include <moaicore/MOAIEntity2D.h>
#include <vector>

struct Node
{
    char ch = '.';
    int y = -1;
    int x = -1;
    int parentX = -1;
    int parentY = -1;
    float gCost = FLT_MAX;
    float hCost = FLT_MAX;
    float fCost = FLT_MAX;
};

inline bool operator<(const Node &lhs, const Node &rhs)
{
    return lhs.fCost < rhs.fCost;
}

class Pathfinder : public virtual MOAIEntity2D
{
  public:
    Pathfinder();
    ~Pathfinder();

    virtual void DrawDebug();

    void SetStartPosition(float x, float y)
    {
        m_StartPosition = USVec2D(x, y);
        UpdatePath();
    }
    void SetEndPosition(float x, float y)
    {
        m_EndPosition = USVec2D(x, y);
        UpdatePath();
    }

    const USVec2D &GetStartPosition() const
    {
        return m_StartPosition;
    }
    const USVec2D &GetEndPosition() const
    {
        return m_EndPosition;
    }

    bool PathfindStep();

    bool IsValid(int x, int y);
    bool IsDestination(int x, int y);

    float CalculateH(int x, int y, Node dest);
    float AddedCost(char ch);

    vector<Node> CalculatePath(Node start, Node dest);
    vector<Node> MakePath(Node dest);

    static vector<Node> *GetPath();

  private:
    void UpdatePath();

    USVec2D m_StartPosition;
    USVec2D m_EndPosition;

    // ----------------------------------------------------------------------- LUA

  public:
    DECL_LUA_FACTORY(Pathfinder)
  public:
    virtual void RegisterLuaFuncs(MOAILuaState &state);

  private:
    static int _setStartPosition(lua_State *L);
    static int _setEndPosition(lua_State *L);
    static int _pathfindStep(lua_State *L);
};

#endif
