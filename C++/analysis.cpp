/****************************************************************************************
 * analysis.cpp                                                                         *
 *                                                                                      *
 * Nolan Hawkins, April 2015                                                            *
 *                                                                                      *
 * This is the main program that does analysis upon two files from                      *
 * data.cityofboston.gov: the active food establishment licenses database and the crime *
 * incident reports database. These files were downloaded and a python script run upon  *
 * them that filled out missing location data for the restauraunts by generating a      *
 * JSON file with keys of addresses and values of latitude/longitude location.          *
 *                                                                                      *
 * The analysis done basically builds what I believe to be a QuadTree (I have never     *
 * previously encountered that data structure) to hold every restauraunt at a specific  *
 * location in metric coordinates (that is, in meters east and meters north of the      *
 * westmost and southmost location). Then, for every crime it uses the QuadTree to find *
 * every restauraunt within 100 meters, and adds to the "crime cost" of that            *
 * restauraunt a value calculated from the type of crime, the weapons involved, and how *
 * recent the crime was (and if it took place before the restauraunt was established).  *
 * Then a CSV of only the crime location, type, and danger was ouputed so as to make    *
 * the crime CSV smaller, and a CSV of relevent information about each restauraunt was  *
 * generated: a few facts about the restauraunt, its total "crime cost" and a list of   *
 * crimes that took place within 100 meters of it. The total "crime cost", though by no *
 * means scientific, is what I believe to be a good heuristic to determine how safe a   *
 * restauraunt is.                                                                      *
 *                                                                                      *
 * Both of the CSV files were then uploaded to Google Fusion Tables in ordder to easily *
 * use them with the Google Maps API                                                    *
 *                                                                                      *
 * Compile with                                                                         *
 *       g++ -g -std=c++11 -o analyze *.cpp                                             *
 * Run with                                                                             *
 *      ./analyze                                                                       *
 *                                                                                      *
 * If using a different version of Crime_Incident_Reports.csv, remember that            *
 * for MedAssist reports not to be counted, it is necessary to update the Crime.h       *
 * definition of MED_ASSIST to be the value outputted at the very end.                  *
 ****************************************************************************************/


#include <vector>
#include <iostream>
#include <fstream>
#include <unordered_map>

#include "Location.h"
#include "Date.h"
#include "Crime.h"
#include "Restauraunt.h"
#include "QuadTree.hpp"

// These describe the locations of the CSV files downloaded from data.cityofboston.gov
#define FOOD_FILE "../data/Active_Food_Establishment_Licenses.csv"
#define CRIME_FILE "../data/Crime_Incident_Reports.csv"

// locs.json was a file generated by a python script that went through all of the restauraunts
// and if the location was not set, used Googles Geocoding API to determine the latitude and
// longitude from the address
#define LOC_FILE "../data/locs.json"

// These are the output files: relevent restauraunt info and crime info parsed from the 
// city of Boston data
#define FOOD_OUT "../data/Food.csv"
#define CRIME_OUT "../data/Crime.csv"

using namespace std;

// Originally, I was wokring entirely in python, so as mentioned in the description for LOCS_FILE,
// I just outputted a JSON file which I then converted to a dict in two lines. C++ made this a 
// little more difficult, and what follows simply transforms the JSON of the LOC_FILE into
// an unordered map for easy access, and returns a pointer to that map 
unordered_map<string, Location>* parseLocFile(){
    ifstream in(LOC_FILE);
    unordered_map<string, Location>* map = new unordered_map<string, Location>();
    in.get();
    in.get();
    char* key = new char[100];
    char* num = new char[20];
    int keyIndex = 0, numIndex = 0;
    Location loc = {0,0};
    char c ;
    bool readingKey=true;
    while(c = in.get()){
        if(c == '}')
            break;
        if(readingKey){
            if(c == '"'){
                key[keyIndex] = '\0';
                in.get(); // ':'
                in.get(); // '['
                in.get(); // ' '
                readingKey = false;
                keyIndex = 0;
            }else{
                key[keyIndex] = c;
                keyIndex++;
            }
        }else{
            if(c == ','){
                num[numIndex] = '\0';
                loc.x = stof(string(num));
                numIndex = 0;
                in.get(); // ' '
            }else if(c == ']'){
                num[numIndex] = '\0';
                loc.y = stof(string(num));
                map->emplace(string(key), loc);
                keyIndex = 0;
                numIndex = 0;
                if(in.get() == '}'){
                    break;
                }
                in.get(); // ','
                in.get(); // '\n'
                readingKey = true;
            }else{
                num[numIndex] = c;
                numIndex++;
            }
        }
    }
    delete[] key;
    delete[] num;
    return map;
}

