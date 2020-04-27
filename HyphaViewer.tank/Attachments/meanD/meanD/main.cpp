// Include DTSource.h if you want to include all the headers.

#include "DTArguments.h"
#include "DTSaveError.h"

#include "DTDataFile.h"
#include "DTDoubleArray.h"
#include "DTMesh2D.h"

// Common utilities
#include "DTDoubleArrayOperators.h"
#include "DTProgress.h"
#include "DTTimer.h"
#include "DTUtilities.h"
#include "DTDictionary.h"

#include <math.h>

DTDoubleArray Computation(const DTMesh2D &M);

int main(int argc,const char *argv[])
{
    DTSetArguments(argc,argv);
    
    DTDataFile inputFile("Input.dtbin",DTFile::ReadOnly);
    // Read in the input variables.
    DTMesh2D Mesh_from_Array_from_Histogram_of_2D_Point_Collection_mapped;
    Read(inputFile,"Mesh from Array from Histogram of 2D Point Collection mapped",Mesh_from_Array_from_Histogram_of_2D_Point_Collection_mapped);
    
    // The computation.
    DTDoubleArray computed;
    clock_t t_before = clock();
    computed = Computation(Mesh_from_Array_from_Histogram_of_2D_Point_Collection_mapped);
    clock_t t_after = clock();
    double exec_time = double(t_after-t_before)/double(CLOCKS_PER_SEC);
    
    // Write the output.
    DTDataFile outputFile("Output.dtbin",DTFile::NewReadWrite);
    
    // Output from computation
    outputFile.Save(computed,"Var");
    outputFile.Save("Array","Seq_Var");
    
    // The execution time.
    outputFile.Save(exec_time,"ExecutionTime");
    outputFile.Save("Real Number","Seq_ExecutionTime");
    
    // The errors.
    DTSaveError(outputFile,"ExecutionErrors");
    outputFile.Save("StringList","Seq_ExecutionErrors");
    
    outputFile.SaveIndex();
    
    return 0;
}

DTDoubleArray Computation(const DTMesh2D &M)
{
    DTMutableDoubleArray toReturn(M.m());
    toReturn = 0.;
    DTDoubleArray data = M.DoubleData();
    double dy = M.Grid().dy();
    for (int m=0; m<M.m(); m++) {
        for (int n=0; n<M.n(); n++) {
            toReturn(m) += data(m, n)*n*dy;
        }
    }
    // Mesh_from_Array_from_Histogram_of_2D_Point_Collection_mapped.Grid() - DTMesh2DGrid - Underlying grid.
    // Mesh_from_Array_from_Histogram_of_2D_Point_Collection_mapped.Grid().dx(), dy() - x and y step size.
    // Mesh_from_Array_from_Histogram_of_2D_Point_Collection_mapped.Grid().Origin() - DTPoint2D.
    // Mesh_from_Array_from_Histogram_of_2D_Point_Collection_mapped.m(), n() - size in x and y direction.
    // Mesh_from_Array_from_Histogram_of_2D_Point_Collection_mapped.FloatPrecision(), Mesh_from_Array_from_Histogram_of_2D_Point_Collection_mapped.DoublePrecision() - check which precision.
    // Mesh_from_Array_from_Histogram_of_2D_Point_Collection_mapped = ConvertToFloat(Mesh_from_Array_from_Histogram_of_2D_Point_Collection_mapped) - free if already float.
    // Mesh_from_Array_from_Histogram_of_2D_Point_Collection_mapped = ConvertToDouble(Mesh_from_Array_from_Histogram_of_2D_Point_Collection_mapped) - free if already double.
    
    return toReturn;
}
