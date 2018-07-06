// Part of DTSource. Copyright 2004-2017. David Adalsteinsson.
// see http://www.visualdatatools.com/DTSource/license.html for more information.

#include "DTSeriesRegion3D.h"

#include "DTRegion3D.h"
#include "DTSeriesTemplates.h"

map<string,DTSeriesRegion3D> DTGlobalSeriesRegion3D;

void DTSeriesRegion3D::Register() const
{
    Register(Name());
}

void DTSeriesRegion3D::Register(string name) const
{
    DTRegisteredSeries<DTSeriesRegion3D,map<string,DTSeriesRegion3D>::const_iterator>(DTGlobalSeriesRegion3D,name,"DTSeriesRegion3D",this);
}

void DTSeriesRegion3D::pregistered(void)
{
    DTPrintRegisteredSeries<DTSeriesRegion3D,map<string,DTSeriesRegion3D>::const_iterator>(DTGlobalSeriesRegion3D);
}

bool DTSeriesRegion3D::HasBeenRegistered(string name)
{
    return DTHasRegisteredSeries<DTSeriesRegion3D,map<string,DTSeriesRegion3D>::const_iterator>(DTGlobalSeriesRegion3D,name);
}

DTSeriesRegion3D DTSeriesRegion3D::ByName(string name)
{
    return DTFindRegisteredSeries<DTSeriesRegion3D,map<string,DTSeriesRegion3D>::const_iterator>(DTGlobalSeriesRegion3D,name,"DTSeriesRegion3D");
}

void DTSeriesRegion3D::Add(DTRegion3D v,double time)
{
    if (SharedSave(time)==false) return;

    Write(DataFile(),baseName(),v);
    AddToBBox(v);
    DataFile().Flush();
}

DTRegion3D DTSeriesRegion3D::Get(double time) const
{
    int k = IndexForTime(time);
    if (k<0) return DTRegion3D();
    DTRegion3D toReturn;
    Read(DataFile(),baseName(k),toReturn);
    return toReturn;
}

DTRegion3D DTSeriesRegion3D::operator()(int index) const
{
    if (index<0 || index>=HowManySaved()) {
        DTErrorOutOfRange("DTSeriesRegion3D(i)",index,HowManySaved());
        return DTRegion3D();
    }
    DTRegion3D toReturn;
    Read(DataFile(),baseName(index),toReturn);
    return toReturn;
}

void Read(const DTDataStorage &input,const std::string &name,DTSeriesRegion3D &toReturn)
{
    string variableName;
    DTMutablePointer<DTDataStorage> storage = DTSeries::ReferencedDataFile(input,name,variableName);
    toReturn = DTSeriesRegion3D(*storage,variableName);
}
