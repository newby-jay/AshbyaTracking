// Part of DTSource. Copyright 2004-2017. David Adalsteinsson.
// see http://www.visualdatatools.com/DTSource/license.html for more information.

#ifndef DTSeriesNetwork_Header
#define DTSeriesNetwork_Header

// See DTSeries.h for further information.
#include "DTSeries.h"

class DTNetwork;

class DTSeriesNetwork : public DTSeries
{
public:
    DTSeriesNetwork() : DTSeries("Network") {}
    DTSeriesNetwork(DTDataStorage &saveInto,string name) : DTSeries(saveInto,name,"Network") {}
    
    void Register() const;
    void Register(string) const;
    static void pregistered(void); // Debug call, lists all registered series of this type.
    static bool HasBeenRegistered(string);
    static DTSeriesNetwork ByName(string);
    
    void Add(DTNetwork v,double time);
    
    DTNetwork Get(double time) const;
    
private:
};

// To read series by reference.
extern void Read(const DTDataStorage &input,const std::string &name,DTSeriesNetwork &toReturn);

#endif

