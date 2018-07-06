// Part of DTSource. Copyright 2004-2017. David Adalsteinsson.
// see http://www.visualdatatools.com/DTSource/license.html for more information.

#include "DTSeriesPointCollection2D.h"

#include "DTPointCollection2D.h"
#include "DTSeriesTemplates.h"

map<string,DTSeriesPointCollection2D> DTGlobalSeriesPointCollection2D;

void DTSeriesPointCollection2D::Register() const
{
    Register(Name());
}

void DTSeriesPointCollection2D::Register(string name) const
{
    DTRegisteredSeries<DTSeriesPointCollection2D,map<string,DTSeriesPointCollection2D>::const_iterator>(DTGlobalSeriesPointCollection2D,name,"DTSeriesPointCollection2D",this);
}

void DTSeriesPointCollection2D::pregistered(void)
{
    DTPrintRegisteredSeries<DTSeriesPointCollection2D,map<string,DTSeriesPointCollection2D>::const_iterator>(DTGlobalSeriesPointCollection2D);
}

bool DTSeriesPointCollection2D::HasBeenRegistered(string name)
{
    return DTHasRegisteredSeries<DTSeriesPointCollection2D,map<string,DTSeriesPointCollection2D>::const_iterator>(DTGlobalSeriesPointCollection2D,name);
}

DTSeriesPointCollection2D DTSeriesPointCollection2D::ByName(string name)
{
    return DTFindRegisteredSeries<DTSeriesPointCollection2D,map<string,DTSeriesPointCollection2D>::const_iterator>(DTGlobalSeriesPointCollection2D,name,"DTSeriesPointCollection2D");
}

void DTSeriesPointCollection2D::Add(DTPointCollection2D v,double time)
{
    if (SharedSave(time)==false) return;

    Write(DataFile(),baseName(),v);
    AddToBBox(BoundingBox(v));
    DataFile().Flush();
}

DTPointCollection2D DTSeriesPointCollection2D::Get(double time) const
{
    int k = IndexForTime(time);
    if (k<0) return DTPointCollection2D();
    DTPointCollection2D toReturn;
    Read(DataFile(),baseName(k),toReturn);
    return toReturn;
}

void Read(const DTDataStorage &input,const std::string &name,DTSeriesPointCollection2D &toReturn)
{
    string variableName;
    DTMutablePointer<DTDataStorage> storage = DTSeries::ReferencedDataFile(input,name,variableName);
    toReturn = DTSeriesPointCollection2D(*storage,variableName);
}
