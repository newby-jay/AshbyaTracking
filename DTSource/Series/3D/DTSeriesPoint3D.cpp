// Part of DTSource. Copyright 2004-2017. David Adalsteinsson.
// see http://www.visualdatatools.com/DTSource/license.html for more information.

#include "DTSeriesPoint3D.h"

#include "DTPoint3D.h"
#include "DTSeriesTemplates.h"

map<string,DTSeriesPoint3D> DTGlobalSeriesPoint3D;

void DTSeriesPoint3D::Register() const
{
    Register(Name());
}

void DTSeriesPoint3D::Register(string name) const
{
    DTRegisteredSeries<DTSeriesPoint3D,map<string,DTSeriesPoint3D>::const_iterator>(DTGlobalSeriesPoint3D,name,"DTSeriesPoint3D",this);
}

void DTSeriesPoint3D::pregistered(void)
{
    DTPrintRegisteredSeries<DTSeriesPoint3D,map<string,DTSeriesPoint3D>::const_iterator>(DTGlobalSeriesPoint3D);
}

bool DTSeriesPoint3D::HasBeenRegistered(string name)
{
    return DTHasRegisteredSeries<DTSeriesPoint3D,map<string,DTSeriesPoint3D>::const_iterator>(DTGlobalSeriesPoint3D,name);
}

DTSeriesPoint3D DTSeriesPoint3D::ByName(string name)
{
    return DTFindRegisteredSeries<DTSeriesPoint3D,map<string,DTSeriesPoint3D>::const_iterator>(DTGlobalSeriesPoint3D,name,"DTSeriesPoint3D");
}

void DTSeriesPoint3D::Add(DTPoint3D v,double time)
{
    if (SharedSave(time)==false) return;

    Write(DataFile(),baseName(),v);
    AddToBBox(DTRegion3D(v,v));
    DataFile().Flush();
}

DTPoint3D DTSeriesPoint3D::Get(double time) const
{
    int k = IndexForTime(time);
    if (k<0) return DTPoint3D();
    DTPoint3D toReturn;
    Read(DataFile(),baseName(k),toReturn);
    return toReturn;
}

DTPoint3D DTSeriesPoint3D::operator()(int index) const
{
    if (index<0 || index>=HowManySaved()) {
        DTErrorOutOfRange("DTSeriesPoint3D(i)",index,HowManySaved());
        return DTPoint3D();
    }
    DTPoint3D toReturn;
    Read(DataFile(),baseName(index),toReturn);
    return toReturn;
}

void Read(const DTDataStorage &input,const std::string &name,DTSeriesPoint3D &toReturn)
{
    string variableName;
    DTMutablePointer<DTDataStorage> storage = DTSeries::ReferencedDataFile(input,name,variableName);
    toReturn = DTSeriesPoint3D(*storage,variableName);
}