// Then, as all the restauraunts that lack a location are stored in an unordered map, this
// function easily and quickly returns the location for a given restauraunt's address:

Location& getLocationFromAddress(unordered_map<string, Location>* addresses, string address){
    Location& l=(*addresses)[address];
    return (*addresses)[address];
}




// This function maps over the QuadTree
// This outputs an individual restauraunt
void writeRestauraunt(Restauraunt* r, void* cl){
    ofstream* out = (ofstream*)cl;
    (*out) << (*r);
}

// This should work, but it seems results in a double 
// free, so I guess not? I don't care too much about memory management
// for this project, though.
void deleteRestauraunt(Restauraunt* r, void* cl){
    delete r;
}


int main(){
    
    unordered_map<string, Location>* addresses = parseLocFile();

    // builds the QuadTree! and reads in all of the restauraunts.
    // I constructed the restauraunt class so as to simply use the >> operator
    // to read a line from the CSV file
    Restauraunt* r = new Restauraunt;
    ifstream foodFile(FOOD_FILE);
    foodFile.ignore(1000, '\n'); // Ignore first line
    QuadTree<Restauraunt> quad;
    while(!(foodFile >> (*r)).eof()){
        // If the location wasn't set, use the address to find the location
        if(!r->locationSet()){
            r->setLocation(getLocationFromAddress(addresses, r->address));
        }
        // Which means that now the metric location of the restauraunt is known, 
        // and can be inserted into the QuadTree
        quad.insert(r->metricLocation, r);
        r = new Restauraunt;
    }
    delete r;
    foodFile.close();
    
    /* Data is stored in crime csv as:
     * COMPNOS,NatureCode,INCIDENT_TYPE_DESCRIPTION,MAIN_CRIMECODE,REPTDISTRICT,
     * REPORTINGAREA,FROMDATE,WEAPONTYPE,Shooting,DOMESTIC,
     * SHIFT,Year,Month,DAY_WEEK,UCRPART,
     * X,Y,STREETNAME,XSTREETNAME,Location
     * 
     * We want INCIDENT_TYPE_DESCRIPTION [2], FROMDATE [6], WEAPONTYPE [7], Shooting [8], and Location [19]
     */
    
    ifstream crimeFile(CRIME_FILE);
    ofstream crimeOut (CRIME_OUT);
    // Output the crime header
    crimeOut << "Location, Date, Type, Danger\n";
    crimeFile.ignore(1000, '\n'); // Ignore first line
    
    char buff[128];
    
    // There were only like 30 destinct incident types, not all of which I understood, so I just assigned
    // each a numerical value. This unordered_map is how: if an incident was not in incidentTypes, set
    // the value to the incidentCount and store that in incidentTypes, and increment incidentCount
    unordered_map<string, unsigned char> incidentTypes;
    unsigned char incidentCount = 0;
    string incidentType;
    
    unordered_map<string, unsigned char>::const_iterator iter;
    // m stores metric location, l stores latitude/longitude
    Location m,l;
    
    int i=0;
    
    // I realized after a bit that I would want a date representing now to determine how long ago things
    // happened, but I didn't want too create a new date for every restauraunt or crime, so last minute
    // I added this variable
    Date now = Date::now();
    vector<struct Crime*> crimes;
    
    // I decided for the crimes, ad I wanted to keep tack of incident types
    // and the like, that rather than doing stream operators I would just do it all
    // here. It doesn't make or the cleanes code, b
    while(!crimeFile.eof()){
        struct Crime* c = new struct Crime;
        c->copies = 0;
        // This loops through the 20 cells of information in the CSV and extracts
        // the relevent bits
        for(int i=0;i<19;i++){
            crimeFile.getline(buff, 128, ',');
            
            if(crimeFile.eof())
                break;
            switch(i){
                case 2:// INCIDENT_TYPE_DESCRIPTION
                    ;
                    incidentType = string(buff); 
                    iter = incidentTypes.find(incidentType);
                    if(iter == incidentTypes.end()){
                        // Then the incidentType isn't in incidentTypes
                        incidentTypes.emplace(incidentType, incidentCount);
                        c->type = incidentCount;
                        incidentCount++;
                    }else{
                        c->type = iter->second;
                    }
                    break;
                case 6: // FROMDATE
                    c->date.setDate(buff);
                    break;
                case 7: // WEAPONTYPE (either Unarmed, Other, Knife, or Firearm)
                    switch(buff[0]){
                        case 'U': // Unarmed
                            c->weapon = 0;
                            break;
                        case 'O': // Other
                            c->weapon = 1;
                            break;
                        case 'K': // Knife
                            c->weapon = 2;
                            break;
                        case 'F': // Firearm
                            c->weapon = 3;
                            break;
                        default:
                            break;
                    }
                    break;
                case 8: //Shooting (Yes or No)
                    
                    // If there was a shooting, add a shooting flag
                    if(buff[0] == 'Y'){ 
                        c->weapon += 4;
                    }
                    break;
                default:
                    break;
            }
        }   
        crimeFile.getline(buff, 128); //This is the location
        l.setLocation(buff);
        m.setLocation((l.x - MIN_LAT)*LAT_TO_METERS , 
                      (l.y - MIN_LNG)*LNG_TO_METERS);
        // Output the crime CSV
        crimeOut << '"' << l << "\", " << c->date << ", "
                 << int(c->type) << ", " << int(c->weapon) << endl;
        
        // the call to quad.findNodes(m, 100) finds all nodes in the QuadTree within
        // a distance of 100 from the location m, which is the metric coordinates of the crime
        vector<Restauraunt*> v = quad.findNodes(m, 100);
        
        // This bit iterates through and adds the crime to the objects of nearby restauraunts
        if(!v.empty()){
            int initialCost = initialCrimeCost(*c);
            // If the initial cost is 0, no need to add the crime, as that means it was ignorable?
            // Basically I just decided that a MedAssist incident probably shouldn't be counted,
            // although I don't actually kknow what that means, its frequency and name suggests
            // that perhaps the police were merely assisting with something of a medical nature.
            
            if(initialCost > 0){
                for(Restauraunt* r: v){
                    r->addCrime(c, m, initialCost, now);
                }
            }
        }
        // if c->copies = 0, then it wasn't within 100 meters of any restauraunt and should be deleted
        if(c->copies = 0)
            delete c;
        else
            crimes.push_back(c);
        i++;
        if(i%100 == 0)
            cout << i << " crimes processed\n";
    }
    crimeFile.close();
    crimeOut.close();
    cout << "MedAssist: " << (int)incidentTypes["MedAssist"] << endl;
    // This section outputs the food CSV nice and succinctly
    ofstream foodOut (FOOD_OUT);
    foodOut << "Location, Name, Date, Address, Description, CrimeCost, Crimes\n";
    quad.mapNodes(writeRestauraunt, &foodOut);
    
    for(struct Crime* crime : crimes){
        delete crime;
    }
    
    delete addresses;
    
    // I should free all of the restauraunts in the quad, but this doesn't seem
    // to work and I don't really care that much for a one-off script:
    // quad.mapNodes(deleteRestauraunt, NULL);
    
}