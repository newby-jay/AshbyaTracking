//
//  DTMesh3DOnDisk.cpp
//  io for disk mesh
//
//  Created by David Adalsteinsson on 3/23/14.
//
//

#include "DTMesh3DOnDisk.h"

#include "DTDoubleArray.h"
#include "DTFloatArray.h"
#include "DTCharArray.h"
#include "DTUCharArray.h"
#include "DTIntArray.h"
#include "DTShortIntArray.h"
#include "DTUShortIntArray.h"
#include "DTArrayConversion.h"
#include "DTUtilities.h"
#include "DTEndianSwap.h"
#include "DTError.h"

DTMesh3DOnDiskStorage::DTMesh3DOnDiskStorage()
{
    offsetOfStart = 0;
    endian = DTFile::LittleEndian;
    type = DTMesh3DOnDiskStorage::Double;
    
    offsetOfStart = 0;
    offsetToNextI = 0;
    offsetToNextJ = 0;
    offsetToNextK = 0;

    currentlyWriting = false;
    howManyKWritten = 0;
}

DTMesh3DOnDiskStorage::~DTMesh3DOnDiskStorage()
{
    if (currentlyWriting && howManyKWritten<grid.o()) {
        DTErrorMessage("~DTMesh3DOnDiskStorage","Did not finish writing the mesh.");
    }
}

int DTMesh3DOnDiskStorage::SizeOfEntry(void) const
{
    int toReturn = 0;
    switch (type) {
        case DTMesh3DOnDiskStorage::Double:
            toReturn = 8;
            break;
        case DTMesh3DOnDiskStorage::Float:
            toReturn = 4;
            break;
        case DTMesh3DOnDiskStorage::Int64:
        case DTMesh3DOnDiskStorage::UInt64:
            toReturn = 8;
            break;
        case DTMesh3DOnDiskStorage::Int32: 
        case DTMesh3DOnDiskStorage::UInt32:
            toReturn = 4;
            break;
        case DTMesh3DOnDiskStorage::Int16:
            toReturn = 2;
        case DTMesh3DOnDiskStorage::UInt16:
            toReturn = 2;
            break;
        case DTMesh3DOnDiskStorage::Int8:
        case DTMesh3DOnDiskStorage::UInt8:
            toReturn = 1;
            break;
    }

    return toReturn;
}

bool DTMesh3DOnDiskStorage::ReadRaw(DTRange range,char *buffer) const
{
    ssize_t startK = range.start;
    ssize_t howMany = range.length;
    int sizeOfEntry = SizeOfEntry();
    FILE *dataFile = file.FILEForReading();

    ssize_t m = grid.m();
    ssize_t n = grid.n();
    ssize_t mn = m*n;
    //int o = grid.o();
    int iC,jC,kC=0;
    off_t pos;
    int posRead = 0;
    
    if (offsetToNextI==sizeOfEntry) {
        // Can block the x reads.
        if (offsetToNextJ==sizeOfEntry*m) {
            // Can block the xy read
            if (offsetToNextK==sizeOfEntry*m*n) {
                // Single read statement
                pos = offsetOfStart + (kC+startK)*offsetToNextK;
                fseeko(dataFile,pos,SEEK_SET);
                fread(buffer,sizeOfEntry,m*n*howMany,dataFile);
            }
            else {
                // There are gaps betwen the k blocks
                for (kC=0;kC<howMany;kC++) {
                    pos = offsetOfStart + (kC+startK)*offsetToNextK;
                    fseeko(dataFile,pos,SEEK_SET);
                    fread(buffer+(kC*mn)*sizeOfEntry,sizeOfEntry,m*n,dataFile);
                }
            }
        }
        else {
            // There is a gap between the x blocks
            for (kC=0;kC<howMany;kC++) {
                for (jC=0;jC<n;jC++) {
                    pos = offsetOfStart + jC*offsetToNextJ + (kC+startK)*offsetToNextK;
                    fseeko(dataFile,pos,SEEK_SET);
                    fread(buffer+(jC*m+kC*mn)*sizeOfEntry,sizeOfEntry,m,dataFile);
                }
            }
        }
    }
    else {
        // Worst case scenario, ton's of individual read commands.
        for (kC=0;kC<howMany;kC++) {
            for (jC=0;jC<n;jC++) {
                pos = offsetOfStart + offsetOfStart + jC*offsetToNextJ + (kC+startK)*offsetToNextK;
                for (iC=0;iC<m;iC++) {
                    fseeko(dataFile,pos,SEEK_SET);
                    fread(buffer+posRead,sizeOfEntry,1,dataFile);
                    pos += offsetToNextI;
                    posRead+=sizeOfEntry;
                }
            }
        }
    }

    bool swapEndian = (DTFile::EndianForMachine()!=endian);
    if (swapEndian) {
        switch (sizeOfEntry) {
            case 2:
                DTSwap2Bytes((unsigned char *)buffer,m*n*range.length*sizeOfEntry);
                break;
            case 4:
                DTSwap4Bytes((unsigned char *)buffer,m*n*range.length*sizeOfEntry);
                break;
            case 8:
                DTSwap8Bytes((unsigned char *)buffer,m*n*range.length*sizeOfEntry);
                break;
        }
    }

    return true;
}

