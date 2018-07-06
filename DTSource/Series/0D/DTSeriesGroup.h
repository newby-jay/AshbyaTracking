// Part of DTSource. Copyright 2004-2017. David Adalsteinsson.
// see http://www.visualdatatools.com/DTSource/license.html for more information.

#ifndef DTSeriesGroup_Header
#define DTSeriesGroup_Header

// See DTSeries.h for further information.
#include "DTSeries.h"

template <class T>
class DTSeriesGroup : public DTSeries
{
public:
    DTSeriesGroup() : DTSeries("Group") {}
    DTSeriesGroup(DTDataStorage &saveInto,string name) : DTSeries(saveInto,name,"Group") {}
    
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
