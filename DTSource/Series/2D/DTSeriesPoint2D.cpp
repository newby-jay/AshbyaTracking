// Part of DTSource. Copyright 2004-2017. David Adalsteinsson.
// see http://www.visualdatatools.com/DTSource/license.html for more information.

#include "DTSeriesPoint2D.h"

#include "DTPoint2D.h"
#include "DTRegion2D.h"
#include "DTSeriesTemplates.h"

map<string,DTSeriesPoint2D> DTGlobalSeriesPoint2D;
 
void DTSeriesPoint2D::Register() const
{
    Register(Name());
}

void DTSeriesPoint2D::Register(string name) const
{
    DTRegisteredSeries<DTSeriesPoint2D,map<string,DTSeriesPoint2D>::const_iterator>(DTGlobalSeriesPoint2D,name,"DTSeriesPoint2D",this);
}

void DTSeriesPoint2D::pregistered(void)
{
    DTPrintRegisteredSeries<DTSeriesPoint2D,map<string,DTSeriesPoint2D>::const_iterator>(DTGlobalSeriesPoint2D);
}

bool DTSeriesPoint2D::HasBeenRegistered(string name)
{
    return DTHasRegisteredSeries<DTSeriesPoint2D,map<string,DTSeriesPoint2D>::const_iterator>(DTGlobalSeriesPoint2D,name);
}

DTSeriesPoint2D DTSeriesPoint2D::ByName(string name)
{
    return DTFindRegisteredSeries<DTSeriesPoint2D,map<string,DTSeriesPoint2D>::const_iterator>(DTGlobalSeriesPoint2D,name,"DTSeriesPoint2D");
}

void DTSeriesPoint2D::Add(DTPoint2D v,double time)
{
    if (SharedSave(time)==false) return;

    Write(DataFile(),baseName(),v);
    AddToBBox(DTRegion2D(v,v));
    DataFile().Flush();
}

DTPoint2D DTSeriesPoint2D::Get(double time) const
{
    int k = IndexForTime(time);
    if (k<0) return DTPoint2D();
    DTPoint2D toReturn;
    Read(DataFile(),baseName(k),toReturn);
    return toReturn;
}

void Read(const DTDataStorage &input,const std::string &name,DTSeriesPoint2D &toReturn)
{
    string variableName;
    DTMutablePointer<DTDataStorage> storage = DTSeries::ReferencedDataFile(input,name,variableName);
    toReturn = DTSeriesPoint2D(*storage,variableName);
}
