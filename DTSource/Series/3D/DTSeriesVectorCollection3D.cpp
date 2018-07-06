// Part of DTSource. Copyright 2004-2015. David A. David Adalsteinsson.
// see http://www.visualdatatools.com/DTSource/license.html for more information.

#include "DTSeriesVectorCollection3D.h"

#include "DTVectorCollection3D.h"
#include "DTRegion3D.h"
#include "DTSeriesTemplates.h"

map<string,DTSeriesVectorCollection3D> DTGlobalSeriesVectorCollection3D;

void DTSeriesVectorCollection3D::Register() const
{
    Register(Name());
}

void DTSeriesVectorCollection3D::Register(string name) const
{
    DTRegisteredSeries<DTSeriesVectorCollection3D,map<string,DTSeriesVectorCollection3D>::const_iterator>(DTGlobalSeriesVectorCollection3D,name,"DTSeriesVectorCollection3D",this);
}

void DTSeriesVectorCollection3D::pregistered(void)
{
    DTPrintRegisteredSeries<DTSeriesVectorCollection3D,map<string,DTSeriesVectorCollection3D>::const_iterator>(DTGlobalSeriesVectorCollection3D);
}

bool DTSeriesVectorCollection3D::HasBeenRegistered(string name)
{
    return DTHasRegisteredSeries<DTSeriesVectorCollection3D,map<string,DTSeriesVectorCollection3D>::const_iterator>(DTGlobalSeriesVectorCollection3D,name);
}

DTSeriesVectorCollection3D DTSeriesVectorCollection3D::ByName(string name)
{
    return DTFindRegisteredSeries<DTSeriesVectorCollection3D,map<string,DTSeriesVectorCollection3D>::const_iterator>(DTGlobalSeriesVectorCollection3D,name,"DTSeriesVectorCollection3D");
}

void DTSeriesVectorCollection3D::Add(DTVectorCollection3D v,double time)
{
    if (SharedSave(time)==false) return;
    AddToBBox(BoundingBox(v));
    Write(DataFile(),baseName(),v,saveInfo);
    DataFile().Flush();
}

DTVectorCollection3D DTSeriesVectorCollection3D::Get(double time) const
{
    int k = IndexForTime(time);
    if (k<0) return DTVectorCollection3D();
    DTVectorCollection3D toReturn;
    Read(DataFile(),baseName(k),toReturn,saveInfo);
    return toReturn;
}

DTVectorCollection3D DTSeriesVectorCollection3D::operator()(int index) const
{
    if (index<0 || index>=HowManySaved()) {
        DTErrorOutOfRange("DTSeriesVectorCollection3D(i)",index,HowManySaved());
        return DTVectorCollection3D();
    }
    DTVectorCollection3D toReturn;
    Read(DataFile(),baseName(index),toReturn);
    return toReturn;
}

