// Part of DTSource. Copyright 2004-2017. David Adalsteinsson.
// see http://www.visualdatatools.com/DTSource/license.html for more information.

#include "DTTetrahedralMesh3D.h"
#include "DTError.h"
#include "DTRegion1D.h"
#include "DTRegion3D.h"
#include "DTArrayConversion.h"

DTTetrahedralMesh3D::DTTetrahedralMesh3D(const DTTetrahedralGrid3D &GridIn,const DTFloatArray &ValuesIn)
{
    if (GridIn.NumberOfVertices()!=ValuesIn.Length()) {
        DTErrorMessage("DTTetrahedralMesh3D(grid,values)","Length of value array incorrect.");
        return;
    }

    grid = GridIn;
    floatData = ValuesIn;
}


DTTetrahedralMesh3D::DTTetrahedralMesh3D(const DTTetrahedralGrid3D &GridIn,const DTDoubleArray &ValuesIn)
{
    if (GridIn.NumberOfVertices()!=ValuesIn.Length()) {
        DTErrorMessage("DTTetrahedralMesh3D(grid,values)","Length of value array incorrect.");
        return;
    }
    
    grid = GridIn;
    doubleData = ValuesIn;
}

DTDoubleArray DTTetrahedralMesh3D::DoubleData(void) const
{
    if (floatData.NotEmpty()) {
        DTErrorMessage("DTTetrahedralMesh3D::DoubleData","Array saved as float.");
        return DTDoubleArray();
    }
    else {
        return doubleData;
    }
}

DTFloatArray DTTetrahedralMesh3D::FloatData(void) const
{
    if (doubleData.NotEmpty()) {
        DTErrorMessage("DTTetrahedralMesh3D::FloatData","Array saved as double.");
        return DTFloatArray();
    }
    else {
        return floatData;
    }
}

DTFloatArray DTTetrahedralMesh3D::Values(void) const
{
    DTErrorMessage("TetrahedralMesh::Values()","Obsolete, use FloatData instead");
    return FloatData();
}

void DTTetrahedralMesh3D::pinfo(void) const
{
    grid.pinfo();
}

DTTetrahedralMesh3D ConvertToFloat(DTTetrahedralMesh3D m)
{
    if (m.FloatPrecision())
        return m;
    else
        return DTTetrahedralMesh3D(m.Grid(),ConvertToFloat(m.DoubleData()));
}

DTTetrahedralMesh3D ConvertToDouble(DTTetrahedralMesh3D m)
{
    if (m.DoublePrecision())
        return m;
    else
        return DTTetrahedralMesh3D(m.Grid(),ConvertToDouble(m.FloatData()));
}

DTTetrahedralMesh3D ChangeGrid(const DTTetrahedralGrid3D &grid,const DTTetrahedralMesh3D &mesh)
{
    if (mesh.Grid().NumberOfVertices()!=grid.NumberOfVertices()) {
        DTErrorMessage("ChangeGrid(TetrahedralGrid3D,TetrahedralMesh3D)","Different number of vertices");
        return DTTetrahedralMesh3D();
    }
    
    if (mesh.DoublePrecision()) {
        return DTTetrahedralMesh3D(grid,mesh.DoubleData());
    }
    else {
        return DTTetrahedralMesh3D(grid,mesh.FloatData());
    }
}

#pragma mark - mutable version

DTMutableTetrahedralMesh3D::DTMutableTetrahedralMesh3D(const DTTetrahedralGrid3D &GridIn)
{
    grid = GridIn;
    mutableFloatData = DTMutableFloatArray(grid.NumberOfVertices());
    floatData = mutableFloatData;
}

DTMutableTetrahedralMesh3D::DTMutableTetrahedralMesh3D(const DTTetrahedralGrid3D &GridIn,const DTMutableFloatArray &V)
{
    if (GridIn.NumberOfVertices()!=V.Length()) {
        DTErrorMessage("DTMutableTetrahedralMesh3D(grid,values)","Length of value array incorrect.");
        return;
    }
    grid = GridIn;
    mutableFloatData = V;
    floatData = mutableFloatData;
}

DTMutableTetrahedralMesh3D::DTMutableTetrahedralMesh3D(const DTTetrahedralGrid3D &GridIn,const DTMutableDoubleArray &V)
{
    if (GridIn.NumberOfVertices()!=V.Length()) {
        DTErrorMessage("DTMutableTetrahedralMesh3D(grid,values)","Length of value array incorrect.");
        return;
    }
    grid = GridIn;
    mutableDoubleData = V;
    doubleData = mutableDoubleData;
}

DTMutableFloatArray DTMutableTetrahedralMesh3D::Values(void) const
{
    DTErrorMessage("TetrahedralMesh::Values()","Obsolete, use FloatData instead");
    return FloatData();
}

DTMutableDoubleArray DTMutableTetrahedralMesh3D::DoubleData(void) const
{
    if (floatData.NotEmpty()) {
        DTErrorMessage("DTMutableTetrahedralMesh3D::DoubleData","Array saved as float.");
        return DTMutableDoubleArray();
    }
    else {
        return mutableDoubleData;
    }
}

DTMutableFloatArray DTMutableTetrahedralMesh3D::FloatData(void) const
{
    if (doubleData.NotEmpty()) {
        DTErrorMessage("DTMutableTetrahedralMesh3D::FloatData","Array saved as double.");
        return DTMutableFloatArray();
    }
    else {
        return mutableFloatData;
    }
}

DTRegion1D ValueRange(const DTTetrahedralMesh3D &M)
{
    return ValueRange(M.Values());
}

DTRegion3D BoundingBox(const DTTetrahedralMesh3D &M)
{
    return BoundingBox(M.Grid());
}

void Read(const DTDataStorage &input,const std::string &name,DTTetrahedralMesh3D &toReturn)
{
    DTTetrahedralGrid3D grid;
    Read(input,name,grid);
    DTFloatArray values;
    Read(input,name+"_V",values);
    toReturn = DTTetrahedralMesh3D(grid,values);
}

void Write(DTDataStorage &output,const std::string &name,const DTTetrahedralMesh3D &toWrite)
{
    Write(output,name+"_V",toWrite.Values());
    Write(output,name,toWrite.Grid());
}

void WriteOne(DTDataStorage &output,const std::string &name,const DTTetrahedralMesh3D &toWrite)
{
    Write(output,name,toWrite);
    Write(output,"Seq_"+name,"3D Tetrahedral Mesh");
    output.Flush();
}

void Read(const DTDataStorage &input,const std::string &name,DTTetrahedralMesh3D &toReturn,DTTetrahedralGrid3D_SaveInfo &shared)
{
    DTTetrahedralGrid3D grid;
    Read(input,name,grid,shared);
    if (input.SavedAsDouble(name+"_V")) {
        DTDoubleArray values;
        Read(input,name+"_V",values);
        toReturn = DTTetrahedralMesh3D(grid,values);
    }
    else {
        DTFloatArray values;
        Read(input,name+"_V",values);
        toReturn = DTTetrahedralMesh3D(grid,values);
    }
}

void Write(DTDataStorage &output,const std::string &name,const DTTetrahedralMesh3D &toWrite,DTTetrahedralGrid3D_SaveInfo &shared)
{
    if (toWrite.FloatPrecision())
        Write(output,name+"_V",toWrite.FloatData());
    else
        Write(output,name+"_V",toWrite.DoubleData());
    
    Write(output,name,toWrite.Grid(),shared);
}

