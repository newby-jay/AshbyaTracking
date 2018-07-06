// Part of DTSource. Copyright 2004-2017. David Adalsteinsson.
// see http://www.visualdatatools.com/DTSource/license.html for more information.

#include "DTSeriesSurfaceValues3D.h"

#include "DTSurfaceValues3D.h"
#include "DTSeriesTemplates.h"

map<string,DTSeriesSurfaceValues3D> DTGlobalSeriesSurfaceValues3D;

void DTSeriesSurfaceValues3D::Register() const
{
    Register(Name());
}

void DTSeriesSurfaceValues3D::Register(string name) const
{
    DTRegisteredSeries<DTSeriesSurfaceValues3D,map<string,DTSeriesSurfaceValues3D>::const_iterator>(DTGlobalSeriesSurfaceValues3D,name,"DTSeriesSurfaceValues3D",this);
}

void DTSeriesSurfaceValues3D::pregistered(void)
{
    DTPrintRegisteredSeries<DTSeriesSurfaceValues3D,map<string,DTSeriesSurfaceValues3D>::const_iterator>(DTGlobalSeriesSurfaceValues3D);
}

bool DTSeriesSurfaceValues3D::HasBeenRegistered(string name)
{
    return DTHasRegisteredSeries<DTSeriesSurfaceValues3D,map<string,DTSeriesSurfaceValues3D>::const_iterator>(DTGlobalSeriesSurfaceValues3D,name);
}

DTSeriesSurfaceValues3D DTSeriesSurfaceValues3D::ByName(string name)
{
    return DTFindRegisteredSeries<DTSeriesSurfaceValues3D,map<string,DTSeriesSurfaceValues3D>::const_iterator>(DTGlobalSeriesSurfaceValues3D,name,"DTSeriesSurfaceValues3D");
}

void DTSeriesSurfaceValues3D::Add(DTSurfaceValues3D v,double time)
{
    if (SharedSave(time)==false) return;
    
    AddToBBox(BoundingBox(v));
    AddToValueRange(ValueRange(v));
    Write(DataFile(),baseName(),v,saveInfo);
    DataFile().Flush();
}

DTSurfaceValues3D DTSeriesSurfaceValues3D::Get(double time) const
{
    int k = IndexForTime(time);
    if (k<0) return DTSurfaceValues3D();
    DTSurfaceValues3D toReturn;
    Read(DataFile(),baseName(k),toReturn,saveInfo);
    return toReturn;
}

DTSurfaceValues3D DTSeriesSurfaceValues3D::operator()(int index) const
{
    if (index<0 || index>=HowManySaved()) {
        DTErrorOutOfRange("DTSeriesSurfaceValues3D(i)",index,HowManySaved());
        return DTSurfaceValues3D();
    }
    DTSurfaceValues3D toReturn;
    Read(DataFile(),baseName(index),toReturn);
    return toReturn;
}

