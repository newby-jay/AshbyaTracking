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
//    HyphaAxisDensity
//////////////////////////////////////////////////////////////////////////////

struct HyphaAxisDensity {
    double hyphaID;
    double branchID;
    DTDoubleArray density;
    DTDoubleArray arcLength;
    DTIntArray nucleiMedialAxisPosition;
    DTDoubleArray axisPosition;
    DTDoubleArray radius;
    DTDoubleArray diffusivity;
    DTDoubleArray confinement;
    DTDoubleArray minProjection;
    
    void pinfo(void) const;
    void pinfoIndent(string) const;
    
    static void WriteStructure(DTDataStorage &,string);
};

void HyphaAxisDensity::pinfo(void) const
{
    pinfoIndent("");
}

void HyphaAxisDensity::pinfoIndent(string pad) const
{
    cerr << pad << "hyphaID = " << hyphaID << endl;
    cerr << pad << "branchID = " << branchID << endl;
    cerr << pad << "density = "; density.pinfo();
    cerr << pad << "arcLength = "; arcLength.pinfo();
    cerr << pad << "nucleiMedialAxisPosition = "; nucleiMedialAxisPosition.pinfo();
}

void HyphaAxisDensity::WriteStructure(DTDataStorage &output,string name)
{
    output.Save("hyphaID",name+"_1N");
    output.Save("Real Number",name+"_1T");
    
    output.Save("branchID",name+"_2N");
    output.Save("Real Number",name+"_2T");
    
    output.Save("density",name+"_3N");
    output.Save("Array",name+"_3T");
    
    output.Save("arcLength",name+"_4N");
    output.Save("Array",name+"_4T");
    
    output.Save("nucleiMedialAxisPosition",name+"_5N");
    output.Save("Array",name+"_5T");

    output.Save("axisPosition",name+"_6N");
    output.Save("Array",name+"_6T");
    
    output.Save("radius",name+"_7N");
    output.Save("Array",name+"_7T");
    
    output.Save("diffusivity",name+"_8N");
    output.Save("Array",name+"_8T");
    
    output.Save("confinement",name+"_9N");
    output.Save("Array",name+"_9T");
    
    output.Save("minProjection",name+"_10N");
    output.Save("Array",name+"_10T");
    
    output.Save(10,name+"_N");
    output.Save("Group",name);
}

extern void Write(DTDataStorage &,string name,const HyphaAxisDensity &);

void Write(DTDataStorage &output,string name,const HyphaAxisDensity &var)
{
    output.Save(var.hyphaID,name+"_hyphaID");
    output.Save(var.branchID,name+"_branchID");
    Write(output,name+"_density",var.density);
    Write(output,name+"_arcLength",var.arcLength);
    Write(output,name+"_nucleiMedialAxisPosition",var.nucleiMedialAxisPosition);
    Write(output,name+"_axisPosition",var.axisPosition);
    Write(output,name+"_radius",var.radius);
    Write(output,name+"_diffusivity",var.diffusivity);
    Write(output,name+"_confinement",var.confinement);
    Write(output,name+"_minProjection",var.minProjection);
//    Write(output,name+"_",var.);
    Write(output,name,DTDoubleArray()); // So that DataTank can see the variable.
}

//////////////////////////////////////////////////////////////////////////////
//    End of structure definitions.
//////////////////////////////////////////////////////////////////////////////


string Computation(double sigma,double NF,double window,DTFile hfile,double range,
                   const string &basePath);

