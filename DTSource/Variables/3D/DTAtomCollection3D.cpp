// Part of DTSource. Copyright 2004-2017. David Adalsteinsson.
// see http://www.visualdatatools.com/DTSource/license.html for more information.

#include "DTAtomCollection3D.h"

#include "DTError.h"
#include "DTRegion1D.h"
#include "DTRegion3D.h"

DTAtomCollection3D::DTAtomCollection3D(const DTFloatArray &loc,const DTUCharArray &atm)
{
    if (loc.IsEmpty())
        return;

    if (loc.m()!=3 || loc.o()!=1) {
        DTErrorMessage("DTAtomCollection3D(locations,atoms)","Location array needs to be a 3xN array.");
        return;
    }

    if (atm.m()!=loc.n() || atm.m()!=atm.Length()) {
        DTErrorMessage("DTAtomCollection3D(locations,atoms)","Atom length needs to equal number of points.");
        return;
    }

    locations = loc;
    atoms = atm;
}

DTAtomCollection3D::DTAtomCollection3D(const DTFloatArray &loc,const DTUCharArray &atm,
                   const DTIntArray &bnd,const DTFloatArray &q)
{
    if (loc.IsEmpty())
        return;

    if (loc.m()!=3 || loc.o()!=1) {
        DTErrorMessage("DTAtomCollection3D(locations,atoms,bonds,charges)",
                       "Location array needs to be a 3xN array.");
        return;
    }

    if (atm.m()!=loc.n() || atm.m()!=atm.Length()) {
        DTErrorMessage("DTAtomCollection3D(locations,atoms,bonds,charges)",
                       "Atom length needs to equal number of points.");
        return;
    }

    if (!bnd.IsEmpty()) {
        DTRegion1D valRange = ValueRange(bnd);
        if (valRange.minV<0 || valRange.maxV>=atm.Length()) {
            DTErrorMessage("DTAtomCollection3D(locations,atoms,bonds,charges)",
                           "Bonds are referring to atoms outside of list.");
            return;
        }
    }

    if (!q.IsEmpty()) {
        if (q.Length()!=q.m() || q.Length()!=atm.Length()) {
            DTErrorMessage("DTAtomCollection3D(locations,atoms,bonds,charges)",
                           "Invalid length of charge array.");
            return;
        }
    }

    locations = loc;
    atoms = atm;
    bonds = bnd;
    charges = q;
}

void DTAtomCollection3D::pinfo(void) const
{
    if (locations.IsEmpty()) {
        std::cerr << "Empty\n";
        return;
    }
    
    if (locations.n()==1)
        std::cerr << "1 atom";
    else
        std::cerr << locations.n() << " atoms";
    
    if (charges.NotEmpty())
        std::cerr << ", charges defined";
    
    if (bonds.NotEmpty()) {
        if (bonds.n()==1) {
            std::cerr << ", 1 bond";
        }
        else {
            std::cerr << ", " << bonds.n() << " bonds";
        }
    }
    
    std::cerr << std::flush;
}

DTAtomCollection3D SetBonds(const DTAtomCollection3D &A,const DTIntArray &B)
{
    return DTAtomCollection3D(A.Locations(),A.Atoms(),B,A.Charges());
}

DTAtomCollection3D SetCharges(const DTAtomCollection3D &A,const DTFloatArray &Q)
{
    return DTAtomCollection3D(A.Locations(),A.Atoms(),A.Bonds(),Q);
}

DTRegion3D BoundingBox(const DTAtomCollection3D &coll)
{
    return BoundingBox3D(coll.Locations());
}

void Read(const DTDataStorage &input,const std::string &name,DTAtomCollection3D &toReturn)
{
    DTFloatArray locations = input.ReadFloatArray(name);
    DTUCharArray atoms = input.ReadUCharArray(name+"_A");

    toReturn = DTAtomCollection3D(locations,atoms);
    if (toReturn.IsEmpty())
        return;

    if (input.Contains(name+"_B")) {
        DTIntArray conn = input.ReadIntArray(name+"_B");
        toReturn = SetBonds(toReturn,conn);
    }

    if (input.Contains(name+"_Q")) {
        DTFloatArray Q = input.ReadFloatArray(name+"_Q");
        toReturn = SetCharges(toReturn,Q);
    }
}

void Write(DTDataStorage &output,const std::string &name,const DTAtomCollection3D &toWrite)
{
    output.Save(toWrite.Locations(),name);
    output.Save(toWrite.Atoms(),name+"_T");
    if (toWrite.Bonds().NotEmpty())
        output.Save(toWrite.Bonds(),name+"_B");
    if (toWrite.Charges().NotEmpty())
        output.Save(toWrite.Charges(),name+"_Q");
}

void WriteOne(DTDataStorage &output,const std::string &name,const DTAtomCollection3D &toWrite)
{
    Write(output,name,toWrite);
    Write(output,"Seq_"+name,"3D Atom Collection");
    output.Flush();
}

