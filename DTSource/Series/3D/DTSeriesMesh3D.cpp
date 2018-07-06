// Part of DTSource. Copyright 2004-2017. David Adalsteinsson.
// see http://www.visualdatatools.com/DTSource/license.html for more information.

#include "DTSeriesMesh3D.h"

#include "DTMesh3D.h"
#include "DTSeriesTemplates.h"

map<string,DTSeriesMesh3D> DTGlobalSeriesMesh3D;

void DTSeriesMesh3D::Register() const
{
    Register(Name());
}

void DTSeriesMesh3D::Register(string name) const
{
    DTRegisteredSeries<DTSeriesMesh3D,map<string,DTSeriesMesh3D>::const_iterator>(DTGlobalSeriesMesh3D,name,"DTSeriesMesh3D",this);
}

void DTSeriesMesh3D::pregistered(void)
{
    DTPrintRegisteredSeries<DTSeriesMesh3D,map<string,DTSeriesMesh3D>::const_iterator>(DTGlobalSeriesMesh3D);
}

bool DTSeriesMesh3D::HasBeenRegistered(string name)
{
    return DTHasRegisteredSeries<DTSeriesMesh3D,map<string,DTSeriesMesh3D>::const_iterator>(DTGlobalSeriesMesh3D,name);
}

DTSeriesMesh3D DTSeriesMesh3D::ByName(string name)
{
    return DTFindRegisteredSeries<DTSeriesMesh3D,map<string,DTSeriesMesh3D>::const_iterator>(DTGlobalSeriesMesh3D,name,"DTSeriesMesh3D");
}

void DTSeriesMesh3D::Add(const DTDoubleArray &v,double time)
{
    if (SharedSave(time)==false) return;
    Write(DataFile(),baseName(),v);

    if (v.NotEmpty())
        AddToBBox(DTRegion3D(DTPoint3D(0,0,0),DTPoint3D(v.m()-1,v.n()-1,v.o()-1)));
    AddToValueRange(ValueRange(v));
    DataFile().Flush();
}

void DTSeriesMesh3D::Add(const DTDoubleArray &v,const DTMesh3DGrid &grid,double time)
{
    if (SharedSave(time)==false) return;
    WriteNoSize(DataFile(),baseName()+"_loc",grid);
    Write(DataFile(),baseName(),v);

    AddToBBox(BoundingBox(grid));
    AddToValueRange(ValueRange(v));
    DataFile().Flush();
}

void DTSeriesMesh3D::Add(const DTMesh3D &v,double time)
{
    if (SharedSave(time)==false) return;
    Write(DataFile(),baseName(),v);
    
    AddToBBox(BoundingBox(v));
    AddToValueRange(ValueRange(v));
    DataFile().Flush();
}

void DTSeriesMesh3D::Add(const DTMesh3DBanded &v,double time)
{
    if (SharedSave(time)==false) return;
    if (_saveDoubleAsFloat)
        WriteFloat(DataFile(),baseName(),v);
    else
        Write(DataFile(),baseName(),v);

    AddToBBox(BoundingBox(v));
    AddToValueRange(ValueRange(v));
    DataFile().Flush();
}

DTMesh3D DTSeriesMesh3D::Get(double time) const
{
    int k = IndexForTime(time);
    if (k<0) return DTMesh3D();
    DTMesh3D toReturn;
    Read(DataFile(),baseName(k),toReturn);
    return toReturn;
}

DTMesh3D DTSeriesMesh3D::operator()(int index) const
{
    if (index<0 || index>=HowManySaved()) {
        DTErrorOutOfRange("DTSeriesMesh3D(i)",index,HowManySaved());
        return DTMesh3D();
    }
    DTMesh3D toReturn;
    Read(DataFile(),baseName(index),toReturn);
    return toReturn;
}

void Read(const DTDataStorage &input,const std::string &name,DTSeriesMesh3D &toReturn)
{
    string variableName;
    DTMutablePointer<DTDataStorage> storage = DTSeries::ReferencedDataFile(input,name,variableName);
    toReturn = DTSeriesMesh3D(*storage,variableName);
}
