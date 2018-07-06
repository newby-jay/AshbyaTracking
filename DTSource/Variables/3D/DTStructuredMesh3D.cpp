// Part of DTSource. Copyright 2004-2015. David A. David Adalsteinsson.
// see http://www.visualdatatools.com/DTSource/license.html for more information.

#include "DTStructuredMesh3D.h"

#include "DTError.h"
#include "DTRegion1D.h"
#include "DTRegion3D.h"

DTStructuredMesh3D::DTStructuredMesh3D(const DTStructuredGrid3D &gr,const DTFloatArray &input)
{
    if (input.IsEmpty()) return;
    
    if (gr.m()!=input.m() || gr.n()!=input.n() || gr.o()!=input.o()) {
        DTWarningMessage("DTStructuredMesh3D(Grid,Array)","Grid size is incompatible.");
        return;
    }
    
    grid = gr;
    data = input;
}

void DTStructuredMesh3D::pinfo(void) const
{
    grid.pinfo();
}

bool operator==(const DTStructuredMesh3D &A,const DTStructuredMesh3D &B)
{
    return (A.Data()==B.Data() && A.Grid()==B.Grid());
}

bool operator!=(const DTStructuredMesh3D &A,const DTStructuredMesh3D &B)
{
    return !(A==B);
}

DTRegion3D BoundingBox(const DTStructuredMesh3D &M)
{
    return BoundingBox(M.Grid());
}

DTRegion1D ValueRange(const DTStructuredMesh3D &M)
{
    return ValueRange(M.Data());
}

void Read(const DTDataStorage &input,const std::string &name,DTStructuredMesh3D &toReturn)
{
    DTFloatArray data;
    Read(input,name+"_V",data);
    
    DTStructuredGrid3D grid;
    Read(input,name,grid);
    
    toReturn = DTStructuredMesh3D(grid,data);
}

void Write(DTDataStorage &output,const std::string &name,const DTStructuredMesh3D &theMesh)
{
    Write(output,name,theMesh.Grid());
    Write(output,name+"_V",theMesh.Data());
}

void Read(const DTDataStorage &input,const std::string &name,DTStructuredMesh3D &toReturn,DTStructuredGrid3D_SaveInfo &shared)
{
    DTFloatArray data;
    Read(input,name+"_V",data);
    
    DTStructuredGrid3D grid;
    Read(input,name,grid,shared);
    
    toReturn = DTStructuredMesh3D(grid,data);
}

void Write(DTDataStorage &output,const std::string &name,const DTStructuredMesh3D &theMesh,DTStructuredGrid3D_SaveInfo &shared)
{
    Write(output,name,theMesh.Grid(),shared);
    Write(output,name+"_V",theMesh.Data());
}

