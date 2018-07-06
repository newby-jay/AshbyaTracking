// Part of DTSource. Copyright 2004-2017. David Adalsteinsson.
// see http://www.visualdatatools.com/DTSource/license.html for more information.

#ifndef DTSeriesRegion1D_Header
#define DTSeriesRegion1D_Header

// See DTSeries.h for further information.
#include "DTSeries.h"

struct DTRegion1D;

class DTSeriesRegion1D : public DTSeries
{
public:
    DTSeriesRegion1D() : DTSeries("Range") {}
    DTSeriesRegion1D(DTDataStorage &saveInto,string name) : DTSeries(saveInto,name,"Range") {}

    void Register() const;
    void Register(string) const;
    static void pregistered(void); // Debug call, lists all registered series of this type.
    static bool HasBeenRegistered(string);
    static DTSeriesRegion1D ByName(string);
    
    void Add(DTRegion1D v,double time);

    DTRegion1D Get(double time) const;

private:

};

// To read series by reference. 
extern void Read(const DTDataStorage &input,const std::string &name,DTSeriesRegion1D &toReturn);

#endif

