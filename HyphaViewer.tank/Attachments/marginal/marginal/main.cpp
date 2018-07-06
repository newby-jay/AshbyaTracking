// Include DTSource.h if you want to include all the headers.

#include "DTArguments.h"
#include "DTSaveError.h"

#include "DTDataFile.h"
#include "DTDoubleArray.h"

// Common utilities
#include "DTDoubleArrayOperators.h"
#include "DTProgress.h"
#include "DTTimer.h"
#include "DTUtilities.h"
#include "DTDictionary.h"

#include <math.h>

DTDoubleArray Computation(const DTDoubleArray &H);

int main(int argc,const char *argv[])
{
    DTSetArguments(argc,argv);

    DTDataFile inputFile("Input.dtbin",DTFile::ReadOnly);
    // Read in the input variables.
    DTDoubleArray H = inputFile.ReadDoubleArray("H");

    // The computation.
    DTDoubleArray computed;
    clock_t t_before = clock();
    computed = Computation(H);
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

DTDoubleArray Computation(const DTDoubleArray &H)
{
    DTMutableDoubleArray toReturn(H.m(), H.n());
    DTMutableDoubleArray Sn(H.m());
    Sn = 0;
    for (int m=0; m<H.m(); m++) {
        for (int n=0; n<H.n(); n++) {
            Sn(m) += H(m, n);
        }
    }
    for (int m=0; m<H.m(); m++) {
        for (int n=0; n<H.n(); n++) {
            toReturn(m, n) = H(m, n)/Sn(m);
        }
    }
    return toReturn;
}
