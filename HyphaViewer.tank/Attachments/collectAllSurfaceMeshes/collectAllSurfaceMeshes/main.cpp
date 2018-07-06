// Include DTSource.h if you want to include all the headers.
#include "Hypha.h"
#include "DTArguments.h"
#include "DTSaveError.h"

#include "DTDataFile.h"
#include "DTFile.h"
#include "DTSurfaceValues3D.h"


// Common utilities
#include "DTDoubleArrayOperators.h"
#include "DTProgress.h"
#include "DTTimer.h"
#include "DTUtilities.h"
#include "DTDictionary.h"

#include <math.h>
#include "DTSeriesGroup.h"
DTSurface3D Computation(DTFile hfile);

int main(int argc,const char *argv[])
{
    DTSetArguments(argc,argv);
    
    DTDataFile inputFile("Input.dtbin",DTFile::ReadOnly);
    // Read in the input variables.
    DTFile hfile("hfile",DTFile::ReadOnly);

    
    // The computation.
    DTSurface3D computed;
    clock_t t_before = clock();
    computed = Computation(hfile);
    clock_t t_after = clock();
    double exec_time = double(t_after-t_before)/double(CLOCKS_PER_SEC);
    
    // Write the output.
    DTDataFile outputFile("Output.dtbin",DTFile::NewReadWrite);
    
    // Output from computation
    Write(outputFile,"Var",computed);
    outputFile.Save("Surface3D","Seq_Var");
    
    // The execution time.
    outputFile.Save(exec_time,"ExecutionTime");
    outputFile.Save("Real Number","Seq_ExecutionTime");
    
    // The errors.
    DTSaveError(outputFile,"ExecutionErrors");
    outputFile.Save("StringList","Seq_ExecutionErrors");
    
    outputFile.SaveIndex();
    
    return 0;
}

DTSurface3D Computation(DTFile hfile)
{
    DTDataFile inputFile(hfile);
    DTSeriesGroup<Hypha> S(inputFile, "Var");
    int Nsegs = S.HowManySaved();
    int Npoints = 0, Ntriangles = 0;
    for (int s=0; s<Nsegs;s++) {
        Hypha h = S.Get(s);
        Npoints += h.surface.NumberOfPoints();
        Ntriangles += h.surface.NumberOfTriangles();
    }
    DTMutableDoubleArray pointData(3, Npoints);
    DTMutableIntArray triangles(3, Ntriangles);
    int Pcount = 0, Tcount = 0;
    for (int s=0; s<Nsegs;s++) {
        Hypha h = S.Get(s);
        int Np = h.surface.NumberOfPoints(), Nt = h.surface.NumberOfTriangles();
        DTDoubleArray points = h.surface.Points().DoubleData();
        DTIntArray tris = h.surface.Connections();
        for (int n=0; n<Np; n++) {
            pointData(0, Pcount + n) = points(0, n);
            pointData(1, Pcount + n) = points(1, n);
            pointData(2, Pcount + n) = points(2, n);
        }
        for (int n=0; n<Nt; n++) {
            triangles(0, Tcount + n) = tris(0, n) + Pcount;
            triangles(1, Tcount + n) = tris(1, n) + Pcount;
            triangles(2, Tcount + n) = tris(2, n) + Pcount;
        }
        Pcount += Np;
        Tcount += Nt;
    }
    DTPointCollection3D newPoints(pointData);
    DTSurface3D toReturn(triangles, newPoints);
    return toReturn;
}
