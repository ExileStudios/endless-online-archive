#ifndef Sys_effectsH
#define Sys_effectsH

#include <vector.h>

#include "Sys_effect.h"

// The spell effects playing on map tiles.
struct Effects
{
    char pad_0x0[8];
    vector<MapEffect> effects;
    int current_index;
    char pad_0x2c[4];

    Effects();
    ~Effects();
    void Animate();
    void RemoveFinished();
    void AddEffect(int x, int y, int effect_id);
    void Clear();
    MapEffect GetEffect(int index);
    int GetCount();
};

#endif
