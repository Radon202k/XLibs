class OutputMemoryStream
{ 
public: 
    OutputMemoryStream(): mBuffer(nullptr), mHead(0), mCapacity(0) 
    {ReallocBuffer(32);} 

    ~OutputMemoryStream() {std::free(mBuffer);} 

    //get a pointer to the data in the stream
    const char* GetBufferPtr() const {return mBuffer;} 

    uint32_t GetLength() const {return mHead;} 

    void Write(const void* inData, size_t inByteCount); 
    void Write(uint32_t inData) {Write(&inData, sizeof( inData));} 
    void Write(int32_t inData) {Write(&inData, sizeof( inData));} 

private: 
    void ReallocBuffer(uint32_t inNewLength); 
    char* mBuffer; 
    uint32_t mHead; 
    uint32_t mCapacity; 
}; 

void OutputMemoryStream::ReallocBuffer(uint32_t inNewLength) 
{ 
    mBuffer = static_cast<char*>(std::realloc( mBuffer, inNewLength)); 
    //handle realloc failure 
    //...
    mCapacity = inNewLength; 
} 

void OutputMemoryStream::Write(const void* inData, size_t inByteCount) 
{ 
    //make sure we have space...
    uint32_t resultHead = mHead + static_cast<uint32_t>(inByteCount); 
    if(resultHead > mCapacity) 
    { 
        ReallocBuffer(std::max( mCapacity * 2, resultHead)); 
    } 
    //copy into buffer at head
    std::memcpy(mBuffer + mHead, inData, inByteCount); 
    //increment head for next write
    mHead = resultHead; 
} 