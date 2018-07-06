// Part of DTSource. Copyright 2004-2017. David Adalsteinsson.
// see http://www.visualdatatools.com/DTSource/license.html for more information.

#ifndef DTSeriesSurfaceTexture3D_Header
#define DTSeriesSurfaceTexture3D_Header

// See DTSeries.h for further information.
#include "DTSeries.h"
#include "DTSurfaceCoordinates3D.h"

class DTSurfaceTexture3D;

class DTSeriesSurfaceTexture3D : public DTSeries
{
public:
    DTSeriesSurfaceTexture3D() : DTSeries("Surface3DTexture") {}
    DTSeriesSurfaceTexture3D(DTDataStorage &saveInto,string name) : DTSeries(saveInto,name,"Surface3DTexture") {}
    
    void Register() const;
    void Register(string) const;
    static void pregistered(void); // Debug call, lists all registered series of this type.
    static bool HasBeenRegistered(string);
    static DTSeriesSurfaceTexture3D ByName(string);
    
    void Add(DTSurfaceTexture3D v,double time);
    
    DTSurfaceTexture3D Get(double time) const;
    DTSurfaceTexture3D operator()(int index) const;

private:
    mutable DTSurfaceCoordinates3D_SaveInfo saveInfo;
};

#endif

