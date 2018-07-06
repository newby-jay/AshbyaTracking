// Part of DTSource. Copyright 2004-2017. David Adalsteinsson.
// see http://www.visualdatatools.com/DTSource/license.html for more information.

#include "DTSeriesPointCollection3D.h"

#include "DTPointCollection3D.h"
#include "DTRegion3D.h"
#include "DTSeriesTemplates.h"

map<string,DTSeriesPointCollection3D> DTGlobalSeriesPointCollection3D;

void DTSeriesPointCollection3D::Register() const
{
    Register(Name());
}

void DTSeriesPointCollection3D::Register(string name) const
{
    DTRegisteredSeries<DTSeriesPointCollection3D,map<string,DTSeriesPointCollection3D>::const_iterator>(DTGlobalSeriesPointCollection3D,name,"DTSeriesPointCollection3D",this);
}

void DTSeriesPointCollection3D::pregistered(void)
{
    DTPrintRegisteredSeries<DTSeriesPointCollection3D,map<string,DTSeriesPointCollection3D>::const_iterator>(DTGlobalSeriesPointCollection3D);
}

bool DTSeriesPointCollection3D::HasBeenRegistered(string name)
{
    return DTHasRegisteredSeries<DTSeriesPointCollection3D,map<string,DTSeriesPointCollection3D>::const_iterator>(DTGlobalSeriesPointCollection3D,name);
}

DTSeriesPointCollection3D DTSeriesPointCollection3D::ByName(string name)
{
    return DTFindRegisteredSeries<DTSeriesPointCollection3D,map<string,DTSeriesPointCollection3D>::const_iterator>(DTGlobalSeriesPointCollection3D,name,"DTSeriesPointCollection3D");
}

void DTSeriesPointCollection3D::Add(DTPointCollection3D v,double time)
{
    if (SharedSave(time)==false) return;
    AddToBBox(BoundingBox(v));
    if (_saveDoubleAsFloat)
        WriteFloat(DataFile(),baseName(),v);
    else
        Write(DataFile(),baseName(),v);
    DataFile().Flush();
}

DTPointCollection3D DTSeriesPointCollection3D::Get(double time) const
{
    int k = IndexForTime(time);
    if (k<0) return DTPointCollection3D();
    DTPointCollection3D toReturn;
    Read(DataFile(),baseName(k),toReturn);
    return toReturn;
}

DTPointCollection3D DTSeriesPointCollection3D::operator()(int index) const
{
    if (index<0 || index>=HowManySaved()) {
        DTErrorOutOfRange("DTSeriesPointCollection3D(i)",index,HowManySaved());
        return DTPointCollection3D();
    }
    DTPointCollection3D toReturn;
    Read(DataFile(),baseName(index),toReturn);
    return toReturn;
}

void Read(const DTDataStorage &input,const std::string &name,DTSeriesPointCollection3D &toReturn)
{
    string variableName;
    DTMutablePointer<DTDataStorage> storage = DTSeries::ReferencedDataFile(input,name,variableName);
    toReturn = DTSeriesPointCollection3D(*storage,variableName);
}
