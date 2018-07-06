// Part of DTSource. Copyright 2004-2015. David A. David Adalsteinsson.
// see http://www.visualdatatools.com/DTSource/license.html for more information.

#ifndef DTSeriesVectorCollection3D_Header
#define DTSeriesVectorCollection3D_Header

// See DTSeries.h for further information.
#include "DTSeries.h"

#include "DTPointCollection3D.h"

class DTVectorCollection3D;

class DTSeriesVectorCollection3D : public DTSeries
{
public:
    DTSeriesVectorCollection3D() : DTSeries("VectorCollection3D") {}
    DTSeriesVectorCollection3D(DTDataStorage &saveInto,string name) : DTSeries(saveInto,name,"VectorCollection3D") {}
    
    void Register() const;
    void Register(string) const;
    static void pregistered(void); // Debug call, lists all registered series of this type.
    static bool HasBeenRegistered(string);
    static DTSeriesVectorCollection3D ByName(string);
    
    void Add(DTVectorCollection3D v,double time);
    
    DTVectorCollection3D Get(double time) const;
    DTVectorCollection3D operator()(int index) const;

private:
    mutable DTPointCollection3D_SaveInfo saveInfo;
};

#endif

