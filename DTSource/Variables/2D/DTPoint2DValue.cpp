// Part of DTSource. Copyright 2004-2017. David Adalsteinsson.
// see http://www.visualdatatools.com/DTSource/license.html for more information.

#include "DTPoint2DValue.h"

#include "DTDoubleArray.h"
#include "DTError.h"

void DTPoint2DValue::pinfo(void) const
{
    std::cerr << "(" << point.x << ", " << point.y << ") - " << value << std::flush;
}

void Read(const DTDataStorage &input,const std::string &name,DTPoint2DValue &toReturn)
{
    DTDoubleArray theArr = input.ReadDoubleArray(name);
    if (theArr.Length()!=3) {
        DTErrorMessage("ReadFromArray(DTPoint2DValue)","Invalid length of array.");
        toReturn = DTPoint2DValue();
    }
    else {
        toReturn = DTPoint2DValue(DTPoint2D(theArr(0),theArr(1)),theArr(2));
    }
}

void Write(DTDataStorage &output,const std::string &name,const DTPoint2DValue &theVar)
{
    DTMutableDoubleArray theArr(3);
    theArr(0) = theVar.point.x;
    theArr(1) = theVar.point.y;
    theArr(2) = theVar.value;
    output.Save(theArr,name);
}

void WriteOne(DTDataStorage &output,const std::string &name,const DTPoint2DValue &toWrite)
{
    Write(output,name,toWrite);
    Write(output,"Seq_"+name,"2D Point Value");
    output.Flush();
}

