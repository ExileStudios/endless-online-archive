#ifndef GamecontrolH
#define GamecontrolH

#include <Classes.hpp>
#include <SysUtils.hpp>
#include <ScktComp.hpp>
#include <DBTables.hpp>
#include <string.h>
#include <stdio.h>
#include <vector.h>
#include <deque.h>
#include <list.h>
#include <stack.h>
#include <queue.h>

struct MapCoord;


struct Game;

// The game's formulas: experience per level, hit rate, armor and element damage.
struct Game
{
    char pad_0x0[8];

    Game();
    ~Game();
    int Exp_RequiredForLevel(int level);
    int Combat_CalcArmorPen(int avg_dmg, int armor, double factor);
    int Combat_CalcHitRate(int accuracy, int evade, double factor);
    long double Combat_CalcElementMult(MapCoord element, short atk_power, short target_value);
    int Combat_ElementScore(int atk_power, int target_value);
};

#endif
