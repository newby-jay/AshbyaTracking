// Include DTSource.h if you want to include all the headers.
#include "Hypha.h"
#include "DTArguments.h"
#include "DTSaveError.h"

#include "DTDataFile.h"
#include "DTFile.h"
#include "DTPointCollection3D.h"

// Common utilities
#include "DTDoubleArrayOperators.h"
#include "DTProgress.h"
#include "DTTimer.h"
#include "DTUtilities.h"
#include "DTDictionary.h"

#include <math.h>
#include "DTSeriesGroup.h"
DTDoubleArray Computation(DTFile hfile,const string &kind);

int main(int argc,const char *argv[])
{
    DTSetArguments(argc,argv);
    
    DTDataFile inputFile("Input.dtbin",DTFile::ReadOnly);
    // Read in the input variables.
    DTFile hfile("hfile",DTFile::ReadOnly);
    string kind = inputFile.ReadString("kind");
    
    // The computation.
    DTDoubleArray computed;
    clock_t t_before = clock();
    computed = Computation(hfile,kind);
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

DTDoubleArray Computation(DTFile hfile,const string &kind)

{
    DTDataFile inputFile(hfile);
    DTSeriesGroup<Hypha> S(inputFile, "Var");
    int Nsegs = S.HowManySaved();
    int Npoints = 0;
    for (int s=0; s<Nsegs;s++) {
        Hypha h = S.Get(s);
        if (kind[0] == 'G')
            Npoints += h.GEMS.points.NumberOfPoints();
        else
            Npoints += h.nuclei.points.NumberOfPoints();
    }
    DTMutableDoubleArray pointData(6, Npoints);
    int Pcount = 0;
    int globalPnumber = 0;
    for (int s=0; s<Nsegs;s++) {
        Hypha h = S.Get(s);
        int Np = 0;
        if (kind[0] == 'G')
            Np = h.GEMS.points.NumberOfPoints();
        else
            Np = h.nuclei.points.NumberOfPoints();
        if (kind[0] == 'G') {
            DTDoubleArray gpoints = h.GEMS.points.DoubleData();
            DTIntArray gtime = h.GEMS.time;
            DTDoubleArray gradius = h.GEMS.radius;
            DTIntArray gparticle = h.GEMS.pointToParticle;
            int pnumberMax = 0;
            for (int n=0; n<Np; n++) {
                pointData(0, Pcount + n) = gpoints(0, n);
                pointData(1, Pcount + n) = gpoints(1, n);
                pointData(2, Pcount + n) = gpoints(2, n);
                pointData(3, Pcount + n) = gtime(n);
                pointData(4, Pcount + n) = gradius(n);
                if (gparticle(n) > pnumberMax) pnumberMax = gparticle(n);
                pointData(5, Pcount + n) = globalPnumber + gparticle(n);
            }
            globalPnumber += pnumberMax + 1;
        }
        else {
            DTDoubleArray npoints = h.nuclei.points.DoubleData();
            DTIntArray ntime = h.nuclei.time;
            DTDoubleArray nradius = h.nuclei.radius;
            DTIntArray nparticle = h.nuclei.pointToParticle;
            for (int n=0; n<Np; n++) {
                pointData(0, Pcount + n) = npoints(0, n);
                pointData(1, Pcount + n) = npoints(1, n);
                pointData(2, Pcount + n) = npoints(2, n);
                pointData(3, Pcount + n) = ntime(n);
                pointData(4, Pcount + n) = nradius(n);
                pointData(5, Pcount + n) = nparticle(n);
            }
        }
        Pcount += Np;
    }
    return pointData;
}