bool DTMesh3DOnDiskStorage::ReadZRange(DTRange range,DTMutableShortIntArray &arr) const
{
    ssize_t startK = range.start;
    ssize_t howMany = range.length;
    if (type!=DTMesh3DOnDiskStorage::Int16) {
        DTErrorMessage("ReadZRange(ShortIntArray)","Incompatible type");
        return false;
    }
    ssize_t m = grid.m();
    ssize_t n = grid.n();
    if (arr.m()!=m || arr.n()!=n) {
        DTErrorMessage("ReadZRange(ShortIntArray)","Incompatible size");
        return false;
    }
    if (arr.o()<howMany) {
        DTErrorMessage("ReadZRange(ShortIntArray)","Array not large enough");
        return false;
    }
    if (startK+howMany>grid.o()) {
        DTErrorMessage("ReadZRange(ShortIntArray)","Reading out of bounds");
        return false;
    }
    
    if (ReadRaw(range,(char *)arr.Pointer())==false)
        return false;
    
    // Might need to swap the endian information.
    
    return true;
}

bool DTMesh3DOnDiskStorage::ReadZRange(DTRange range,DTMutableFloatArray &arr) const
{
    ssize_t startK = range.start;
    ssize_t howMany = range.length;
    if (type!=DTMesh3DOnDiskStorage::Float) {
        DTErrorMessage("ReadZRange(DTMutableFloatArray)","Incompatible type");
        return false;
    }
    ssize_t m = grid.m();
    ssize_t n = grid.n();
    if (arr.m()!=m || arr.n()!=n) {
        DTErrorMessage("ReadZRange(DTMutableFloatArray)","Incompatible size");
        return false;
    }
    if (arr.o()<howMany) {
        DTErrorMessage("ReadZRange(DTMutableFloatArray)","Array not large enough");
        return false;
    }
    if (startK+howMany>grid.o()) {
        DTErrorMessage("ReadZRange(DTMutableFloatArray)","Reading out of bounds");
        return false;
    }
    
    if (ReadRaw(range,(char *)arr.Pointer())==false)
        return false;
    
    // Might need to swap the endian information.
    
    return true;
}

