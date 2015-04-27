/******************************************************************************
 * Location.h                                                                 *
 *                                                                            *
 * Nolan Hawkins, April 2015                                                  *
 *                                                                            *
 * This is a simple location class, originally a struct but then a few        *
 * functions were added. Super simple and self explanatory, really.           *
 ******************************************************************************/

#ifndef LOCATION
#define LOCATION

#include <cstdio>
#include <ostream>

class Location{
public:
    double x, y;
    
    Location();
    // These constructors just call the respective setLocation fucntions
    Location(double x, double y);
    Location(char* s);
    
    // these set the location
    
    // to another location
    void setLocation(const Location& l);
    // to specified coordinates
    void setLocation(double x, double y);
    // or to a parsing of a string in the form of "###, ###" 
    void setLocation(char* s);
    
    
    // returns the (delta X)^2 + (delta Y)^2
    // as the distances are merely compared, the square root necessary
    // to make this an actual distance function is unnecessaryy
    double distSquared(const Location& l);
    
    // returns true if the location has been set
    bool isSet();
    
    
    // outputs the location to an output stream
    friend std::ostream &operator<<(std::ostream  &output, Location& l);

};

#endif