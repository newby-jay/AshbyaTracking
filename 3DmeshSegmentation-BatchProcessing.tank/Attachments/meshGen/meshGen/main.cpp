// Include DTSource.h if you want to include all the headers.


#include "HyphaTools.h"
#include "Hypha.h"
#include "Ashbya.h"


// Common utilities
#include "DTArguments.h"
#include "DTSaveError.h"

#include "DTDataFile.h"
#include "DTDoubleArray.h"
#include "DTSurface3D.h"
#include "DTTetrahedralGrid3D.h"
#include "DTSeriesGroup.h"
#include "DTDoubleArrayOperators.h"
#include "DTProgress.h"
#include "DTTimer.h"
#include "DTUtilities.h"
#include "DTDictionary.h"


#include <math.h>
#include <list>
#include <fstream>
#include <vector>


string Computation(const DTDoubleArray &pointsWithSegments,const string &fileBase);

int main(int argc,const char *argv[])
{
    DTSetArguments(argc,argv);
    
    DTDataFile inputFile("Input.dtbin",DTFile::ReadOnly);
    // Read in the input variables.
    DTDoubleArray pointsWithSegments = inputFile.ReadDoubleArray("pointsWithSegments");
    string fileBase = inputFile.ReadString("fileBase");
    
    // The computation.
    string computed;
    clock_t t_before = clock();
    computed = Computation(pointsWithSegments,fileBase);
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

string Computation(const DTDoubleArray &pointsWithSegments,const string &fileBase)
{
    std::list<Hypha> outList = collectHyphae(pointsWithSegments);
    //outList = addTracks(outList, GEMS, 0);
    //outList = addTracks(outList, nuclei, 1);
    string toReturn = fileBase + "-hyphaeList-SurfacesOnly.dtbin";
    DTDataFile outputFile(toReturn, DTFile::NewReadWrite);
    DTSeriesGroup<Hypha> S(outputFile, "Var");
    //S.SetSaveEachTime(true);
    int Nsegs = outList.size();
    for (int n=0; n<Nsegs; n++) {
        Hypha H = outList.front();
        S.Add(H, n);
        outList.pop_front();
    }
    outputFile.SaveIndex();
    return toReturn;
}

