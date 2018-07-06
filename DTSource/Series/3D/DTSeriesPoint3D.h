// Part of DTSource. Copyright 2004-2017. David Adalsteinsson.
// see http://www.visualdatatools.com/DTSource/license.html for more information.

#ifndef DTSeriesPoint3D_Header
#define DTSeriesPoint3D_Header

// See DTSeries.h for further information.
#include "DTSeries.h"

class DTPoint3D;

class DTSeriesPoint3D : public DTSeries
{
public:
    DTSeriesPoint3D() : DTSeries("Point3D") {}
    DTSeriesPoint3D(DTDataStorage &saveInto,string name) : DTSeries(saveInto,name,"Point3D") {}

    void Register() const;
    void Register(string) const;
    static void pregistered(void); // Debug call, lists all registered series of this type.
    static bool HasBeenRegistered(string);
    static DTSeriesPoint3D ByName(string);
    
    void Add(DTPoint3D v,double time);

    DTPoint3D Get(double time) const;
    DTPoint3D operator()(int index) const;

private:
    
};

// To read series by reference. 
extern void Read(const DTDataStorage &input,const std::string &name,DTSeriesPoint3D &toReturn);

#endif

