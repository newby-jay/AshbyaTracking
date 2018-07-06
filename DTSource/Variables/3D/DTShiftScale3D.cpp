// Part of DTSource. Copyright 2004-2017. David Adalsteinsson.
// see http://www.visualdatatools.com/DTSource/license.html for more information.

#include "DTShiftScale3D.h"

#include "DTDoubleArray.h"
#include "DTError.h"

void DTShiftScale3D::pinfo(void) const
{
    std::cerr << "Shift = (" << shift.x << ", " << shift.y << ", " << shift.z << "), scale = (" << scaleX << ", " << scaleY << ", " << scaleZ << ")\n" << std::flush;
}

DTShiftScale3D ShiftBy(DTPoint3D P)
{
	return DTShiftScale3D(P,1,1,1);
}

void Read(const DTDataStorage &input,const std::string &name,DTShiftScale3D &toReturn)
{
    DTDoubleArray theArr = input.ReadDoubleArray(name);
    if (theArr.Length()!=6) {
        DTErrorMessage("ReadFromArray(DTShiftScale3D)","Invalid length of array.");
        toReturn = DTShiftScale3D();
    }
    else {
        toReturn = DTShiftScale3D(DTPoint3D(theArr(0),theArr(1),theArr(2)),
                                  theArr(3),theArr(4),theArr(5));
    }
}

void Write(DTDataStorage &output,const std::string &name,const DTShiftScale3D &theVar)
{
    DTMutableDoubleArray theArr(6);
    theArr(0) = theVar.Shift().x;
    theArr(1) = theVar.Shift().y;
    theArr(2) = theVar.Shift().z;
    theArr(3) = theVar.ScaleX();
    theArr(4) = theVar.ScaleY();
    theArr(5) = theVar.ScaleZ();
    output.Save(theArr,name);
}

void WriteOne(DTDataStorage &output,const std::string &name,const DTShiftScale3D &toWrite)
{
    Write(output,name,toWrite);
    Write(output,"Seq_"+name,"3D Shift And Scale");
    output.Flush();
}

