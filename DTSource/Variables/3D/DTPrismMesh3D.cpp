// Part of DTSource. Copyright 2004-2017. David Adalsteinsson.
// see http://www.visualdatatools.com/DTSource/license.html for more information.

#include "DTPrismMesh3D.h"
#include "DTError.h"
#include "DTRegion1D.h"
#include "DTRegion3D.h"

DTPrismMesh3D::DTPrismMesh3D(const DTPrismGrid3D &GridIn,const DTFloatArray &ValuesIn)
{
    if (GridIn.NumberOfZSlices()!=ValuesIn.n() || GridIn.NumberOfPoints()!=ValuesIn.m() || ValuesIn.o()>1) {
        DTErrorMessage("DTPrismMesh3D(grid,values)","Size of value array incorrect.");
        return;
    }
    
    grid = GridIn;
    values = ValuesIn;
}

void DTPrismMesh3D::pinfo(void) const
{
    grid.pinfo();
}

DTRegion1D ValueRange(const DTPrismMesh3D &M)
{
    return ValueRange(M.Values());
}

DTRegion3D BoundingBox(const DTPrismMesh3D &M)
{
    return BoundingBox(M.Grid());
}

void Read(const DTDataStorage &input,const std::string &name,DTPrismMesh3D &toReturn)
{
    DTPrismGrid3D grid;
    Read(input,name,grid);
    DTFloatArray values;
    Read(input,name+"_v",values);
    toReturn = DTPrismMesh3D(grid,values);
}

void Write(DTDataStorage &output,const std::string &name,const DTPrismMesh3D &toWrite)
{
    Write(output,name+"_v",toWrite.Values());
    Write(output,name,toWrite.Grid());
}

void Read(const DTDataStorage &input,const std::string &name,DTPrismMesh3D &toReturn,DTPrismGrid3D_SaveInfo &shared)
{
    DTPrismGrid3D grid;
    Read(input,name,grid,shared);
    DTFloatArray values;
    Read(input,name+"_v",values);
    toReturn = DTPrismMesh3D(grid,values);
}

void Write(DTDataStorage &output,const std::string &name,const DTPrismMesh3D &toWrite,DTPrismGrid3D_SaveInfo &shared)
{
    Write(output,name+"_v",toWrite.Values());
    Write(output,name,toWrite.Grid(),shared);
}

