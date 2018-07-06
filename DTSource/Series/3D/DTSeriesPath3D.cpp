// Part of DTSource. Copyright 2004-2017. David Adalsteinsson.
// see http://www.visualdatatools.com/DTSource/license.html for more information.

#include "DTSeriesPath3D.h"

#include "DTPath3D.h"
#include "DTSeriesTemplates.h"

map<string,DTSeriesPath3D> DTGlobalSeriesPath3D;

void DTSeriesPath3D::Register() const
{
    Register(Name());
}

void DTSeriesPath3D::Register(string name) const
{
    DTRegisteredSeries<DTSeriesPath3D,map<string,DTSeriesPath3D>::const_iterator>(DTGlobalSeriesPath3D,name,"DTSeriesPath3D",this);
}

void DTSeriesPath3D::pregistered(void)
{
    DTPrintRegisteredSeries<DTSeriesPath3D,map<string,DTSeriesPath3D>::const_iterator>(DTGlobalSeriesPath3D);
}

bool DTSeriesPath3D::HasBeenRegistered(string name)
{
    return DTHasRegisteredSeries<DTSeriesPath3D,map<string,DTSeriesPath3D>::const_iterator>(DTGlobalSeriesPath3D,name);
}

DTSeriesPath3D DTSeriesPath3D::ByName(string name)
{
    return DTFindRegisteredSeries<DTSeriesPath3D,map<string,DTSeriesPath3D>::const_iterator>(DTGlobalSeriesPath3D,name,"DTSeriesPath3D");
}

void DTSeriesPath3D::Add(DTPath3D v,double time)
{
    if (SharedSave(time)==false) return;

    if (_saveDoubleAsFloat)
        WriteFloat(DataFile(),baseName(),v);
    else
        Write(DataFile(),baseName(),v);

    AddToBBox(BoundingBox(v));
    DataFile().Flush();
}

DTPath3D DTSeriesPath3D::Get(double time) const
{
    int k = IndexForTime(time);
    if (k<0) return DTPath3D();
    DTPath3D toReturn;
    Read(DataFile(),baseName(k),toReturn);
    return toReturn;
}

DTPath3D DTSeriesPath3D::operator()(int index) const
{
    if (index<0 || index>=HowManySaved()) {
        DTErrorOutOfRange("DTSeriesPath3D(i)",index,HowManySaved());
        return DTPath3D();
    }
    DTPath3D toReturn;
    Read(DataFile(),baseName(index),toReturn);
    return toReturn;
}

void Read(const DTDataStorage &input,const std::string &name,DTSeriesPath3D &toReturn)
{
    string variableName;
    DTMutablePointer<DTDataStorage> storage = DTSeries::ReferencedDataFile(input,name,variableName);
    toReturn = DTSeriesPath3D(*storage,variableName);
}
