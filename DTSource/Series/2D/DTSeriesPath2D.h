// Part of DTSource. Copyright 2004-2015. David A. David Adalsteinsson.
// see http://www.visualdatatools.com/DTSource/license.html for more information.

#ifndef DTSeriesPath2D_Header
#define DTSeriesPath2D_Header

// See DTSeries.h for further information.
#include "DTSeries.h"

class DTPath2D;

class DTSeriesPath2D : public DTSeries
{
public:
    DTSeriesPath2D() : DTSeries("Path2D") {}
    DTSeriesPath2D(DTDataStorage &saveInto,string name) : DTSeries(saveInto,name,"Path2D") {}

    void Register() const;
    void Register(string) const;
    static void pregistered(void); // Debug call, lists all registered series of this type.
    static bool HasBeenRegistered(string);
    static DTSeriesPath2D ByName(string);
    
    void Add(DTPath2D v,double time);

    DTPath2D Get(double time) const;
    
private:
};

// To read series by reference. 
extern void Read(const DTDataStorage &input,const std::string &name,DTSeriesPath2D &toReturn);

#endif

