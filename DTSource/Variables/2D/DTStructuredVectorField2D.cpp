// Part of DTSource. Copyright 2006. David Adalsteinsson.
// see http://www.visualdatatools.com/DTSource/license.html for more information.

#include "DTStructuredVectorField2D.h"
#include "DTError.h"
#include "DTRegion2D.h"

DTStructuredVectorField2D::DTStructuredVectorField2D(const DTStructuredGrid2D &GridIn,
                                                     const DTFloatArray &x,
                                                     const DTFloatArray &y)
{
    if (x.o()>1 || y.o()>1) {
        DTErrorMessage("DTStructuredVectorField2D(grid,x,y)","x and y can not be 3D.");
        return;
    }

    if (GridIn.m()!=x.m() || GridIn.n()!=x.n() || 1!=x.o()) {
        DTErrorMessage("DTStructuredVectorField2D(grid,x,y)","Size of x is incorrect.");
        return;
    }
    if (GridIn.m()!=y.m() || GridIn.n()!=y.n() || 1!=y.o()) {
        DTErrorMessage("DTStructuredVectorField2D(grid,x,y)","Size of y is incorrect.");
        return;
    }
    
    grid = GridIn;
    xComponent = x;
    yComponent = y;
}

void DTStructuredVectorField2D::pinfo(void) const
{
    grid.pinfo();
}

DTRegion2D BoundingBox(const DTStructuredVectorField2D &M)
{
    return BoundingBox(M.Grid());
}

void Read(const DTDataStorage &input,const std::string &name,DTStructuredVectorField2D &toReturn)
{
    DTStructuredGrid2D grid;
    Read(input,name,grid);
    DTFloatArray x,y;
    Read(input,name+"_VX",x);
    Read(input,name+"_VY",y);
    toReturn = DTStructuredVectorField2D(grid,x,y);
}

void Write(DTDataStorage &output,const std::string &name,const DTStructuredVectorField2D &toWrite)
{
    Write(output,name+"_VX",toWrite.X());
    Write(output,name+"_VY",toWrite.Y());
    Write(output,name,toWrite.Grid());
}

void Read(const DTDataStorage &input,const std::string &name,DTStructuredVectorField2D &toReturn,DTStructuredGrid2D_SaveInfo &shared)
{
    DTStructuredGrid2D grid;
    Read(input,name,grid,shared);
    DTFloatArray x,y;
    Read(input,name+"_VX",x);
    Read(input,name+"_VY",y);
    toReturn = DTStructuredVectorField2D(grid,x,y);
}

void Write(DTDataStorage &output,const std::string &name,const DTStructuredVectorField2D &toWrite,DTStructuredGrid2D_SaveInfo &shared)
{
    Write(output,name+"_VX",toWrite.X());
    Write(output,name+"_VY",toWrite.Y());
    Write(output,name,toWrite.Grid(),shared);
}

