// Part of DTSource. Copyright 2004-2017. David Adalsteinsson.
// see http://www.visualdatatools.com/DTSource/license.html for more information.

#ifndef DTSeriesRegion3D_Header
#define DTSeriesRegion3D_Header

// See DTSeries.h for further information.
#include "DTSeries.h"

struct DTRegion3D;

class DTSeriesRegion3D : public DTSeries
{
public:
    DTSeriesRegion3D() : DTSeries("Region3D") {}
    DTSeriesRegion3D(DTDataStorage &saveInto,string name) : DTSeries(saveInto,name,"Region3D") {}

    void Register() const;
    void Register(string) const;
    static void pregistered(void); // Debug call, lists all registered series of this type.
    static bool HasBeenRegistered(string);
    static DTSeriesRegion3D ByName(string);
    
    void Add(DTRegion3D v,double time);

    DTRegion3D Get(double time) const;
    DTRegion3D operator()(int index) const;

private:
    
};

// To read series by reference. 
extern void Read(const DTDataStorage &input,const std::string &name,DTSeriesRegion3D &toReturn);

#endif

