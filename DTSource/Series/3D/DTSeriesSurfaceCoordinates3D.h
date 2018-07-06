// Part of DTSource. Copyright 2004-2017. David Adalsteinsson.
// see http://www.visualdatatools.com/DTSource/license.html for more information.

#ifndef DTSeriesSurfaceCoordinates3D_Header
#define DTSeriesSurfaceCoordinates3D_Header

// See DTSeries.h for further information.
#include "DTSeries.h"
#include "DTSurfaceCoordinates3D.h"

class DTSeriesSurfaceCoordinates3D : public DTSeries
{
public:
    DTSeriesSurfaceCoordinates3D() : DTSeries("Surface3DCoordinates") {}
    DTSeriesSurfaceCoordinates3D(DTDataStorage &saveInto,string name) : DTSeries(saveInto,name,"Surface3DCoordinates") {}
    
    void Register() const;
    void Register(string) const;
    static void pregistered(void); // Debug call, lists all registered series of this type.
    static bool HasBeenRegistered(string);
    static DTSeriesSurfaceCoordinates3D ByName(string);
    
    void Add(DTSurfaceCoordinates3D v,double time);
    
    DTSurfaceCoordinates3D Get(double time) const;
    DTSurfaceCoordinates3D operator()(int index) const;

private:
    mutable DTSurfaceCoordinates3D_SaveInfo saveInfo;
};

#endif

