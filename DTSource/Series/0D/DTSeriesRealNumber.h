// Part of DTSource. Copyright 2004-2015. David A. David Adalsteinsson.
// see http://www.visualdatatools.com/DTSource/license.html for more information.

#ifndef DTSeriesRealNumber_Header
#define DTSeriesRealNumber_Header

// See DTSeries.h for further information.
#include "DTSeries.h"

class DTSeriesRealNumber : public DTSeries
{
public:
    DTSeriesRealNumber() : DTSeries("Real Number") {}
    DTSeriesRealNumber(DTDataStorage &saveInto,string name) : DTSeries(saveInto,name,"Real Number") {}

    void Register() const;
    void Register(string) const;
    static void pregistered(void); // Debug call, lists all registered series of this type.
    static bool HasBeenRegistered(string);
    static DTSeriesRealNumber ByName(string);
    
    void Add(double v,double time);

    double Get(double time) const;

private:

};

// To read series by reference. 
extern void Read(const DTDataStorage &input,const std::string &name,DTSeriesRealNumber &toReturn);

#endif

