// Part of DTSource. Copyright 2004-2017. David Adalsteinsson.
// see http://www.visualdatatools.com/DTSource/license.html for more information.

#include "DTSeriesVectorCollection2D.h"

#include "DTVectorCollection2D.h"
#include "DTSeriesTemplates.h"

map<string,DTSeriesVectorCollection2D> DTGlobalSeriesVectorCollection2D;

void DTSeriesVectorCollection2D::Register() const
{
    Register(Name());
}

void DTSeriesVectorCollection2D::Register(string name) const
{
    DTRegisteredSeries<DTSeriesVectorCollection2D,map<string,DTSeriesVectorCollection2D>::const_iterator>(DTGlobalSeriesVectorCollection2D,name,"DTSeriesVectorCollection2D",this);
}

void DTSeriesVectorCollection2D::pregistered(void)
{
    DTPrintRegisteredSeries<DTSeriesVectorCollection2D,map<string,DTSeriesVectorCollection2D>::const_iterator>(DTGlobalSeriesVectorCollection2D);
}

bool DTSeriesVectorCollection2D::HasBeenRegistered(string name)
{
    return DTHasRegisteredSeries<DTSeriesVectorCollection2D,map<string,DTSeriesVectorCollection2D>::const_iterator>(DTGlobalSeriesVectorCollection2D,name);
}

DTSeriesVectorCollection2D DTSeriesVectorCollection2D::ByName(string name)
{
    return DTFindRegisteredSeries<DTSeriesVectorCollection2D,map<string,DTSeriesVectorCollection2D>::const_iterator>(DTGlobalSeriesVectorCollection2D,name,"DTSeriesVectorCollection2D");
}

void DTSeriesVectorCollection2D::Add(DTVectorCollection2D v,double time)
{
    if (SharedSave(time)==false) return;

    Write(DataFile(),baseName(),v);
    AddToBBox(BoundingBox(v));
    DataFile().Flush();
}

DTVectorCollection2D DTSeriesVectorCollection2D::Get(double time) const
{
    int k = IndexForTime(time);
    if (k<0) return DTVectorCollection2D();
    DTVectorCollection2D toReturn;
    Read(DataFile(),baseName(k),toReturn);
    return toReturn;
}

void Read(const DTDataStorage &input,const std::string &name,DTSeriesVectorCollection2D &toReturn)
{
    string variableName;
    DTMutablePointer<DTDataStorage> storage = DTSeries::ReferencedDataFile(input,name,variableName);
    toReturn = DTSeriesVectorCollection2D(*storage,variableName);
}
