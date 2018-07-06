// Part of DTSource. Copyright 2004-2017. David Adalsteinsson.
// see http://www.visualdatatools.com/DTSource/license.html for more information.

#include "DTPrismVectorField3D.h"

#include "DTPrismVectorField3D.h"
#include "DTError.h"
#include "DTRegion3D.h"

DTPrismVectorField3D::DTPrismVectorField3D(const DTPrismGrid3D &GridIn,
                                           const DTFloatArray &x,
                                           const DTFloatArray &y,
                                           const DTFloatArray &z)
{
    if (GridIn.NumberOfZSlices()!=x.n() || GridIn.NumberOfTriangles()!=x.m() || x.o()>1) {
        DTErrorMessage("DTPrismVectorField3D(grid,x,y,z)","Size of x is incorrect.");
        return;
    }
    if (GridIn.NumberOfZSlices()!=y.n() || GridIn.NumberOfTriangles()!=y.m() || y.o()>1) {
        DTErrorMessage("DTPrismVectorField3D(grid,x,y,z)","Size of y is incorrect.");
        return;
    }
    if (GridIn.NumberOfZSlices()!=z.n() || GridIn.NumberOfTriangles()!=z.m() || z.o()>1) {
        DTErrorMessage("DTPrismVectorField3D(grid,x,y,z)","Size of z is incorrect.");
        return;
    }
    
    grid = GridIn;
    xComponent = x;
    yComponent = y;
    zComponent = z;
}

void DTPrismVectorField3D::pinfo(void) const
{
    grid.pinfo();
}

DTRegion3D BoundingBox(const DTPrismVectorField3D &M)
{
    return BoundingBox(M.Grid());
}

void Read(const DTDataStorage &input,const std::string &name,DTPrismVectorField3D &toReturn)
{
    DTPrismGrid3D grid;
    Read(input,name,grid);
    DTFloatArray x,y,z;
    Read(input,name+"_vx",x);
    Read(input,name+"_vy",y);
    Read(input,name+"_vz",z);
    toReturn = DTPrismVectorField3D(grid,x,y,z);
}

void Write(DTDataStorage &output,const std::string &name,const DTPrismVectorField3D &toWrite)
{
    Write(output,name+"_vx",toWrite.X());
    Write(output,name+"_vy",toWrite.Y());
    Write(output,name+"_vz",toWrite.Z());
    Write(output,name,toWrite.Grid());
}

void Read(const DTDataStorage &input,const std::string &name,DTPrismVectorField3D &toReturn,DTPrismGrid3D_SaveInfo &shared)
{
    DTPrismGrid3D grid;
    Read(input,name,grid,shared);
    DTFloatArray x,y,z;
    Read(input,name+"_vx",x);
    Read(input,name+"_vy",y);
    Read(input,name+"_vz",z);
    toReturn = DTPrismVectorField3D(grid,x,y,z);
}

void Write(DTDataStorage &output,const std::string &name,const DTPrismVectorField3D &toWrite,DTPrismGrid3D_SaveInfo &shared)
{
    Write(output,name+"_vx",toWrite.X());
    Write(output,name+"_vy",toWrite.Y());
    Write(output,name+"_vz",toWrite.Z());
    Write(output,name,toWrite.Grid(),shared);
}

