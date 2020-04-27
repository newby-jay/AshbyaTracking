// Include DTSource.h if you want to include all the headers.

#include "DTArguments.h"
#include "DTSaveError.h"

#include "DTDataFile.h"

// Common utilities
#include "DTDoubleArrayOperators.h"
#include "DTProgress.h"
#include "DTTimer.h"
#include "DTUtilities.h"
#include "DTDictionary.h"
#include "DTSeriesRealNumber.h"
#include <math.h>

string Computation(double Nt);

int main(int argc,const char *argv[])
{
    DTSetArguments(argc,argv);
    
    DTDataFile inputFile("Input.dtbin",DTFile::ReadOnly);
    // Read in the input variables.
    double Nt = inputFile.ReadNumber("Nt");
    
    // The computation.
    string computed;
    clock_t t_before = clock();
    computed = Computation(Nt);
    clock_t t_after = clock();
    double exec_time = double(t_after-t_before)/double(CLOCKS_PER_SEC);
    
    // Write the output.
    DTDataFile outputFile("Output.dtbin",DTFile::NewReadWrite);
    
    // Output from computation
    outputFile.Save(computed,"Var");
    outputFile.Save("String","Seq_Var");
    
    // The execution time.
    outputFile.Save(exec_time,"ExecutionTime");
    outputFile.Save("Real Number","Seq_ExecutionTime");
    
    // The errors.
    DTSaveError(outputFile,"ExecutionErrors");
    outputFile.Save("StringList","Seq_ExecutionErrors");
    
    outputFile.SaveIndex();
    
    return 0;
}

string Computation(double Ntd)
{
    int Nt = Ntd;
    DTDataFile seriesOutput("/tmp/timeFromVariable.dtbin", DTFile::NewReadWrite);
    DTSeriesRealNumber time(seriesOutput, "time");
    for (int n=0;n<Nt;n++) {
        double t = n;
        time.Add(t, n);
    }
    return "/tmp/timeFromVariable.dtbin";
}

