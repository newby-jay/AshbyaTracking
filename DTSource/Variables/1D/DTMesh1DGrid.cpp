// Part of DTSource. Copyright 2004-2017. David Adalsteinsson.
// see http://www.visualdatatools.com/DTSource/license.html for more information.

#include "DTMesh1DGrid.h"

#include "DTError.h"
#include "DTIntArray.h"
#include "DTDoubleArray.h"
#include "DTRegion1D.h"

#include <math.h>

DTMesh1DGrid::DTMesh1DGrid(double origin,double hin,int mv)
: _m(0), _origin(0.0), _h(1.0) {
    if (mv<0) {
        DTErrorMessage("DTMeshGrid(origin,h,m)","One of the size is negative.");
        return;
    }
    if (hin<=0) {
        DTErrorMessage("DTMeshGrid(origin,h,m)","The step size is negative.");
        return;
    }
    _m = mv;
    _origin = origin;
    _h = hin;
}

void DTMesh1DGrid::pinfo(void) const
{
    std::cerr << "origin = " << _origin << ", h = " << _h << ", size = " << _m << std::flush;
}

double DTMesh1DGrid::FromSpaceToGrid(double v)
{
    double toReturn = (v-_origin)/_h;
    if (fabs(toReturn-round(toReturn))<1e-10)
        toReturn = round(toReturn); // Deal with rounding errors.
    return toReturn;
}


bool operator==(const DTMesh1DGrid &A,const DTMesh1DGrid &B)
{
    return (A.Origin()==B.Origin() && A.h()==B.h());
}

bool operator!=(const DTMesh1DGrid &A,const DTMesh1DGrid &B)
{
    return (A.Origin()!=B.Origin() && A.h()!=B.h());
}

void Read(const DTDataStorage &input,const std::string &name,DTMesh1DGrid &toReturn)
{
    DTDoubleArray base;

    base = input.ReadDoubleArray(name);
    if (base.Length()!=2) {
        DTErrorMessage("ReadFromArray(DTMesh1DGrid)","Incorrect size array.");
        toReturn = DTMesh1DGrid();
        return;
    }

    DTIntArray size;
    std::string sizeName = name+"_size";
    if (input.Contains(sizeName))
        size = input.ReadIntArray(sizeName);
    if (size.Length()>0 && size.Length()!=1) {
        DTErrorMessage("ReadFromArray(DTMesh1DGrid)","Incorrect size array.");
        toReturn = DTMesh1DGrid();
        return;
    }

    if (size.Length()==0)
        toReturn = DTMesh1DGrid(base(0),base(1),0);
    else
        toReturn = DTMesh1DGrid(base(0),base(1),size(0));
}

void Write(DTDataStorage &output,const std::string &name,const DTMesh1DGrid &theVar)
{
    // First save [xzero yzero dx dy]
    DTMutableDoubleArray base(2);
    base(0) = theVar.Origin();
    base(1) = theVar.h();

    // Save the size if it's not zero.
    if (theVar.m()) {
        DTMutableIntArray size(1);
        size(0) = theVar.m();
        output.Save(size,name+"_size");
    }

    output.Save(base,name);
}

void WriteOne(DTDataStorage &output,const std::string &name,const DTMesh1DGrid &toWrite)
{
    Write(output,name,toWrite);
    Write(output,"Seq_"+name,"1D Mesh Grid");
    output.Flush();
}

