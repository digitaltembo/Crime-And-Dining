/****************************************************************************************
 * Restauraunt.cpp                                                                      *
 *                                                                                      *
 * Nolan Hawkins                                                                        *
 *                                                                                      *
 * Desribing a simple class Restauraunt to hold name, address, and description of a     *
 * restauraunt in addition to it's position and a list of the crimes commited           *
 * nearby.                                                                              *
 ****************************************************************************************/

#include "Restauraunt.h"

using namespace std;

Restauraunt::Restauraunt(){
    crimeCost = 0;
}

// return true if the location has been set
bool Restauraunt::locationSet(){
    return latLng.isSet();
}

// sets the location to a given location, updating the metric location as well
void Restauraunt::setLocation(Location& l){
    latLng.setLocation(l);
    metricLocation.setLocation((latLng.x - MIN_LAT)*LAT_TO_METERS , 
                               (latLng.y - MIN_LNG)*LNG_TO_METERS);
}

// adds a crime to the list of crimes stored in the vector crimes,
// and updates the crimeCost with a calculation based on time, distance to
// the crime, and crime type (with weapons and shootings
void Restauraunt::addCrime(struct Crime* c, const Location& crimeLoc, int initialCost, const Date& now){
    crimeCost += finalCrimeCost(c->date, date, now, crimeLoc, metricLocation, initialCost);
    crimes.push_back(c);
    c->copies ++;
}

//input from CSV
istream& operator>>(istream  &input, Restauraunt& r){
    // Each input.getline gets another cell from the CSV table. The column
    // the cell belongs to is shown in the comments
    char buff[128];
    input.getline(buff, 128, ','); // BusinessName
    if(input.eof())
        return input;
    r.name = string(buff);
    input.getline(buff, 128, ','); // DBAName, whatever that means
    input.getline(buff, 128, ','); // Address
    r.address = string(buff) + ' ';
    input.getline(buff, 128, ','); // City
    r.address += string(buff) + ", ";
    input.getline(buff, 128, ','); // State
    r.address += string(buff) + ", ";
    input.getline(buff, 128, ','); // Zip
    r.address += string(buff);
    input.getline(buff, 128, ','); // LICSTATUS
    input.getline(buff, 128, ','); // LICENSECAT
    input.getline(buff, 128, ','); // description
    r.description = string(buff);
    input.getline(buff, 128, ','); // date
    r.date.setDate(buff);
    input.getline(buff, 128, ','); // phone
    input.getline(buff, 128, ','); // property id
    input.getline(buff, 128); // location
    r.latLng.setLocation(buff);
    if(r.latLng.x != 0)
        r.metricLocation.setLocation((r.latLng.x - MIN_LAT)*LAT_TO_METERS , 
                                     (r.latLng.y - MIN_LNG)*LNG_TO_METERS);
    return input;
}   

// C++ would be nicer with some good built in string replacement
// that replaced all instances of a string.
// This code was taken from a StackOverflow answer, but I cannot
// seem to find it again
string stringReplace(string s, string query, string replacement){
    size_t index = 0;
    size_t length = query.length();
    while (true) {
        /* Locate the substring to replace. */
        index = s.find(query, index);
        if (index == string::npos) break;

        /* Make the replacement. */
        s.replace(index, length, replacement);

        /* Advance index forward so the next iteration doesn't pick it up as well. */
        index += 3;
    }
    return s;
}

//output as CSV with header "Location, Name, Date, Address, Description, CrimeCost
ostream& operator<<(ostream  &output, Restauraunt& r){
    output <<'"'<< r.latLng << "\", \"" << stringReplace(r.name, "\"", "\\\"")
           << "\", " << r.date << ", \"" << stringReplace(r.address , "\"", "\\\"")
           << "\", \"" << stringReplace(r.description, "\"", "\\\"") 
           << "\", " << r.crimeCost << ", ";
    for(struct Crime* c : r.crimes){
        output << '|' << c->date <<'~' << int(c->type) << '~' << int(c->weapon);
    }
    output << endl;

    return output;
}