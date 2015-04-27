/******************************************************************************
 * Date.h                                                                     *
 *                                                                            *
 * Nolan Hawkins, April 2015                                                  *
 *                                                                            *
 * A simple date class to hold the month, day, and year, with overloaded      *
 * operators for determining the most recent date and the (rather approximate,*
 * overlooking leap years and the fact that not every month actually has      *
 * 30.4 days) difference in days between 2 dates.                             *
 ******************************************************************************/

#ifndef DATE
#define DATE

#include <cstdio>
#include <ostream>

class Date{
public:
    Date();
    Date(int month, int day, int year);
    // parse Date from string
    Date(char* s);
    // parse Date from string
    void setDate(char* s);
    
    // return a date representation of the current day
    static Date now();
    
    // date a is > date b if b occured before a
    friend bool operator>(const Date& a, const Date& b);
    friend bool operator<(const Date& a, const Date& b);
    
    //returns approximate difference in days
    friend int operator-(const Date& a, const Date& b);
    
    // ouputs date to stream in the form of  mm/dd/yyyy
    friend std::ostream &operator<<(std::ostream  &output, const Date& d);
        
    // the actual data, contrived to fit in 4 bytes, hopefully
    unsigned char month, day;
    short year;
};

#endif