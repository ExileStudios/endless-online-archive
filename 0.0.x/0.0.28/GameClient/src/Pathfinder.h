#ifndef PathfinderH
#define PathfinderH

#include <vector.h>

#include "Pathnode.h"

// Walks the player to a clicked tile: a search of up to 12 steps, then one step at a time.
struct PathFinder
{
    vector<PathNode> nodes;
    Windows::TPoint steps[16];
    int generation_end;
    int previous_generation_end;
    int generation_start;
    int current_step;
    int step_count;
    char is_active;
    char pad_0xb5[3];

    PathFinder();
    ~PathFinder();
    char FindPath(int target_x, int target_y);
    char ContainsNode(int x, int y);
    void FollowPath();
    char IsPathActive();
    void CancelPath();
    static Windows::TPoint GetCurrentStep(PathFinder *self);
    void AdvanceStep();
};

#endif
