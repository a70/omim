#include "cpu_buffer.hpp"

#include "../base/math.hpp"
#include "../base/shared_buffer_manager.hpp"
#include "../base/assert.hpp"

CPUBuffer::CPUBuffer(uint8_t elementSize, uint16_t capacity)
  : base_t(elementSize, capacity)
{
  uint32_t memorySize = my::NextPowOf2(GetCapacity() * GetElementSize());
  m_memory = SharedBufferManager::instance().reserveSharedBuffer(memorySize);
  m_memoryCursor = Data();
}

CPUBuffer::~CPUBuffer()
{
  m_memoryCursor = NULL;
  SharedBufferManager::instance().freeSharedBuffer(m_memory->size(), m_memory);
}

void CPUBuffer::UploadData(const void * data, uint16_t elementCount)
{
  uint32_t byteCountToCopy = GetElementSize() * elementCount;
#ifdef DEBUG
  // Memory check
  ASSERT(GetCursor() + byteCountToCopy <= Data() + m_memory->size(), ());
#endif

  memcpy(GetCursor(), data, byteCountToCopy);
  base_t::UploadData(elementCount);
}

void CPUBuffer::Seek(uint16_t elementNumber)
{
  uint32_t offsetFromBegin = GetElementSize() * elementNumber;
  ASSERT(Data() + offsetFromBegin <= Data() + m_memory->size(), ());
  base_t::Seek(elementNumber);
  m_memoryCursor = Data() + offsetFromBegin;
}

uint16_t CPUBuffer::GetCurrentElementNumber() const
{
  uint16_t pointerDiff = GetCursor() - Data();
  ASSERT(pointerDiff % GetElementSize() == 0, ());
  return pointerDiff / GetElementSize();
}

const unsigned char * CPUBuffer::Data() const
{
  return &((*m_memory)[0]);
}

unsigned char * CPUBuffer::GetCursor() const
{
  return m_memoryCursor;
}