bool DTMesh3DOnDiskStorage::ReadZRange(DTRange range,DTMutableCharArray &work,DTMutableDoubleArray &arr) const
{
    ssize_t startK = range.start;
    ssize_t howMany = range.length;
    ssize_t m = grid.m();
    ssize_t n = grid.n();
    if (arr.m()!=m || arr.n()!=n) {
        DTErrorMessage("ReadZRange(work,DoubleArray)","Incompatible size");
        return false;
    }
    if (arr.o()<howMany) {
        DTErrorMessage("ReadZRange(work,DoubleArray)","Array not large enough");
        return false;
    }
    if (startK+howMany>grid.o()) {
        DTErrorMessage("ReadZRange(work,DoubleArray)","Reading out of bounds");
        return false;
    }
    if (SizeOfEntry()==8) {
        // Both are double
        if (ReadRaw(range,(char *)arr.Pointer())==false)
            return false;
        switch (type) {
            case DTMesh3DOnDiskStorage::Int64:
                DTConversionBetweenRowPointers((const int64_t *)arr.Pointer(),arr.Pointer(),arr.Length());
                break;
            case DTMesh3DOnDiskStorage::UInt64:
                DTConversionBetweenRowPointers((const uint64_t *)arr.Pointer(),arr.Pointer(),arr.Length());
                break;
            default:
                break;
        }
    }
    else if (SizeOfEntry()*howMany*m*n>work.Length()) {
        DTErrorMessage("ReadZRange(work,DoubleArray)","Work array not large enough.");
        return false;
    }
    else {
        if (ReadRaw(range,(char *)work.Pointer())==false)
            return false;
        // Convert from the work array
        switch (type) {
            case DTMesh3DOnDiskStorage::Float:
                DTConversionBetweenRowPointers((const float *)work.Pointer(),arr.Pointer(),howMany*m*n);
                break;
            case DTMesh3DOnDiskStorage::Int32:
                DTConversionBetweenRowPointers((const int32_t *)work.Pointer(),arr.Pointer(),howMany*m*n);
                break;
            case DTMesh3DOnDiskStorage::UInt32:
                DTConversionBetweenRowPointers((const uint32_t *)work.Pointer(),arr.Pointer(),howMany*m*n);
                break;
            case DTMesh3DOnDiskStorage::Int16:
                DTConversionBetweenRowPointers((const int16_t *)work.Pointer(),arr.Pointer(),howMany*m*n);
                break;
            case DTMesh3DOnDiskStorage::UInt16:
                DTConversionBetweenRowPointers((const uint16_t *)work.Pointer(),arr.Pointer(),howMany*m*n);
                break;
            case DTMesh3DOnDiskStorage::Int8:
                DTConversionBetweenRowPointers((const int8_t *)work.Pointer(),arr.Pointer(),howMany*m*n);
                break;
            case DTMesh3DOnDiskStorage::UInt8:
                DTConversionBetweenRowPointers((const uint8_t *)work.Pointer(),arr.Pointer(),howMany*m*n);
                break;
            default:
                break;
        }
    }


    return true;
}

bool DTMesh3DOnDiskStorage::ReadZRange(DTRange range,DTMutableCharArray &work,DTMutableFloatArray &arr) const
{
    ssize_t startK = range.start;
    ssize_t howMany = range.length;
    ssize_t m = grid.m();
    ssize_t n = grid.n();

    if (arr.m()!=m || arr.n()!=n) {
        DTErrorMessage("ReadZRange(work,FloatArray)","Incompatible size");
        return false;
    }
    if (arr.o()<howMany) {
        DTErrorMessage("ReadZRange(work,FloatArray)","Array not large enough");
        return false;
    }
    if (startK+howMany>grid.o()) {
        DTErrorMessage("ReadZRange(work,FloatArray)","Reading out of bounds");
        return false;
    }
    if (SizeOfEntry()==4) {
        // Both are float
        if (ReadRaw(range,(char *)arr.Pointer())==false)
            return false;
        switch (type) {
            case DTMesh3DOnDiskStorage::Int32:
                DTConversionBetweenRowPointers((const int32_t *)arr.Pointer(),arr.Pointer(),arr.Length());
                break;
            case DTMesh3DOnDiskStorage::UInt32:
                DTConversionBetweenRowPointers((const uint32_t *)arr.Pointer(),arr.Pointer(),arr.Length());
                break;
            default:
                break;
        }
    }
    else if (SizeOfEntry()*howMany*m*n>work.Length()) {
        DTErrorMessage("ReadZRange(work,FloatArray)","Work array not large enough.");
        return false;
    }
    else {
        if (ReadRaw(range,(char *)work.Pointer())==false)
            return false;
        // Convert from the work array
        switch (type) {
            case DTMesh3DOnDiskStorage::Double:
                DTConversionBetweenRowPointers((const double *)work.Pointer(),arr.Pointer(),howMany*m*n);
                break;
            case DTMesh3DOnDiskStorage::Int64:
                DTConversionBetweenRowPointers((const int64_t *)work.Pointer(),arr.Pointer(),howMany*m*n);
                break;
            case DTMesh3DOnDiskStorage::UInt64:
                DTConversionBetweenRowPointers((const uint64_t *)work.Pointer(),arr.Pointer(),howMany*m*n);
                break;
            case DTMesh3DOnDiskStorage::Int16:
                DTConversionBetweenRowPointers((const int16_t *)work.Pointer(),arr.Pointer(),howMany*m*n);
                break;
            case DTMesh3DOnDiskStorage::UInt16:
                DTConversionBetweenRowPointers((const uint16_t *)work.Pointer(),arr.Pointer(),howMany*m*n);
                break;
            case DTMesh3DOnDiskStorage::Int8:
                DTConversionBetweenRowPointers((const int8_t *)work.Pointer(),arr.Pointer(),howMany*m*n);
                break;
            case DTMesh3DOnDiskStorage::UInt8:
                DTConversionBetweenRowPointers((const uint8_t *)work.Pointer(),arr.Pointer(),howMany*m*n);
                break;
            default:
                break;
        }
    }


    return true;
}

