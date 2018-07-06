// Part of DTSource. Copyright 2004-2017. David Adalsteinsson.
// see http://www.visualdatatools.com/DTSource/license.html for more information.

#include "DTTriangularVectorField2D.h"

#include "DTRegion2D.h"
#include "DTRegion1D.h"
#include "DTError.h"

DTTriangularVectorField2D::DTTriangularVectorField2D(const DTTriangularGrid2D &gr,const DTDoubleArray &x,const DTDoubleArray &y)
{
    if (x.m()!=gr.Points().n() || x.Length()!=x.m() || y.m()!=gr.Points().n() || y.Length()!=y.m()) {
        DTErrorMessage("DTTriangularVectorField2D(Grid,Array,Array)","Arrays and grid are incompatible.");
        return;
    }
    
    grid = gr;
    xComponent = x;
    yComponent = y;
}

void DTTriangularVectorField2D::pinfo(void) const
{
    Grid().pinfo();
}

bool operator==(const DTTriangularVectorField2D &A,const DTTriangularVectorField2D &B)
{
    return (A.X()==B.X() && A.Y()==B.Y() && A.Grid()==B.Grid());
}

bool operator!=(const DTTriangularVectorField2D &A,const DTTriangularVectorField2D &B)
{
    return (A.X()!=B.X() || A.Y()!=B.Y() || A.Grid()!=B.Grid());
}

DTRegion2D BoundingBox(const DTTriangularVectorField2D &M)
{
    return BoundingBox(M.Grid());
}

void Read(const DTDataStorage &input,const std::string &name,DTTriangularVectorField2D &toReturn)
{
    DTTriangularGrid2D grid;
    DTDoubleArray x,y;
    Read(input,name+"_VX",x);
    Read(input,name+"_VY",y);
    Read(input,name,grid);
    
    toReturn = DTTriangularVectorField2D(grid,x,y);
}

void Write(DTDataStorage &output,const std::string &name,const DTTriangularVectorField2D &theVectorField)
{
    Write(output,name+"_VX",theVectorField.X());
    Write(output,name+"_VY",theVectorField.Y());
    Write(output,name,theVectorField.Grid());
}

void WriteOne(DTDataStorage &output,const std::string &name,const DTTriangularVectorField2D &toWrite)
{
    Write(output,name,toWrite);
    Write(output,"Seq_"+name,"2D Triangular Vector Field");
    output.Flush();
}

void Read(const DTDataStorage &input,const std::string &name,DTTriangularVectorField2D &toReturn,DTTriangularGrid2D_SaveInfo &shared)
{
    DTTriangularGrid2D grid;
    DTDoubleArray x,y;
    Read(input,name+"_VX",x);
    Read(input,name+"_VY",y);
    Read(input,name,grid,shared);
    
    toReturn = DTTriangularVectorField2D(grid,x,y);
}

void Write(DTDataStorage &output,const std::string &name,const DTTriangularVectorField2D &theVectorField,DTTriangularGrid2D_SaveInfo &shared)
{
    Write(output,name+"_VX",theVectorField.X());
    Write(output,name+"_VY",theVectorField.Y());
    Write(output,name,theVectorField.Grid(),shared);
}

