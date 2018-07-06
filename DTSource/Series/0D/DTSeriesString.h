// Part of DTSource. Copyright 2004-2017. David Adalsteinsson.
// see http://www.visualdatatools.com/DTSource/license.html for more information.

#ifndef DTSeriesString_Header
#define DTSeriesString_Header

// See DTSeries.h for further information.
#include "DTSeries.h"

class DTSeriesString : public DTSeries
{
public:
    DTSeriesString() : DTSeries("String") {}
    DTSeriesString(DTDataStorage &saveInto,string name) : DTSeries(saveInto,name,"String") {}

    void Register() const;
    void Register(string) const;
    static void pregistered(void); // Debug call, lists all registered series of this type.
    static bool HasBeenRegistered(string);
    static DTSeriesString ByName(string);
    
    void Add(string v,double time);
    std::string Get(double time) const;

private:
    
};

// To read series by reference. 
extern void Read(const DTDataStorage &input,const std::string &name,DTSeriesString &toReturn);

#endif

