// Part of DTSource. Copyright 2006. David Adalsteinsson.
// see http://www.visualdatatools.com/DTSource/license.html for more information.

#include "DTStructuredVectorField3D.h"
#include "DTError.h"
#include "DTRegion3D.h"

DTStructuredVectorField3D::DTStructuredVectorField3D(const DTStructuredGrid3D &GridIn,
                                                     const DTFloatArray &x,
                                                     const DTFloatArray &y,
                                                     const DTFloatArray &z)
{
    if (GridIn.m()!=x.m() || GridIn.n()!=x.n() || GridIn.o()!=x.o()) {
        DTErrorMessage("DTStructuredVectorField3D(grid,x,y,z)","Size of x is incorrect.");
        return;
    }
    if (GridIn.m()!=y.m() || GridIn.n()!=y.n() || GridIn.o()!=y.o()) {
        DTErrorMessage("DTStructuredVectorField3D(grid,x,y,z)","Size of y is incorrect.");
        return;
    }
    if (GridIn.m()!=z.m() || GridIn.n()!=z.n() || GridIn.o()!=z.o()) {
        DTErrorMessage("DTStructuredVectorField3D(grid,x,y,z)","Size of z is incorrect.");
        return;
    }
    
    grid = GridIn;
    xComponent = x;
    yComponent = y;
    zComponent = z;
}

void DTStructuredVectorField3D::pinfo(void) const
{
    grid.pinfo();
}

DTRegion3D BoundingBox(const DTStructuredVectorField3D &M)
{
    return BoundingBox(M.Grid());
}

void Read(const DTDataStorage &input,const std::string &name,DTStructuredVectorField3D &toReturn)
{
    DTStructuredGrid3D grid;
    Read(input,name,grid);
    DTFloatArray x,y,z;
    Read(input,name+"_VX",x);
    Read(input,name+"_VY",y);
    Read(input,name+"_VZ",z);
    toReturn = DTStructuredVectorField3D(grid,x,y,z);
}

void Write(DTDataStorage &output,const std::string &name,const DTStructuredVectorField3D &toWrite)
{
    Write(output,name+"_VX",toWrite.X());
    Write(output,name+"_VY",toWrite.Y());
    Write(output,name+"_VZ",toWrite.Z());
    Write(output,name,toWrite.Grid());
}

void Read(const DTDataStorage &input,const std::string &name,DTStructuredVectorField3D &toReturn,DTStructuredGrid3D_SaveInfo &shared)
{
    DTStructuredGrid3D grid;
    Read(input,name,grid,shared);
    DTFloatArray x,y,z;
    Read(input,name+"_VX",x);
    Read(input,name+"_VY",y);
    Read(input,name+"_VZ",z);
    toReturn = DTStructuredVectorField3D(grid,x,y,z);
}

void Write(DTDataStorage &output,const std::string &name,const DTStructuredVectorField3D &toWrite,DTStructuredGrid3D_SaveInfo &shared)
{
    Write(output,name+"_VX",toWrite.X());
    Write(output,name+"_VY",toWrite.Y());
    Write(output,name+"_VZ",toWrite.Z());
    Write(output,name,toWrite.Grid(),shared);
}

