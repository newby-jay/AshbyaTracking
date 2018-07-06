// Part of DTSource. Copyright 2004-2017. David Adalsteinsson.
// see http://www.visualdatatools.com/DTSource/license.html for more information.

#include "DTSeriesSurfaceCoordinates3D.h"

#include "DTSurfaceCoordinates3D.h"
#include "DTSeriesTemplates.h"

map<string,DTSeriesSurfaceCoordinates3D> DTGlobalSeriesSurfaceCoordinates3D;

void DTSeriesSurfaceCoordinates3D::Register() const
{
    Register(Name());
}

void DTSeriesSurfaceCoordinates3D::Register(string name) const
{
    DTRegisteredSeries<DTSeriesSurfaceCoordinates3D,map<string,DTSeriesSurfaceCoordinates3D>::const_iterator>(DTGlobalSeriesSurfaceCoordinates3D,name,"DTSeriesSurfaceCoordinates3D",this);
}

void DTSeriesSurfaceCoordinates3D::pregistered(void)
{
    DTPrintRegisteredSeries<DTSeriesSurfaceCoordinates3D,map<string,DTSeriesSurfaceCoordinates3D>::const_iterator>(DTGlobalSeriesSurfaceCoordinates3D);
}

bool DTSeriesSurfaceCoordinates3D::HasBeenRegistered(string name)
{
    return DTHasRegisteredSeries<DTSeriesSurfaceCoordinates3D,map<string,DTSeriesSurfaceCoordinates3D>::const_iterator>(DTGlobalSeriesSurfaceCoordinates3D,name);
}

DTSeriesSurfaceCoordinates3D DTSeriesSurfaceCoordinates3D::ByName(string name)
{
    return DTFindRegisteredSeries<DTSeriesSurfaceCoordinates3D,map<string,DTSeriesSurfaceCoordinates3D>::const_iterator>(DTGlobalSeriesSurfaceCoordinates3D,name,"DTSeriesSurfaceCoordinates3D");
}

void DTSeriesSurfaceCoordinates3D::Add(DTSurfaceCoordinates3D v,double time)
{
    if (SharedSave(time)==false) return;
    
    AddToBBox(BoundingBox(v));
    Write(DataFile(),baseName(),v,saveInfo);
    DataFile().Flush();
}

DTSurfaceCoordinates3D DTSeriesSurfaceCoordinates3D::Get(double time) const
{
    int k = IndexForTime(time);
    if (k<0) return DTSurfaceCoordinates3D();
    DTSurfaceCoordinates3D toReturn;
    Read(DataFile(),baseName(k),toReturn,saveInfo);
    return toReturn;
}

DTSurfaceCoordinates3D DTSeriesSurfaceCoordinates3D::operator()(int index) const
{
    if (index<0 || index>=HowManySaved()) {
        DTErrorOutOfRange("DTSeriesSurfaceCoordinates3D(i)",index,HowManySaved());
        return DTSurfaceCoordinates3D();
    }
    DTSurfaceCoordinates3D toReturn;
    Read(DataFile(),baseName(index),toReturn);
    return toReturn;
}

