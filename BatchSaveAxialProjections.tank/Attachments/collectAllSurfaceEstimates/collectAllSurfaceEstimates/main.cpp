// Include DTSource.h if you want to include all the headers.
#include "Hypha.h"
#include "DTArguments.h"
#include "DTSaveError.h"

#include "DTDataFile.h"
#include "DTDoubleArray.h"
#include "DTFile.h"

// Common utilities
#include "DTDoubleArrayOperators.h"
#include "DTProgress.h"
#include "DTTimer.h"
#include "DTUtilities.h"
#include "DTDictionary.h"
#include "DTDataFile.h"
#include "DTSeriesGroup.h"
#include <math.h>

DTDoubleArray Computation(double sigma,double NF,double window,DTFile hfile);

int main(int argc,const char *argv[])
{
    DTSetArguments(argc,argv);

    DTDataFile inputFile("Input.dtbin",DTFile::ReadOnly);
    // Read in the input variables.
    double sigma = inputFile.ReadNumber("sigma");
    double NF = inputFile.ReadNumber("NF");
    double window = inputFile.ReadNumber("window");
    DTFile hfile("hfile",DTFile::ReadOnly);

    // The computation.
    DTDoubleArray computed;
    clock_t t_before = clock();
    computed = Computation(sigma,NF,window,hfile);
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

DTDoubleArray Computation(double sigma,double NF,double window,DTFile hfile)
{
    int range = 20;
    DTDataFile inputFile(hfile);
    DTSeriesGroup<Hypha> S(inputFile, "Var");
    int Nsegs = S.HowManySaved();
    int Npoints = 0;
    for (int s=0; s<Nsegs;s++) {
        Hypha h = S.Get(s);
        Npoints += h.surface.NumberOfPoints();
    }
    DTMutableDoubleArray values(NF, Npoints);
    int Pcount = 0;
    for (int s=0; s<Nsegs;s++) {
        Hypha h = S.Get(s);
        int Np = h.surface.NumberOfPoints();
        DTDoubleArray vals = diffusivitySurfaceMap(h, window, s, NF, sigma, range);
        for (int n=0; n<Np; n++) {
            for (int t=0; t<NF; t++) {
                values(t, Pcount + n) = vals(t, n);
            }
        }
        Pcount += Np;
    }
    return values;
}
