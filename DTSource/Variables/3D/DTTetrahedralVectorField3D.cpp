// Part of DTSource. Copyright 2004-2017. David Adalsteinsson.
// see http://www.visualdatatools.com/DTSource/license.html for more information.

#include "DTTetrahedralVectorField3D.h"
#include "DTRegion3D.h"
#include "DTError.h"

DTTetrahedralVectorField3D::DTTetrahedralVectorField3D(const DTTetrahedralGrid3D &GridIn,const DTFloatArray &VectorsIn)
{
    if (GridIn.Points().NumberOfPoints()!=VectorsIn.n()) {
        DTErrorMessage("DTTetrahedralVectorField3D(grid,vectors)","Length of value array incorrect.");
        return;
    }

    if (VectorsIn.m()!=0 && (VectorsIn.m()!=3 || VectorsIn.o()!=1)) {
        DTErrorMessage("DTTetrahedralVectorField3D(grid,vectors)","Vector array incorrect.");
        return;
    }

    grid = GridIn;
    vectors = VectorsIn;
}

void DTTetrahedralVectorField3D::pinfo(void) const
{
    grid.pinfo();
}

DTRegion3D BoundingBox(const DTTetrahedralVectorField3D &F)
{
    return BoundingBox(F.Grid());
}

void Read(const DTDataStorage &input,const std::string &name,DTTetrahedralVectorField3D &toReturn)
{
    DTTetrahedralGrid3D grid;
    Read(input,name,grid);
    DTFloatArray vectors;
    Read(input,name+"_XYZ",vectors);
    toReturn = DTTetrahedralVectorField3D(grid,vectors);
}

void Write(DTDataStorage &output,const std::string &name,const DTTetrahedralVectorField3D &toWrite)
{
    Write(output,name+"_XYZ",toWrite.Vectors());
    Write(output,name,toWrite.Grid());
}

void WriteOne(DTDataStorage &output,const std::string &name,const DTTetrahedralVectorField3D &toWrite)
{
    Write(output,name,toWrite);
    Write(output,"Seq_"+name,"3D Tetrahedral Vector Field");
    output.Flush();
}

void Read(const DTDataStorage &input,const std::string &name,DTTetrahedralVectorField3D &toReturn,DTTetrahedralGrid3D_SaveInfo &shared)
{
    DTTetrahedralGrid3D grid;
    Read(input,name,grid,shared);
    DTFloatArray vectors;
    Read(input,name+"_XYZ",vectors);
    toReturn = DTTetrahedralVectorField3D(grid,vectors);
}

void Write(DTDataStorage &output,const std::string &name,const DTTetrahedralVectorField3D &toWrite,DTTetrahedralGrid3D_SaveInfo &shared)
{
    Write(output,name+"_XYZ",toWrite.Vectors());
    Write(output,name,toWrite.Grid(),shared);
}

