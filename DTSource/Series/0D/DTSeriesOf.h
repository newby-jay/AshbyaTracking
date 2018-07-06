// Part of DTSource. Copyright 2004-2017. David Adalsteinsson.
// see http://www.visualdatatools.com/DTSource/license.html for more information.

#ifndef DTSeriesOf_Header
#define DTSeriesOf_Header

// This is for a series of series.
#include "DTSeries.h"

template <class T>
class DTSeriesOf : public DTSeries
{
public:
    DTSeriesOf() : DTSeries("Group") {}
    DTSeriesOf(DTDataStorage &saveInto,string name) : DTSeries(saveInto,name,"Series") {}
    
    void Add(T v,double time) {
        if (HowManySaved()==0) T::WriteStructure(DataFile(),"SeqInfo_"+Name());
        if (SharedSave(time)==false) return;
        Write(DataFile(),baseName(),v);
        DataFile().Flush();
    }
    
    T Get(double time) {
        int k = IndexForTime(time);
        if (k<0) return T();
        T toReturn;
        Read(DataFile(),baseName(k),toReturn);
        return toReturn;
    }
};

#endif
