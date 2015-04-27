/****************************************************************************************
 * Restauraunt.h                                                                        *
 *                                                                                      *
 * Nolan Hawkins                                                                        *
 *                                                                                      *
 * Desribing a simple class Restauraunt to hold name, address, and description of a     *
 * restauraunt in addition to it's position and a list of the crimes commited           *
 * nearby.                                                                              *
 ****************************************************************************************/


#ifndef RESTAURAUNT
#define RESTAURAUNT

#include <iostream>
#include <fstream>
#include <vector>

#include "Location.h"
#include "Date.h"
#include "Crime.h"

#define MIN_LAT 42.237125
#define MAX_LAT 42.393484

#define MIN_LNG -71.17261
#define MAX_LNG -70.99673

#define LAT_TO_METERS 111080
#define LNG_TO_METERS 364437

class Restauraunt{
public:
    Restauraunt();
    
    // return true if the location has been set
    bool locationSet();
    
    // sets the location to a given location, updating the metric location as well
    void setLocation(Location& l);
    
    // adds a crime to the list of crimes stored in the vector crimes,
    // and updates the crimeCost with a calculation based on time, distance to
    // the crime, and crime type (with weapons and shootings
    void addCrime(struct Crime* c, const Location& l, int initialCost, const Date& now);
    
    
    // These are important functions, for both the reading in of a restauraunt from a 
    // line in the city of Boston's data on restauraunts (in .csv form), and outputting
    // a line of my own .csv
    friend std::istream &operator>>(std::istream  &input, Restauraunt& r);
    
    friend std::ostream &operator<<(std::ostream  &output, Restauraunt& r);
    
    // metricLocation is coordinates in meters from the minimum latitude and longitude
    // in the data, whereas latLng is simply the latitudinal/longitudinal coordinats
    Location latLng, metricLocation;
    std::string name, address, description;
    int crimeCost;
    std::vector<struct Crime*> crimes;
    Date date;
    
};

#endif