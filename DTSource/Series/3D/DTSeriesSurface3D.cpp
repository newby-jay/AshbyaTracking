// Part of DTSource. Copyright 2004-2017. David Adalsteinsson.
// see http://www.visualdatatools.com/DTSource/license.html for more information.

#include "DTSeriesSurface3D.h"

#include "DTSurface3D.h"
#include "DTSeriesTemplates.h"

map<string,DTSeriesSurface3D> DTGlobalSeriesSurface3D;

void DTSeriesSurface3D::Register() const
{
    Register(Name());
}

void DTSeriesSurface3D::Register(string name) const
{
    DTRegisteredSeries<DTSeriesSurface3D,map<string,DTSeriesSurface3D>::const_iterator>(DTGlobalSeriesSurface3D,name,"DTSeriesSurface3D",this);
}

void DTSeriesSurface3D::pregistered(void)
{
    DTPrintRegisteredSeries<DTSeriesSurface3D,map<string,DTSeriesSurface3D>::const_iterator>(DTGlobalSeriesSurface3D);
}

bool DTSeriesSurface3D::HasBeenRegistered(string name)
{
    return DTHasRegisteredSeries<DTSeriesSurface3D,map<string,DTSeriesSurface3D>::const_iterator>(DTGlobalSeriesSurface3D,name);
}

DTSeriesSurface3D DTSeriesSurface3D::ByName(string name)
{
    return DTFindRegisteredSeries<DTSeriesSurface3D,map<string,DTSeriesSurface3D>::const_iterator>(DTGlobalSeriesSurface3D,name,"DTSeriesSurface3D");
}

void DTSeriesSurface3D::Add(DTSurface3D v,double time)
{
    if (SharedSave(time)==false) return;

    AddToBBox(BoundingBox(v));
    if (_saveDoubleAsFloat)
        WriteFloat(DataFile(),baseName(),v);
    else
        Write(DataFile(),baseName(),v);
    DataFile().Flush();
}

DTSurface3D DTSeriesSurface3D::Get(double time) const
{
    int k = IndexForTime(time);
    if (k<0) return DTSurface3D();
    DTSurface3D toReturn;
    Read(DataFile(),baseName(k),toReturn);
    return toReturn;
}

DTSurface3D DTSeriesSurface3D::operator()(int index) const
{
    if (index<0 || index>=HowManySaved()) {
        DTErrorOutOfRange("DTSeriesSurface3D(i)",index,HowManySaved());
        return DTSurface3D();
    }
    DTSurface3D toReturn;
    Read(DataFile(),baseName(index),toReturn);
    return toReturn;
}

void Read(const DTDataStorage &input,const std::string &name,DTSeriesSurface3D &toReturn)
{
    string variableName;
    DTMutablePointer<DTDataStorage> storage = DTSeries::ReferencedDataFile(input,name,variableName);
    toReturn = DTSeriesSurface3D(*storage,variableName);
}