void DTMesh3DOnDiskStorage::RawAppend(char *ptr,ssize_t howMany,ssize_t slices)
{
    FILE *dataFile = file.FILEForWriting();
    fseeko(dataFile,0,SEEK_END);
    fwrite(ptr, howMany, 1,dataFile);
    howManyKWritten += slices;
}

#pragma mark DTMesh3DOnDisk

DTMesh3DOnDisk::DTMesh3DOnDisk(DTFile theFile,DTMesh3DGrid theGrid ,DTFile::Endian eType,DTMesh3DOnDiskStorage::NumberType nType,
                               off_t off,off_t offI,off_t offJ,off_t offK)
{
    DTMesh3DOnDiskStorage theStorage;

    theStorage.file = theFile;
    theStorage.offsetOfStart = off;
    theStorage.offsetToNextI = offI;
    theStorage.offsetToNextJ = offJ;
    theStorage.offsetToNextK = offK;
    theStorage.grid = theGrid;
    theStorage.endian = eType;
    theStorage.type = nType;
    theStorage.currentlyWriting = false;
    theStorage.howManyKWritten = 0;

    storage = DTPointer<DTMesh3DOnDiskStorage>(new DTMesh3DOnDiskStorage(theStorage));
}

void DTMesh3DOnDisk::pinfo(void) const
{
    if (m()==0) {
        std::cerr << "Empty.\n";
        return;
    }
    
    std::cerr << "Size = " << m() << " x " << n() << " x " << o() << ", ";
    DTMesh3DGrid grid = Grid();
    DTPoint3D Or = grid.Origin();
    std::cerr << "Origin = (" << Or.x << "," << Or.y << "," << Or.z << "), ";
    if (grid.dx()==grid.dy() && grid.dy()==grid.dz())
        std::cerr << "h = " << grid.dx();
    else {
        std::cerr << "dx = " << grid.dx();
        std::cerr << ", dy = " << grid.dy();
        std::cerr << ", dz = " << grid.dz();
    }
    
    std::cerr << ". File = " << storage->file.Name();
    if (storage->currentlyWriting) {
        if (storage->howManyKWritten<o()) {
            std::cerr << ". " << storage->howManyKWritten << "/" << o() << " written";
        }
        else {
            std::cerr << ". fully written";
        }
    }

    switch (storage->type) {
        case DTMesh3DOnDiskStorage::Double:
            std::cerr << ". Double";
            break;
        case DTMesh3DOnDiskStorage::Float:
            std::cerr << ". Float";
            break;
        case DTMesh3DOnDiskStorage::Int64:
            std::cerr << ". 64bit Int";
            break;
        case DTMesh3DOnDiskStorage::UInt64:
            std::cerr << ". 64bit unsigned int";
            break;
        case DTMesh3DOnDiskStorage::Int32:
            std::cerr << ". 32 bit int";
            break;
        case DTMesh3DOnDiskStorage::UInt32:
            std::cerr << ". 32 bit unsigned int";
            break;
        case DTMesh3DOnDiskStorage::Int16:
            std::cerr << ". 16 bit int";
            break;
        case DTMesh3DOnDiskStorage::UInt16:
            std::cerr << ". 16 bit unsigned int";
            break;
        case DTMesh3DOnDiskStorage::Int8:
            std::cerr << ". 8 bit int";
            break;
        case DTMesh3DOnDiskStorage::UInt8:
            std::cerr << ". 8 bit unsigned int";
            break;
    }
 
    std::cerr << std::endl << std::flush;
}

#pragma mark -
#pragma mark Mutable version

