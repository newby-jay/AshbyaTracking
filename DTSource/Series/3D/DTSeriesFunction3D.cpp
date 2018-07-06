// Part of DTSource. Copyright 2004-2017. David Adalsteinsson.
// see http://www.visualdatatools.com/DTSource/license.html for more information.

#include "DTSeriesFunction3D.h"

#include "DTFunction3D.h"
#include "DTSeriesTemplates.h"

map<string,DTSeriesFunction3D> DTGlobalSeriesFunction3D;

void DTSeriesFunction3D::Register() const
{
    Register(Name());
}

void DTSeriesFunction3D::Register(string name) const
{
    DTRegisteredSeries<DTSeriesFunction3D,map<string,DTSeriesFunction3D>::const_iterator>(DTGlobalSeriesFunction3D,name,"DTSeriesFunction3D",this);
}

void DTSeriesFunction3D::pregistered(void)
{
    DTPrintRegisteredSeries<DTSeriesFunction3D,map<string,DTSeriesFunction3D>::const_iterator>(DTGlobalSeriesFunction3D);
}

bool DTSeriesFunction3D::HasBeenRegistered(string name)
{
    return DTHasRegisteredSeries<DTSeriesFunction3D,map<string,DTSeriesFunction3D>::const_iterator>(DTGlobalSeriesFunction3D,name);
}

DTSeriesFunction3D DTSeriesFunction3D::ByName(string name)
{
    return DTFindRegisteredSeries<DTSeriesFunction3D,map<string,DTSeriesFunction3D>::const_iterator>(DTGlobalSeriesFunction3D,name,"DTSeriesFunction3D");
}

void DTSeriesFunction3D::Add(DTFunction3D v,double time)
{
    if (SharedSave(time)==false) return;
    Write(DataFile(),baseName(),v);
    DataFile().Flush();
}

DTFunction3D DTSeriesFunction3D::Get(double time) const
{
    int k = IndexForTime(time);
    if (k<0) return DTFunction3D();
    DTFunction3D toReturn;
    Read(DataFile(),baseName(k),toReturn);
    return toReturn;
}

DTFunction3D DTSeriesFunction3D::operator()(int index) const
{
    if (index<0 || index>=HowManySaved()) {
        DTErrorOutOfRange("DTSeriesFunction3D(i)",index,HowManySaved());
        return DTFunction3D();
    }
    DTFunction3D toReturn;
    Read(DataFile(),baseName(index),toReturn);
    return toReturn;
}

void Read(const DTDataStorage &input,const std::string &name,DTSeriesFunction3D &toReturn)
{
    string variableName;
    DTMutablePointer<DTDataStorage> storage = DTSeries::ReferencedDataFile(input,name,variableName);
    toReturn = DTSeriesFunction3D(*storage,variableName);
}
