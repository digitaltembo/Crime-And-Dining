/******************************************************************************
 * Date.cpp                                                                   *
 *                                                                            *
 * Nolan Hawkins, April 2015                                                  *
 *                                                                            *
 * A simple date class to hold the month, day, and year, with overloaded      *
 * operators for determining the most recent date and the (rather approximate,*
 * overlooking leap years and the fact that not every month actually has      *
 * 30.4 days) difference in days between 2 dates.                             *
 ******************************************************************************/

#include "Date.h"

Date::Date(){
    month = 0;
    day   = 0;
    year  = 0;
}

Date::Date(int month, int day, int year){
    this->month = (unsigned char)month;
    this->day   = (unsigned char)day;
    this->year  = (short)year;
}

// parse Date from string
Date::Date(char* s){
    sscanf(s, "%hhu/%hhu/%hd", &month, &day, &year);
}

void Date::setDate(char* s){
    sscanf(s, "%hhu/%hhu/%hd", &month, &day, &year);
}

// return a date representation of the current day
Date Date::now(){
    time_t now = time(0);
    tm *t = gmtime(&now);
    return Date(t->tm_mon, t->tm_mday, t->tm_year+1900);
}

// date a is > date b if b occured before a
bool operator>(const Date& a, const Date& b){
    if(a.year < b.year)
        return false;
    if(a.year > b.year)
        return true;
    if(a.month < b.month)
        return false;
    if(a.month > b.month)
        return true;
    if(a.day > b.day)
        return true;
    return false;
}
bool operator<(const Date& b, const Date& a){
    if(a.year < b.year)
        return false;
    if(a.year > b.year)
        return true;
    if(a.month < b.month)
        return false;
    if(a.month > b.month)
        return true;
    if(a.day > b.day)
        return true;
    return false;
}

//returns approximate difference in days
int operator-(const Date& a,const Date& b){
    return (int)((a.year-b.year)*365.25 + (a.month-b.month)*30.4 + (a.day-b.day));
}

// ouputs date to stream in the form of  mm/dd/yyyy
std::ostream &operator<<(std::ostream  &output, const Date& date){
    output << int(date.month) << '/' << int(date.day) << '/' << date.year;
    return output;
}