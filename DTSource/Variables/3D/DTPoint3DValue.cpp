// Part of DTSource. Copyright 2004-2017. David Adalsteinsson.
// see http://www.visualdatatools.com/DTSource/license.html for more information.

#include "DTPoint3DValue.h"

#include "DTDoubleArray.h"
#include "DTError.h"

void DTPoint3DValue::pinfo(void) const
{
    std::cerr << "(" << point.x << ", " << point.y << ", " << point.z << ") - " << value << std::flush;
}

void Read(const DTDataStorage &input,const std::string &name,DTPoint3DValue &toReturn)
{
    DTDoubleArray theArr = input.ReadDoubleArray(name);
    if (theArr.Length()!=4) {
        DTErrorMessage("ReadFromArray(DTPoint3DValue)","Invalid length of array.");
        toReturn = DTPoint3DValue();
    }
    else {
        toReturn = DTPoint3DValue(DTPoint3D(theArr(0),theArr(1),theArr(2)),theArr(3));
    }
}

void Write(DTDataStorage &output,const std::string &name,const DTPoint3DValue &theVar)
{
    DTMutableDoubleArray theArr(4);
    theArr(0) = theVar.point.x;
    theArr(1) = theVar.point.y;
    theArr(2) = theVar.point.z;
    theArr(3) = theVar.value;
    output.Save(theArr,name);
}

void WriteOne(DTDataStorage &output,const std::string &name,const DTPoint3DValue &toWrite)
{
    Write(output,name,toWrite);
    Write(output,"Seq_"+name,"3D Point Value");
    output.Flush();
}

