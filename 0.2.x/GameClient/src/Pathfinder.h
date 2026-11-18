#ifndef PathfinderH
#define PathfinderH

#include <new>
#include <vector.h>

#include "Pathnode.h"

// Walks the player to a clicked tile: a search of up to 12 steps, then one step at a time.
struct PathFinder
{
    vector<PathNode> nodes;
    TPoint steps[16];
    int generation_end;
    int previous_generation_end;
    int generation_start;
    int current_step;
    int step_count;
    char is_active;

    PathFinder();
    ~PathFinder();
    char FindPath(int target_x, int target_y);
    char ContainsNode(int x, int y);
    void FollowPath();
    char IsPathActive();
    void CancelPath();
    TPoint GetCurrentStep();
    void AdvanceStep();
};

#endif
