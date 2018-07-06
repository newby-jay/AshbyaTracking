// Part of DTSource. Copyright 2004-2015. David A. David Adalsteinsson.
// see http://www.visualdatatools.com/DTSource/license.html for more information.

#include "DTSeriesBitmap2D.h"

#include "DTBitmap2D.h"
#include "DTSeriesTemplates.h"

map<string,DTSeriesBitmap2D> DTGlobalSeriesBitmap2D;

void DTSeriesBitmap2D ::Register() const
{
    Register(Name());
}

void DTSeriesBitmap2D ::Register(string name) const
{
    DTRegisteredSeries<DTSeriesBitmap2D ,map<string,DTSeriesBitmap2D >::const_iterator>(DTGlobalSeriesBitmap2D,name,"DTSeriesBitmap2D ",this);
}

void DTSeriesBitmap2D ::pregistered(void)
{
    DTPrintRegisteredSeries<DTSeriesBitmap2D ,map<string,DTSeriesBitmap2D >::const_iterator>(DTGlobalSeriesBitmap2D);
}

bool DTSeriesBitmap2D ::HasBeenRegistered(string name)
{
    return DTHasRegisteredSeries<DTSeriesBitmap2D ,map<string,DTSeriesBitmap2D >::const_iterator>(DTGlobalSeriesBitmap2D,name);
}

DTSeriesBitmap2D  DTSeriesBitmap2D ::ByName(string name)
{
    return DTFindRegisteredSeries<DTSeriesBitmap2D ,map<string,DTSeriesBitmap2D >::const_iterator>(DTGlobalSeriesBitmap2D,name,"DTSeriesBitmap2D ");
}

void DTSeriesBitmap2D::Add(DTBitmap2D v,double time)
{
    if (SharedSave(time)==false) return;
    Write(DataFile(),baseName(),v);
    
    AddToBBox(BoundingBox(v));
    DataFile().Flush();
}

DTBitmap2D DTSeriesBitmap2D::Get(double time) const
{
    int k = IndexForTime(time);
    if (k<0) return DTBitmap2D();
    DTBitmap2D toReturn;
    Read(DataFile(),baseName(k),toReturn);
    return toReturn;
}

void Read(const DTDataStorage &input,const std::string &name,DTSeriesBitmap2D &toReturn)
{
    string variableName;
    DTMutablePointer<DTDataStorage> storage = DTSeries::ReferencedDataFile(input,name,variableName);
    toReturn = DTSeriesBitmap2D(*storage,input.ReadString(name));
}