int main(int argc,const char *argv[])
{
    DTSetArguments(argc,argv);
    
    DTDataFile inputFile("Input.dtbin",DTFile::ReadOnly);
    // Read in the input variables.
    double sigma = inputFile.ReadNumber("sigma");
    double NF = inputFile.ReadNumber("NF");
    double window = inputFile.ReadNumber("window");
    DTFile hfile("hfile",DTFile::ReadOnly);
    double range = inputFile.ReadNumber("range");
    string basePath = inputFile.ReadString("basePath");
    
    // The computation.
    string computed;
    clock_t t_before = clock();
    computed = Computation(sigma,NF,window,hfile,range,basePath);
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

string Computation(double sigma,double NF,double window,DTFile hfile,double range,
                   const string &basePath)
{
    DTDataFile inputFile(hfile);
    DTSeriesGroup<Hypha> S(inputFile, "Var");
    std::list< std::list<HyphaAxisDensity> > HADList;
    int Nsegs = S.HowManySaved();
    //// Loop over all hypha saved in the file
    for (int n=0; n<Nsegs; n++) {
        Hypha h = S.Get(n);
        std::pair<std::list<DTDoubleArray>, DTIntArray> orderedData = getOrderedMedialAxis(h);
        MedialAxisValues histOut = densityDiffusivityProjectedOnMedialAxis(h, orderedData.second, window, NF, sigma, range);
        DTDoubleArray minProjection = surfaceMapMinProjectedOnMedialAxis(h, orderedData.second, 75, NF, 6.65, range);
        DTDoubleArray density = histOut.density;
        Tracks nuc = h.nuclei;
        int Npoints = nuc.points.NumberOfPoints();
        DTMutableIntArray NucOrderedInds(Npoints);
        for (int m=0;m<Npoints;m++) {
            NucOrderedInds(m) = orderedData.second(nuc.skelAndSurfInd(m, 0));
        }
        DTMutableDoubleArray positions(h.hyphaGraph.NskelPoints, 3);
        for (int m=0;m<orderedData.second.m();m++) {
            DTPoint3D point = h.skeleton(m);
            if (orderedData.second(m) < 0) continue;
            positions(orderedData.second(m), 0) = point.x;
            positions(orderedData.second(m), 1) = point.y;
            positions(orderedData.second(m), 2) = point.z;
        }
        std::list<HyphaAxisDensity> HList;
        std::list<DTDoubleArray> ALlist = orderedData.first;
        int offset = 0;
        //// Loop over each branch in the hypha
        int Nbranches = ALlist.size();
        for (int m=0; m<Nbranches;m++) {
            DTDoubleArray AL = ALlist.front();
            ALlist.pop_front();
            int NinBranch = AL.m();
            DTMutableDoubleArray densityBranch(NF, NinBranch);
            DTMutableDoubleArray diffusivityBranch(NF, NinBranch);
            DTMutableDoubleArray confinementBranch(NF, NinBranch);
            DTMutableDoubleArray minProjectionBranch(NF, NinBranch);
            DTMutableDoubleArray radiusBranch(NinBranch);
            DTMutableDoubleArray positionsBranch(NinBranch, 3);
            //// density, diffusivity, and radius for each axis point in the current branch
            for (int l=0;l<NinBranch;l++) {
                radiusBranch(l) = histOut.radius(offset + l);
                positionsBranch(l, 0) = positions(offset + l, 0);
                positionsBranch(l, 1) = positions(offset + l, 1);
                positionsBranch(l, 2) = positions(offset + l, 2);
                for (int t=0;t<NF;t++) {
                    densityBranch(t, l) = density(t, offset + l);
                    diffusivityBranch(t, l) = histOut.diffusivity(t, offset + l);
                    confinementBranch(t, l) = histOut.confinement(t, offset + l);
                    minProjectionBranch(t, l) = minProjection(t, offset + l);
                }
            }
            //// record relative index of closest medial axis point to each nucleus
            std::list<int> nucleiBranchList;
            std::list<int> nucleiTimeBranchList;
            for (int q=0;q<Npoints;q++) {
                int nq = NucOrderedInds(q);
                if ((nq >= offset) && (nq < offset + NinBranch)) {
                    nucleiBranchList.push_back(nq - offset);
                    nucleiTimeBranchList.push_back(nuc.time(q));
                }
            }
            // transfer list data to DT array
            DTMutableIntArray nucleiBranch(nucleiBranchList.size(), 2);
            for (int q=0;q<nucleiBranch.m();q++) {
                nucleiBranch(q, 0) = nucleiBranchList.front();
                nucleiBranchList.pop_front();
                nucleiBranch(q, 1) = nucleiTimeBranchList.front();
                nucleiTimeBranchList.pop_front();
            }
            offset += NinBranch;

            HyphaAxisDensity H;
            H.hyphaID = n + 1;
            H.branchID = m;
            H.arcLength = AL;
            H.density = densityBranch;
            H.nucleiMedialAxisPosition = nucleiBranch;
            H.axisPosition = positionsBranch;
            H.radius = radiusBranch;
            H.diffusivity = diffusivityBranch;
            H.confinement = confinementBranch;
            H.minProjection = minProjectionBranch;
            HList.push_back(H);
        }
        HADList.push_back(HList);
    }
    string toReturn = basePath + "-hyphaAxisDensity.dtbin";
    DTDataFile outputFile(basePath + "-hyphaAxisDensity.dtbin", DTFile::NewReadWrite);
    DTSeriesGroup<HyphaAxisDensity> Sout(outputFile, "Var");
    int j = 0;
    for (int n=0; n<Nsegs; n++) {
        std::list<HyphaAxisDensity> H = HADList.front();
        HADList.pop_front();
        int Nbranches = H.size();
        for (int m=0; m<Nbranches;m++) {
            HyphaAxisDensity Hm = H.front();
            H.pop_front();
            Sout.Add(Hm, j);
            j++;
        }
    }
    outputFile.SaveIndex();
    return toReturn;
}
