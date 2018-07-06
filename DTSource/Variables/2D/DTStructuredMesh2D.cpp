// Part of DTSource. Copyright 2004-2017. David Adalsteinsson.
// see http://www.visualdatatools.com/DTSource/license.html for more information.

#include "DTStructuredMesh2D.h"

#include "DTError.h"
#include "DTRegion1D.h"
#include "DTRegion2D.h"

DTStructuredMesh2D::DTStructuredMesh2D(const DTStructuredGrid2D &gr,const DTDoubleArray &input)
{
    if (input.IsEmpty()) return;

    if (input.o()!=1) {
        DTErrorMessage("DTStructuredMesh2D(Grid,Array)","Array is three dimensional.");
        return;
    }

    if (gr.m()!=input.m() || gr.n()!=input.n()) {
        DTWarningMessage("DTStructuredMesh2D(Grid,Array)","Grid size is incompatible.");
        return;
    }

    grid = gr;
    data = input;
}

void DTStructuredMesh2D::pinfo(void) const
{
    grid.pinfo();
}

bool operator==(const DTStructuredMesh2D &A,const DTStructuredMesh2D &B)
{
    return (A.Data()==B.Data() && A.Grid()==B.Grid());
}

bool operator!=(const DTStructuredMesh2D &A,const DTStructuredMesh2D &B)
{
    return !(A==B);
}

DTRegion2D BoundingBox(const DTStructuredMesh2D &M)
{
    return BoundingBox(M.Grid());
}

DTRegion1D ValueRange(const DTStructuredMesh2D &M)
{
    return ValueRange(M.Data());
}

void Read(const DTDataStorage &input,const std::string &name,DTStructuredMesh2D &toReturn)
{
    DTDoubleArray data;
    Read(input,name+"_V",data);

    DTStructuredGrid2D grid;
    Read(input,name,grid);

    toReturn = DTStructuredMesh2D(grid,data);
}

void Write(DTDataStorage &output,const std::string &name,const DTStructuredMesh2D &theMesh)
{
    Write(output,name,theMesh.Grid());
    Write(output,name+"_V",theMesh.Data());
}

void WriteOne(DTDataStorage &output,const std::string &name,const DTStructuredMesh2D &toWrite)
{
    Write(output,name,toWrite);
    Write(output,"Seq_"+name,"2D Structured Mesh");
    output.Flush();
}

void Read(const DTDataStorage &input,const std::string &name,DTStructuredMesh2D &toReturn,DTStructuredGrid2D_SaveInfo &shared)
{
    DTDoubleArray data;
    Read(input,name+"_V",data);
    
    DTStructuredGrid2D grid;
    Read(input,name,grid,shared);
    
    toReturn = DTStructuredMesh2D(grid,data);
}

void Write(DTDataStorage &output,const std::string &name,const DTStructuredMesh2D &theMesh,DTStructuredGrid2D_SaveInfo &shared)
{
    Write(output,name,theMesh.Grid(),shared);
    Write(output,name+"_V",theMesh.Data());
}

