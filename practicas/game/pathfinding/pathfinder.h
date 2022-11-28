#ifndef __PATHFINDER_H__
#define __PATHFINDER_H__

#include <moaicore/MOAIEntity2D.h>
#include <vector>

struct Node
{
    int id = -1;
    USVec2D pos = { 0.f,0.f };
    vector<Node *> connected;
};

inline bool operator==(const Node &lhs, const Node &rhs)
{
    return lhs.id == rhs.id && lhs.pos.mX == rhs.pos.mX && lhs.pos.mY == rhs.pos.mY;
}

class Pathfinder : public virtual MOAIEntity2D
{
public:
    Pathfinder();
    ~Pathfinder();

    virtual void DrawDebug();

    void SetStartPosition(float x, float y);
    void SetEndPosition(float x, float y);

    const USVec2D &GetStartPosition() const
    {
        return m_StartPosition;
    }

    const USVec2D &GetEndPosition() const
    {
        return m_EndPosition;
    }

    bool IsDestination(Node node);

    float CalculateH(USVec2D a, USVec2D b);

    void Rec(Node* cur, vector<Node*> path, float pathFitness, vector<Node*>* bestPath, float* bestPathFitness);

    vector<Node*> CalculatePath();

    int ClosestNodeTo(vector<Node*> vec, USVec2D pos);
    int ClosestNodeTo(vector<Node> vec, USVec2D pos);

    static vector<USVec2D> *GetPath();

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
