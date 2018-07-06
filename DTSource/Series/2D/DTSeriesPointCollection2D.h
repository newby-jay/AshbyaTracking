// Part of DTSource. Copyright 2004-2015. David A. David Adalsteinsson.
// see http://www.visualdatatools.com/DTSource/license.html for more information.

#ifndef DTSeriesPointCollection2D_Header
#define DTSeriesPointCollection2D_Header

// See DTSeries.h for further information.
#include "DTSeries.h"

class DTPointCollection2D;

class DTSeriesPointCollection2D : public DTSeries
{
public:
    DTSeriesPointCollection2D() : DTSeries("PointCollection2D") {}
    DTSeriesPointCollection2D(DTDataStorage &saveInto,string name) : DTSeries(saveInto,name,"PointCollection2D") {}

    void Register() const;
    void Register(string) const;
    static void pregistered(void); // Debug call, lists all registered series of this type.
    static bool HasBeenRegistered(string);
    static DTSeriesPointCollection2D ByName(string);
    
    void Add(DTPointCollection2D v,double time);

    DTPointCollection2D Get(double time) const;
    
private:

};

// To read series by reference. 
extern void Read(const DTDataStorage &input,const std::string &name,DTSeriesPointCollection2D &toReturn);

#endif

