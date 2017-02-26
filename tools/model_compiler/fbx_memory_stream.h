#ifndef SLT_TOOLS_MODEL_COMPILER_FBX_MEMORY_STREAM_H
#define SLT_TOOLS_MODEL_COMPILER_FBX_MEMORY_STREAM_H

#include <fbxsdk.h>
#include "slt/mem/data_view.h"

// We use a custom in-memory stream instead of using FBX's default file stream
// so that the standard slt file logging kicks in.
class sltFbxMemoryStream : public FbxStream {
 public:
  inline sltFbxMemoryStream(FbxManager* manager, slt::DataView data)
      : manager_(manager), data_(data) {}

  // Inherited via FbxStream
  EState GetState() override {
    return state_;
  }

  bool Open(void* pStreamData) override {
    state_ = eOpen;
    return true;
  }

  bool Close() override {
    state_ = eClosed;
    return true;
  }

  bool Flush() override {
    return true;
  }

  int Write(const void*, int) override {
    SLT_UNREACHABLE();
    return 0;
  }

  int Read(void* dst, int amount) const override {
    SLT_ASSERT_GE(data_.size(), amount + position_);
    memcpy(dst, data_.data() + position_, amount);
    position_ += amount;
    return amount;
  }

  int GetReaderID() const override {
    auto registry = manager_->GetIOPluginRegistry();
    return registry->FindReaderIDByExtension("fbx");
  }

  int GetWriterID() const override {
    return -1;
  }

  void Seek(const FbxInt64& pOffset,
            const FbxFile::ESeekPos& pSeekPos) override {
    FbxInt64 ref = position_;
    switch(pSeekPos) {
      case FbxFile::eBegin:
        ref = 0;
        break;
      case FbxFile::eCurrent:
        ref = position_;
        break;
      case FbxFile::eEnd:
        ref = data_.size();
        break;
      default:
        break;
    }

    ref += pOffset;
    position_ = ref;
  }

  long GetPosition() const override {
    return long(position_);
  }

  void SetPosition(long pPosition) override {
    position_ = pPosition;
  }

  int GetError() const override {
    return 0;
  }

  void ClearError() override {}

 private:
  EState state_ = eEmpty;
  slt::DataView data_;
  FbxManager* manager_;
  mutable std::size_t position_ = 0;
};

#endif