#include "Crime.h"

using namespace std;

// initialCrimeCost is determined per crime and is a rough heuristic of the danger of the crime,
// 0 being safe and higher numbers being more dangerous
int initialCrimeCost(const struct Crime& c){
    // c.type represents the crime type in integer form, but this depends on what file
    // is read in, as the integer is merely the order in which an incident showed up in
    // In my Crime CSV, MedAssist showed up 10th, and I want to ignore MedAssist, so I
    // used this
    if(c.type == MED_ASSIST){ // MED_ASSIST is def'd to 10
        return 0;
    }
    
    // Otherwise, the initial cost is determined by whether there was a shooting and
    // the weapons involved
    
    // The constants SHOOTING_COST, WEAPON_COST, and FIREARM_COST are 5, 2, and 3 respectively,
    // are defined in Crime.h, and are significant in calculation of the "Danger" of a crime
    int cost = 1;
    if(c.weapon & SHOOTING_FLAG){
        cost*=SHOOTING_COST;
    }
    switch(c.weapon & WEAPON_FLAG){
        case 0://unarmed
            break;
        case 1:case 2: // Knife or Other
            cost*=WEAPON_COST;
            break;
        case 3:
            cost*=FIREARM_COST;
    }
    return cost;
}

// finalCrimeCost is determined by both a crime and a restauraunt. It could potentially use how
// close the crime was to the restauraunt, but I decided to ignore that as all crimes
// I look at occur within 100 meters of the restauraunt in question.
// Instead, it uses the initial crime cost and a rough heuristic counting more
// recent crimes as higher and crimes that occured before the restauraunt was established
// as lower.
int finalCrimeCost(const Date& crimeDate, const Date& establishmentDate, const Date& now,
                   const Location& crimeLoc, const Location& establishmentLoc,
                   int initialCost
                  ){
    if(initialCost == 0)
        return 0;
    if(establishmentDate < crimeDate){
        int daysSinceCrime = now - establishmentDate;
        initialCost = (int)(initialCost*(2/(1 + daysSinceCrime/50.0) + 1));
        // No space penalty for now
        return initialCost;
    }
    return (int)(initialCost/ceil(((establishmentDate - crimeDate)+1.0)/365));
}