DTMutableMesh3DOnDisk::DTMutableMesh3DOnDisk(std::string nm,DTMesh3DGrid grid,DTMesh3DOnDiskStorage::NumberType tp)
{
    DTMesh3DOnDiskStorage theStorage;

    theStorage.file = DTFile(nm,DTFile::NewReadWrite);
    theStorage.type = tp;
    theStorage.offsetOfStart = 0;
    off_t sizeOfEntry = theStorage.SizeOfEntry();
    theStorage.offsetToNextI = sizeOfEntry;
    theStorage.offsetToNextJ = sizeOfEntry*grid.m();
    theStorage.offsetToNextK =sizeOfEntry*grid.m()*grid.n();
    theStorage.grid = grid;
    // The endian should be based on what the type is right now
    theStorage.endian = DTFile::EndianForMachine();
    theStorage.howManyKWritten = 0;

    mutableStorage = DTMutablePointer<DTMesh3DOnDiskStorage>(new DTMesh3DOnDiskStorage(theStorage));
    storage = mutableStorage;
}

DTMutableMesh3DOnDisk::DTMutableMesh3DOnDisk(DTFile theFile,DTMesh3DGrid grid,DTMesh3DOnDiskStorage::NumberType tp)
{
    DTMesh3DOnDiskStorage theStorage;
    
    theStorage.file = theFile;
    theStorage.type = tp;
    theStorage.offsetOfStart = 0;
    off_t sizeOfEntry = theStorage.SizeOfEntry();
    theStorage.offsetToNextI = sizeOfEntry;
    theStorage.offsetToNextJ = sizeOfEntry*grid.m();
    theStorage.offsetToNextK =sizeOfEntry*grid.m()*grid.n();
    theStorage.grid = grid;
    // The endian should be based on what the type is right now
    theStorage.endian = DTFile::EndianForMachine();
    theStorage.howManyKWritten = 0;
    
    mutableStorage = DTMutablePointer<DTMesh3DOnDiskStorage>(new DTMesh3DOnDiskStorage(theStorage));
    storage = mutableStorage;
}

void DTMutableMesh3DOnDisk::Append(DTRange range,const DTIntArray &arr)
{
    // Need to have the same type
    if (storage->type!=DTMesh3DOnDiskStorage::Int32) {
        DTErrorMessage("DTMesh3DOnDisk::Append(DTIntArray)","Type mismatch");
        return;
    }
    if (m()!=arr.m() || n()!=arr.n()) {
        DTErrorMessage("DTMesh3DOnDisk::Append(DTIntArray)","Size mismatch");
        return;
    }
    ssize_t howMany = range.length;
    if (storage->howManyKWritten+range.length>o()) {
        howMany = o() - storage->howManyKWritten;
        DTErrorMessage("DTMesh3DOnDisk::Append(DTIntArray)","Out of bounds");
    }
    if (howMany<=0) return;

    storage->RawAppend((char *)(arr.Pointer()+m()*n()*range.start),m()*n()*howMany*4,howMany);
}

void DTMutableMesh3DOnDisk::Append(DTRange range,const DTFloatArray &arr)
{
    // Need to have the same type
    if (storage->type!=DTMesh3DOnDiskStorage::Float) {
        DTErrorMessage("DTMesh3DOnDisk::Append(DTFloatArray)","Type mismatch");
        return;
    }
    if (m()!=arr.m() || n()!=arr.n()) {
        DTErrorMessage("DTMesh3DOnDisk::Append(DTFloatArray)","Size mismatch");
        return;
    }
    ssize_t howMany = range.length;
    if (storage->howManyKWritten+range.length>o()) {
        howMany = o() - storage->howManyKWritten;
        DTErrorMessage("DTMesh3DOnDisk::Append(DTFloatArray)","Out of bounds");
    }
    if (howMany<=0) return;

    storage->RawAppend((char *)(arr.Pointer()+m()*n()*range.start),m()*n()*howMany*4,howMany);
}

void DTMutableMesh3DOnDisk::Append(DTRange range,const DTDoubleArray &arr)
{
    // Need to have the same type
    if (storage->type!=DTMesh3DOnDiskStorage::Double) {
        DTErrorMessage("DTMesh3DOnDisk::Append(DTDoubleArray)","Type mismatch");
        return;
    }
    if (m()!=arr.m() || n()!=arr.n()) {
        DTErrorMessage("DTMesh3DOnDisk::Append(DTDoubleArray)","Size mismatch");
        return;
    }
    ssize_t howMany = range.length;
    if (storage->howManyKWritten+range.length>o()) {
        howMany = o() - storage->howManyKWritten;
        DTErrorMessage("DTMesh3DOnDisk::Append(DTDoubleArray)","Out of bounds");
    }
    if (howMany<=0) return;
    
    storage->RawAppend((char *)(arr.Pointer()+m()*n()*range.start),m()*n()*howMany*8,howMany);
}

