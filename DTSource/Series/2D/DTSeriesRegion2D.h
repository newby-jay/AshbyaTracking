// Part of DTSource. Copyright 2004-2017. David Adalsteinsson.
// see http://www.visualdatatools.com/DTSource/license.html for more information.

#ifndef DTSeriesRegion2D_Header
#define DTSeriesRegion2D_Header

// See DTSeries.h for further information.
#include "DTSeries.h"

struct DTRegion2D;

class DTSeriesRegion2D : public DTSeries
{
public:
    DTSeriesRegion2D() : DTSeries("Region2D") {}
    DTSeriesRegion2D(DTDataStorage &saveInto,string name) : DTSeries(saveInto,name,"Region2D") {}

    void Register() const;
    void Register(string) const;
    static void pregistered(void); // Debug call, lists all registered series of this type.
    static bool HasBeenRegistered(string);
    static DTSeriesRegion2D ByName(string);
    
    void Add(DTRegion2D v,double time);

    DTRegion2D Get(double time) const;
    
private:
    
};

// To read series by reference. 
extern void Read(const DTDataStorage &input,const std::string &name,DTSeriesRegion2D &toReturn);

#endif

