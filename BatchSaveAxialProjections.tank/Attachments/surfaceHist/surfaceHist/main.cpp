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

#include <math.h>
#include "DTSeriesGroup.h"

DTDoubleArray Computation(int window, int seg,int NF, double sigma, DTFile hfile);

int main(int argc,const char *argv[])
{
    DTSetArguments(argc,argv);
    
    DTDataFile inputFile("Input.dtbin",DTFile::ReadOnly);
    // Read in the input variables.
    double window = inputFile.ReadNumber("window");
    double seg = inputFile.ReadNumber("seg");
    double NF = inputFile.ReadNumber("NF");
    double sigma = inputFile.ReadNumber("sigma");
    DTFile hfile("hfile",DTFile::ReadOnly);
    //string hfileName = inputFile.ReadString("hfileName");
    
    // The computation.
    DTDoubleArray computed;
    clock_t t_before = clock();
    computed = Computation(window,seg,NF,sigma,hfile);
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

DTDoubleArray Computation(int window, int seg,int NF, double sigma, DTFile hfile)
{
    
    int range = 10;
    // DTDataFile inputFile("/Users/jaynewby/Dropbox/Gladfelter/GEMS 1_17/1010/HyphaeList.dtbin", DTFile::ReadOnly);
    // DTDataFile inputFile(hfileName, DTFile::ReadOnly);
    DTDataFile inputFile(hfile);
    DTSeriesGroup<Hypha> S(inputFile, "Var");
    Hypha h = S.Get(seg);
    DTDoubleArray toReturn = diffusivitySurfaceMap(h, window, seg, NF, sigma, range);
    return toReturn;
}
