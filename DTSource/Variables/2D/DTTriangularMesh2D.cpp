// Part of DTSource. Copyright 2004-2017. David Adalsteinsson.
// see http://www.visualdatatools.com/DTSource/license.html for more information.

#include "DTTriangularMesh2D.h"

#include "DTRegion2D.h"
#include "DTRegion1D.h"
#include "DTError.h"

DTTriangularMesh2D::DTTriangularMesh2D(const DTTriangularGrid2D &gr,const DTDoubleArray &input)
{
    if (input.m()!=gr.Points().n() || input.Length()!=input.m()) {
        DTErrorMessage("DTTriangularMesh2D(Grid,Array)","Array is incompatible with grid.");
        return;
    }
    
    grid = gr;
    data = input;
}
    
void DTTriangularMesh2D::pinfo(void) const
{
    Grid().pinfo();
}

bool operator==(const DTTriangularMesh2D &A,const DTTriangularMesh2D &B)
{
    return (A.Values()==B.Values() && A.Grid()==B.Grid());
}

bool operator!=(const DTTriangularMesh2D &A,const DTTriangularMesh2D &B)
{
    return (A.Values()!=B.Values() || A.Grid()!=B.Grid());
}

DTRegion2D BoundingBox(const DTTriangularMesh2D &M)
{
    return BoundingBox(M.Grid());
}

DTRegion1D ValueRange(const DTTriangularMesh2D &M)
{
    return ValueRange(M.Values());
}

void Read(const DTDataStorage &input,const std::string &name,DTTriangularMesh2D &toReturn)
{
    DTTriangularGrid2D grid;
    DTDoubleArray v;
    Read(input,name+"_V",v);
    Read(input,name,grid);
    
    toReturn = DTTriangularMesh2D(grid,v);
}

void Write(DTDataStorage &output,const std::string &name,const DTTriangularMesh2D &theMesh)
{
    Write(output,name+"_V",theMesh.Values());
    Write(output,name,theMesh.Grid());
}

void WriteOne(DTDataStorage &output,const std::string &name,const DTTriangularMesh2D &toWrite)
{
    Write(output,name,toWrite);
    Write(output,"Seq_"+name,"2D Triangular Mesh");
    output.Flush();
}

void Read(const DTDataStorage &input,const std::string &name,DTTriangularMesh2D &toReturn,DTTriangularGrid2D_SaveInfo &shared)
{
    DTTriangularGrid2D grid;
    DTDoubleArray v;
    Read(input,name+"_V",v);
    Read(input,name,grid,shared);
    
    toReturn = DTTriangularMesh2D(grid,v);
}

void Write(DTDataStorage &output,const std::string &name,const DTTriangularMesh2D &toWrite,DTTriangularGrid2D_SaveInfo &shared)
{
    Write(output,name+"_V",toWrite.Values());
    Write(output,name,toWrite.Grid(),shared);
}

#pragma mark mutable

DTMutableTriangularMesh2D::DTMutableTriangularMesh2D(const DTTriangularGrid2D &GridIn)
{
    grid = GridIn;
    mutableData = DTMutableDoubleArray(grid.NumberOfPoints());
    data = mutableData;
}

DTMutableTriangularMesh2D::DTMutableTriangularMesh2D(const DTTriangularGrid2D &GridIn,const DTMutableDoubleArray &V)
{
    if (GridIn.Points().n()!=V.Length() || V.Length()!=V.m()) {
        DTErrorMessage("DTMutableTriangularMesh2D(grid,values)","Length of value array incorrect.");
        return;
    }
    grid = GridIn;
    mutableData = V;
    data = mutableData;
}
    
