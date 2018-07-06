// Include DTSource.h if you want to include all the headers.
#include "Hypha.h"
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
#include "DTSeriesGroup.h"
string Computation(const DTDoubleArray &nuclei,const DTDoubleArray &GEMS,DTFile hfile,
                   const string &basePath);

int main(int argc,const char *argv[])
{
    DTSetArguments(argc,argv);
    
    DTDataFile inputFile("Input.dtbin",DTFile::ReadOnly);
    // Read in the input variables.
    DTDoubleArray nuclei = inputFile.ReadDoubleArray("nuclei");
    DTDoubleArray GEMS = inputFile.ReadDoubleArray("GEMS");
    DTFile hfile("hfile",DTFile::ReadOnly);
    string basePath = inputFile.ReadString("basePath");
    
    // The computation.
    string computed;
    clock_t t_before = clock();
    computed = Computation(nuclei,GEMS,hfile,basePath);
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

string Computation(const DTDoubleArray &nuclei,const DTDoubleArray &GEMS,DTFile hfile,
                   const string &basePath)
{
    DTDataFile inputFile(hfile);
    DTSeriesGroup<Hypha> S(inputFile, "Var");
    int Nsegs = S.HowManySaved();
    std::list<Hypha> hyphaList;
    for (int s=0; s<Nsegs;s++) {
        Hypha h = S.Get(s);
        hyphaList.push_back(h);
    }
    hyphaList = addTracks(hyphaList, GEMS, 0);
    hyphaList = addTracks(hyphaList, nuclei, 1);
    DTDataFile outputFile(basePath + "-hyphaeList-withTracks.dtbin", DTFile::NewReadWrite);
    DTSeriesGroup<Hypha> Sout(outputFile, "Var");
    for (int n=0; n<Nsegs; n++) {
        Hypha H = hyphaList.front();
        Sout.Add(H, n);
        hyphaList.pop_front();
    }
    outputFile.SaveIndex();
    string toReturn = basePath + "-hyphaeList-withTracks.dtbin";
    return toReturn;
}

