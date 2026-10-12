#include <vcl.h>
#pragma hdrstop

#include "Gamecontrol.h"
#include "Shopvalues.h"

#pragma package(smart_init)

Game::Game()
{
}

Game::~Game()
{
}

// The experience a level needs.
int Game::Exp_RequiredForLevel(int level)
{
    int required_exp = 0;
    if (level > 0)
        required_exp = level * 1.1 * (level * 1.1) * (level * 1.1) * 100.0;
    return required_exp;
}

// How much of a hit gets through armor: 10 to 90, 50 when the damage equals the armor; factor sets the range.
int Game::Combat_CalcArmorPen(int avg_dmg, int armor, double factor)
{
    double result = 50.0;
    if (avg_dmg < armor)
    {
        double low_limit = armor;
        low_limit = low_limit - low_limit * factor;
        if (low_limit < 0.0)
            low_limit = 0.0;
        double damage_shortfall = armor - avg_dmg;
        double low_span = armor - low_limit;
        double low_rating = damage_shortfall / low_span * 40.0;
        if (avg_dmg < low_limit)
            result = 10.0;
        else
            result -= low_rating;
    }
    if (avg_dmg > armor)
    {
        double high_limit = armor;
        high_limit = high_limit * factor + high_limit + 1.0;
        double damage_excess = avg_dmg - armor;
        double high_span = high_limit - armor;
        double high_rating = damage_excess / high_span * 40.0;
        if (avg_dmg > high_limit)
            result = 90.0;
        else
            result += high_rating;
    }
    return result;
}

// The hit rate: 20 to 80, 50 when accuracy equals evade; factor sets the range.
int Game::Combat_CalcHitRate(int accuracy, int evade, double factor)
{
    double result = 50.0;
    if (accuracy < evade)
    {
        double low_limit = evade;
        low_limit = low_limit - low_limit * factor;
        if (low_limit < 0.0)
            low_limit = 0.0;
        double accuracy_shortfall = evade - accuracy;
        double low_span = evade - low_limit;
        double low_rating = accuracy_shortfall / low_span * 30.0;
        if (accuracy < low_limit)
            result = 20.0;
        else
            result -= low_rating;
    }
    if (accuracy > evade)
    {
        double high_limit = evade;
        high_limit = high_limit * factor + high_limit + 1.0;
        double accuracy_excess = accuracy - evade;
        double high_span = high_limit - evade;
        double high_rating = accuracy_excess / high_span * 30.0;
        if (accuracy > high_limit)
            result = 80.0;
        else
            result += high_rating;
    }
    return result;
}

// The damage multiplier of an element; 1.0 when it adds nothing.
long double Game::Combat_CalcElementMult(MapCoord element, short atk_power, short target_value)
{
    double result = 1.0;
    if (element.x > 0)
    {
        if (element.x == 2)
            result = atk_power + element.y / 10 + 8;
        else if (target_value > 0)
            result = Combat_ElementScore(element.y + atk_power, target_value);
    }
    if (result > 1.0)
        result = result / 100.0 + 1.0;
    return result;
}

// The element score of an attack against its target, each capped at 100.
int Game::Combat_ElementScore(int atk_power, int target_value)
{
    if (atk_power > 100)
        atk_power = 100;
    if (target_value > 100)
        target_value = 100;
    int score = (atk_power + atk_power + target_value) / 3;
    return score / 3 + 15;
}
