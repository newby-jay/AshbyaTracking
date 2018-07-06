// Part of DTSource. Copyright 2004-2017. David Adalsteinsson.
// see http://www.visualdatatools.com/DTSource/license.html for more information.

#include "DTSeriesRegion1D.h"

#include "DTRegion1D.h"
#include "DTSeriesTemplates.h"

map<string,DTSeriesRegion1D> DTGlobalSeriesRegion1D;

void DTSeriesRegion1D::Register() const
{
    Register(Name());
}

void DTSeriesRegion1D::Register(string name) const
{
    DTRegisteredSeries<DTSeriesRegion1D,map<string,DTSeriesRegion1D>::const_iterator>(DTGlobalSeriesRegion1D,name,"DTSeriesRegion1D",this);
}

void DTSeriesRegion1D::pregistered(void)
{
    DTPrintRegisteredSeries<DTSeriesRegion1D,map<string,DTSeriesRegion1D>::const_iterator>(DTGlobalSeriesRegion1D);
}

bool DTSeriesRegion1D::HasBeenRegistered(string name)
{
    return DTHasRegisteredSeries<DTSeriesRegion1D,map<string,DTSeriesRegion1D>::const_iterator>(DTGlobalSeriesRegion1D,name);
}

DTSeriesRegion1D DTSeriesRegion1D::ByName(string name)
{
    return DTFindRegisteredSeries<DTSeriesRegion1D,map<string,DTSeriesRegion1D>::const_iterator>(DTGlobalSeriesRegion1D,name,"DTSeriesRegion1D");
}

void DTSeriesRegion1D::Add(DTRegion1D v,double time)
{
    if (SharedSave(time)==false) return;

    Write(DataFile(),baseName(),v);
    DataFile().Flush();
}

DTRegion1D DTSeriesRegion1D::Get(double time) const
{
    int k = IndexForTime(time);
    if (k<0) return DTRegion1D();
    DTRegion1D toReturn;
    Read(DataFile(),baseName(k),toReturn);
    return toReturn;
}

void Read(const DTDataStorage &input,const std::string &name,DTSeriesRegion1D &toReturn)
{
    string variableName;
    DTMutablePointer<DTDataStorage> storage = DTSeries::ReferencedDataFile(input,name,variableName);
    toReturn = DTSeriesRegion1D(*storage,variableName);
}
