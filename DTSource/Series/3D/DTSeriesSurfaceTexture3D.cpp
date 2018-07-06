// Part of DTSource. Copyright 2004-2017. David Adalsteinsson.
// see http://www.visualdatatools.com/DTSource/license.html for more information.

#include "DTSeriesSurfaceTexture3D.h"

#include "DTSurfaceTexture3D.h"
#include "DTSeriesTemplates.h"

map<string,DTSeriesSurfaceTexture3D> DTGlobalSeriesSurfaceTexture3D;

void DTSeriesSurfaceTexture3D::Register() const
{
    Register(Name());
}

void DTSeriesSurfaceTexture3D::Register(string name) const
{
    DTRegisteredSeries<DTSeriesSurfaceTexture3D,map<string,DTSeriesSurfaceTexture3D>::const_iterator>(DTGlobalSeriesSurfaceTexture3D,name,"DTSeriesSurfaceTexture3D",this);
}

void DTSeriesSurfaceTexture3D::pregistered(void)
{
    DTPrintRegisteredSeries<DTSeriesSurfaceTexture3D,map<string,DTSeriesSurfaceTexture3D>::const_iterator>(DTGlobalSeriesSurfaceTexture3D);
}

bool DTSeriesSurfaceTexture3D::HasBeenRegistered(string name)
{
    return DTHasRegisteredSeries<DTSeriesSurfaceTexture3D,map<string,DTSeriesSurfaceTexture3D>::const_iterator>(DTGlobalSeriesSurfaceTexture3D,name);
}

DTSeriesSurfaceTexture3D DTSeriesSurfaceTexture3D::ByName(string name)
{
    return DTFindRegisteredSeries<DTSeriesSurfaceTexture3D,map<string,DTSeriesSurfaceTexture3D>::const_iterator>(DTGlobalSeriesSurfaceTexture3D,name,"DTSeriesSurfaceTexture3D");
}

void DTSeriesSurfaceTexture3D::Add(DTSurfaceTexture3D v,double time)
{
    if (SharedSave(time)==false) return;
    
    AddToBBox(BoundingBox(v));
    Write(DataFile(),baseName(),v,saveInfo);
    DataFile().Flush();
}

DTSurfaceTexture3D DTSeriesSurfaceTexture3D::Get(double time) const
{
    int k = IndexForTime(time);
    if (k<0) return DTSurfaceTexture3D();
    DTSurfaceTexture3D toReturn;
    Read(DataFile(),baseName(k),toReturn,saveInfo);
    return toReturn;
}

DTSurfaceTexture3D DTSeriesSurfaceTexture3D::operator()(int index) const
{
    if (index<0 || index>=HowManySaved()) {
        DTErrorOutOfRange("DTSeriesSurfaceTexture3D(i)",index,HowManySaved());
        return DTSurfaceTexture3D();
    }
    DTSurfaceTexture3D toReturn;
    Read(DataFile(),baseName(index),toReturn);
    return toReturn;
}

