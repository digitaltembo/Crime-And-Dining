/******************************************************************************
 * Location.cpp                                                                *
 *                                                                            *
 * Nolan Hawkins, April 2015                                                  *
 *                                                                            *
 * This is a simple location class, originally a struct but then a few        *
 * functions were added. Super simple and self explanatory, really.           *
 ******************************************************************************/

#include "Location.h"

// Constructors:
Location::Location(){
    x = y = 0;
}

Location::Location(double x, double y){
    setLocation(x, y);
}


Location::Location(char* s){
    setLocation(s);
}


// Assignments:
void Location::setLocation(const Location& l){
    x = l.x;
    y = l.y;
}

void Location::setLocation(double x, double y){
    this->x = x;
    this->y = y;
}

// parses location from a string in the form of "###, ###" (with the quotations)
void Location::setLocation(char* s){
    sscanf(s, "\"(%lf, %lf)\"", &x, &y);
}  

// returns the (delta X)^2 + (delta Y)^2
// as the distances are merely compared, the square root necessary
// to make this an actual distance function is unnecessaryy
double Location::distSquared(const Location& l){
    return (l.x-x)*(l.x-x) + (l.y-y)*(l.y-y);
}

// returns true if the location has been set
bool Location::isSet(){
    return x != 0;
}

// outputs the location to an output stream
std::ostream& operator<<(std::ostream  &output, Location& l){
    output << l.x << ", " << l.y ;
    return output;
}