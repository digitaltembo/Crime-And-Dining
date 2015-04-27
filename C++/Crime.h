#ifndef CRIME
#define CRIME

#include "Location.h"
#include "Date.h"
#include <cmath>

#include <iostream>

#define SHOOTING_COST 5
#define FIREARM_COST  3
#define WEAPON_COST   2


#define SHOOTING_FLAG 4
#define WEAPON_FLAG 3

#define MED_ASSIST 10

struct Crime{
    unsigned char type;
    unsigned char weapon;
    Date date;
    int copies;
};
int initialCrimeCost(const struct Crime& c);
int finalCrimeCost(const Date& crimeDate, const Date& establishmentDate, const Date& now,
                   const Location& crimeLoc, const Location& establishmentLoc,
                   int initalCost
                  );

#endif