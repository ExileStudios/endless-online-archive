#ifndef Sys_effectsH
#define Sys_effectsH

#include <new>
#include <vector.h>

#include "Sys_effect.h"

// The spell effects playing on map tiles.
struct Effects
{
    char unused_bytes[4];
    vector<MapEffect> effects;
    int current_index;

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