void Read(const DTDataStorage &input,const std::string &name,DTMesh3DOnDisk &toReturn)
{
    DTIntArray size = input.ReadIntArray(name+"_d");
    if (size.Length()!=4) {
        DTErrorMessage("Read(data,name,DTMesh3DOnDisk","size array is invalid");
        toReturn = DTMesh3DOnDisk();
        return;
    }
    off_t off = size(0);
    off_t strideI = size(1);
    off_t strideJ = size(2);
    off_t strideK = size(3);
    
    DTIntArray et = input.ReadIntArray(name+"_et");
    if (et.Length()!=2) {
        DTErrorMessage("Read(data,name,DTMesh3DOnDisk","endian and type not valid");
        toReturn = DTMesh3DOnDisk();
        return;
    }
    
    DTFile::Endian endian = DTFile::LittleEndian;
    switch (et(0)) {
        case 1:
            endian = DTFile::LittleEndian;
            break;
        case 2:
            endian = DTFile::BigEndian;
            break;
    }
        
    DTMesh3DOnDiskStorage::NumberType type = DTMesh3DOnDiskStorage::Double;
    switch (et(1)) {
        case 10:
            type = DTMesh3DOnDiskStorage::Int8;
            break;
        case 9:
            type = DTMesh3DOnDiskStorage::UInt8;
            break;
        case 8:
            type = DTMesh3DOnDiskStorage::Int16;
            break;
        case 7:
            type = DTMesh3DOnDiskStorage::UInt16;
            break;
        case 6:
            type = DTMesh3DOnDiskStorage::Int32;
            break;
        case 5:
            type = DTMesh3DOnDiskStorage::UInt32;
            break;
        case 4:
            type = DTMesh3DOnDiskStorage::Int64;
            break;
        case 3:
            type = DTMesh3DOnDiskStorage::UInt64;
            break;
        case 2:
            type = DTMesh3DOnDiskStorage::Float;
            break;
        case 1:
            type = DTMesh3DOnDiskStorage::Double;
            break;
    }
    
    DTMesh3DGrid theGrid;
    Read(input,name+"_grid", theGrid);
    
    DTFile theFile(input.ReadString(name),DTFile::ExistingReadWrite);
    
    toReturn = DTMesh3DOnDisk(theFile, theGrid, endian, type,off,strideI,strideJ,strideK);
}

void DTMesh3DOnDisk::WriteIntoFile(DTDataStorage &output,const std::string &name) const
{
    DTMutableCharArray et(2);
    et(0) = (Endian()==DTFile::LittleEndian ? 1 : 2);
    switch (Type()) {
        case DTMesh3DOnDiskStorage::Int8:
            et(1) = 10;
            break;
        case DTMesh3DOnDiskStorage::UInt8:
            et(1) = 9;
            break;
        case DTMesh3DOnDiskStorage::Int16:
            et(1) = 8;
            break;
        case DTMesh3DOnDiskStorage::UInt16:
            et(1) = 7;
            break;
        case DTMesh3DOnDiskStorage::Int32:
            et(1) = 6;
            break;
        case DTMesh3DOnDiskStorage::UInt32:
            et(1) = 5;
            break;
        case DTMesh3DOnDiskStorage::Int64:
            et(1) = 4;
            break;
        case DTMesh3DOnDiskStorage::UInt64:
            et(1) = 3;
            break;
        case DTMesh3DOnDiskStorage::Float:
            et(1) = 2;
            break;
        case DTMesh3DOnDiskStorage::Double:
            et(1) = 1;
            break;
            
    }
    Write(output,name+"_et",et);
    
    DTMutableIntArray t(4);
    t(0) = (int)storage->offsetOfStart;
    t(1) = (int)storage->offsetToNextI;
    t(2) = (int)storage->offsetToNextJ;
    t(3) = (int)storage->offsetToNextK;
    Write(output,name+"_d",t);
    
    Write(output,name+"_grid",Grid());
    
    Write(output,name,storage->file.Name());
}

void Write(DTDataStorage &output,const std::string &name,const DTMesh3DOnDisk &theVar)
{
    theVar.WriteIntoFile(output,name);
}

void WriteOne(DTDataStorage &output,const std::string &name,const DTMesh3DOnDisk &toWrite)
{
    Write(output,name,toWrite);
    Write(output,"Seq_"+name,"MeshDisk3D");
    output.Flush();
}

