// Part of DTSource. Copyright 2004-2017. David Adalsteinsson.
// see http://www.visualdatatools.com/DTSource/license.html for more information.

#ifndef DTSeriesSurfaceValues3D_Header
#define DTSeriesSurfaceValues3D_Header

// See DTSeries.h for further information.
#include "DTSeries.h"
#include "DTSurface3D.h"

class DTSurfaceValues3D;

class DTSeriesSurfaceValues3D : public DTSeries
{
public:
    DTSeriesSurfaceValues3D() : DTSeries("Surface3DValues") {}
    DTSeriesSurfaceValues3D(DTDataStorage &saveInto,string name) : DTSeries(saveInto,name,"Surface3DValues") {}
    
    void Register() const;
    void Register(string) const;
    static void pregistered(void); // Debug call, lists all registered series of this type.
    static bool HasBeenRegistered(string);
    static DTSeriesSurfaceValues3D ByName(string);
    
    void Add(DTSurfaceValues3D v,double time);
    
    DTSurfaceValues3D Get(double time) const;
    DTSurfaceValues3D operator()(int index) const;

private:
    mutable DTSurface3D_SaveInfo saveInfo;
};

#endif

