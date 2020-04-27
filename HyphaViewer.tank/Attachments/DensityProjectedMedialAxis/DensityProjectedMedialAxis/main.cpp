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
//////////////////////////////////////////////////////////////////////////////
//    DT_RetGroup
//////////////////////////////////////////////////////////////////////////////

struct DT_RetGroup {
    DTDoubleArray density;
    DTDoubleArray arcLength;
    DTIntArray nucleiPosition;
    
    void pinfo(void) const;
    void pinfoIndent(string) const;
    
    static void WriteStructure(DTDataStorage &,string);
};

void DT_RetGroup::pinfo(void) const
{
    pinfoIndent("");
}

void DT_RetGroup::pinfoIndent(string pad) const
{
    cerr << pad << "density = "; density.pinfo();
    cerr << pad << "arcLength = "; arcLength.pinfo();
}

void DT_RetGroup::WriteStructure(DTDataStorage &output,string name)
{
    output.Save("density",name+"_1N");
    output.Save("Array",name+"_1T");
    
    output.Save("arcLength",name+"_2N");
    output.Save("Array",name+"_2T");
    
    output.Save("nucleiPosition",name+"_3N");
    output.Save("Array",name+"_3T");
    
    output.Save(3,name+"_N");
    output.Save("Group",name);
}

extern void Write(DTDataStorage &,string name,const DT_RetGroup &);

void Write(DTDataStorage &output,string name,const DT_RetGroup &var)
{
    Write(output,name+"_density",var.density);
    Write(output,name+"_arcLength",var.arcLength);
    Write(output,name+"_nucleiPosition",var.nucleiPosition);
    Write(output,name,DTDoubleArray()); // So that DataTank can see the variable.
}

//////////////////////////////////////////////////////////////////////////////
//    End of structure definitions.
//////////////////////////////////////////////////////////////////////////////

DT_RetGroup Computation(double sigma,double NF,double window,DTFile hfile,double seg,
                        double range);

int main(int argc,const char *argv[])
{
    DTSetArguments(argc,argv);
    
    DTDataFile inputFile("Input.dtbin",DTFile::ReadOnly);
    // Read in the input variables.
    double sigma = inputFile.ReadNumber("sigma");
    double NF = inputFile.ReadNumber("NF");
    double window = inputFile.ReadNumber("window");
    DTFile hfile("hfile",DTFile::ReadOnly);
    double seg = inputFile.ReadNumber("seg");
    double range = inputFile.ReadNumber("range");
    
    // Write the output.
    DTDataFile outputFile("Output.dtbin",DTFile::NewReadWrite);
    
    DT_RetGroup computed;
    
    // The computation.
    clock_t t_before = clock();
    computed = Computation(sigma,NF,window,hfile,seg,range);
    clock_t t_after = clock();
    double exec_time = double(t_after-t_before)/double(CLOCKS_PER_SEC);
    
    outputFile.Save("Group","Seq_Var");
    DT_RetGroup::WriteStructure(outputFile,"SeqInfo_Var");
    Write(outputFile,"Var",computed);
    
    // The execution time.
    outputFile.Save(exec_time,"ExecutionTime");
    outputFile.Save("Real Number","Seq_ExecutionTime");
    
    // The errors.
    DTSaveError(outputFile,"ExecutionErrors");
    outputFile.Save("StringList","Seq_ExecutionErrors");
    
    outputFile.SaveIndex();
    
    return 0;
}

DT_RetGroup Computation(double sigma,double NF,double window,DTFile hfile,double seg,
                        double range)
{
    DTDataFile inputFile(hfile);
    DTSeriesGroup<Hypha> S(inputFile, "Var");
    //int Nsegs = S.HowManySaved();
    Hypha h = S.Get(seg);
    std::pair<DTDoubleArray, DTIntArray> orderedData = getSkelToOrdered(h);
    DTDoubleArray density = densitySkelHist(h, orderedData.second, window, NF, sigma, range);
    
    Tracks nuc = h.nuclei;
    int Npoints = nuc.points.NumberOfPoints();
    DTMutableIntArray NucOrderedInds(Npoints);
    for (int n=0;n<Npoints;n++) {
        NucOrderedInds(n) = orderedData.second(nuc.skelAndSurfInd(n, 0));
    }
    
    DT_RetGroup toReturn;
    toReturn.arcLength = orderedData.first;
    toReturn.density = density;
    toReturn.nucleiPosition = NucOrderedInds;
    return toReturn;
}
