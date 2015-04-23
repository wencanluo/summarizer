// Generated by the protocol buffer compiler.  DO NOT EDIT!
// source: summarizer/distribution.proto

#define INTERNAL_SUPPRESS_PROTOBUF_FIELD_DEPRECATION
#include "summarizer/distribution.pb.h"

#include <algorithm>

#include <google/protobuf/stubs/common.h>
#include <google/protobuf/stubs/once.h>
#include <google/protobuf/io/coded_stream.h>
#include <google/protobuf/wire_format_lite_inl.h>
#include <google/protobuf/descriptor.h>
#include <google/protobuf/generated_message_reflection.h>
#include <google/protobuf/reflection_ops.h>
#include <google/protobuf/wire_format.h>
// @@protoc_insertion_point(includes)

namespace topicsum {

namespace {

const ::google::protobuf::Descriptor* DistributionEntry_descriptor_ = NULL;
const ::google::protobuf::internal::GeneratedMessageReflection*
  DistributionEntry_reflection_ = NULL;
const ::google::protobuf::Descriptor* DistributionProto_descriptor_ = NULL;
const ::google::protobuf::internal::GeneratedMessageReflection*
  DistributionProto_reflection_ = NULL;

}  // namespace


void protobuf_AssignDesc_summarizer_2fdistribution_2eproto() {
  protobuf_AddDesc_summarizer_2fdistribution_2eproto();
  const ::google::protobuf::FileDescriptor* file =
    ::google::protobuf::DescriptorPool::generated_pool()->FindFileByName(
      "summarizer/distribution.proto");
  GOOGLE_CHECK(file != NULL);
  DistributionEntry_descriptor_ = file->message_type(0);
  static const int DistributionEntry_offsets_[2] = {
    GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(DistributionEntry, word_),
    GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(DistributionEntry, value_),
  };
  DistributionEntry_reflection_ =
    new ::google::protobuf::internal::GeneratedMessageReflection(
      DistributionEntry_descriptor_,
      DistributionEntry::default_instance_,
      DistributionEntry_offsets_,
      GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(DistributionEntry, _has_bits_[0]),
      GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(DistributionEntry, _unknown_fields_),
      -1,
      ::google::protobuf::DescriptorPool::generated_pool(),
      ::google::protobuf::MessageFactory::generated_factory(),
      sizeof(DistributionEntry));
  DistributionProto_descriptor_ = file->message_type(1);
  static const int DistributionProto_offsets_[1] = {
    GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(DistributionProto, entry_),
  };
  DistributionProto_reflection_ =
    new ::google::protobuf::internal::GeneratedMessageReflection(
      DistributionProto_descriptor_,
      DistributionProto::default_instance_,
      DistributionProto_offsets_,
      GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(DistributionProto, _has_bits_[0]),
      GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(DistributionProto, _unknown_fields_),
      -1,
      ::google::protobuf::DescriptorPool::generated_pool(),
      ::google::protobuf::MessageFactory::generated_factory(),
      sizeof(DistributionProto));
}

namespace {

GOOGLE_PROTOBUF_DECLARE_ONCE(protobuf_AssignDescriptors_once_);
inline void protobuf_AssignDescriptorsOnce() {
  ::google::protobuf::GoogleOnceInit(&protobuf_AssignDescriptors_once_,
                 &protobuf_AssignDesc_summarizer_2fdistribution_2eproto);
}

void protobuf_RegisterTypes(const ::std::string&) {
  protobuf_AssignDescriptorsOnce();
  ::google::protobuf::MessageFactory::InternalRegisterGeneratedMessage(
    DistributionEntry_descriptor_, &DistributionEntry::default_instance());
  ::google::protobuf::MessageFactory::InternalRegisterGeneratedMessage(
    DistributionProto_descriptor_, &DistributionProto::default_instance());
}

}  // namespace

void protobuf_ShutdownFile_summarizer_2fdistribution_2eproto() {
  delete DistributionEntry::default_instance_;
  delete DistributionEntry_reflection_;
  delete DistributionProto::default_instance_;
  delete DistributionProto_reflection_;
}

void protobuf_AddDesc_summarizer_2fdistribution_2eproto() {
  static bool already_here = false;
  if (already_here) return;
  already_here = true;
  GOOGLE_PROTOBUF_VERIFY_VERSION;

  ::google::protobuf::DescriptorPool::InternalAddGeneratedFile(
    "\n\035summarizer/distribution.proto\022\010topicsu"
    "m\"0\n\021DistributionEntry\022\014\n\004word\030\001 \002(\t\022\r\n\005"
    "value\030\002 \001(\001\"\?\n\021DistributionProto\022*\n\005entr"
    "y\030\001 \003(\0132\033.topicsum.DistributionEntry", 156);
  ::google::protobuf::MessageFactory::InternalRegisterGeneratedFile(
    "summarizer/distribution.proto", &protobuf_RegisterTypes);
  DistributionEntry::default_instance_ = new DistributionEntry();
  DistributionProto::default_instance_ = new DistributionProto();
  DistributionEntry::default_instance_->InitAsDefaultInstance();
  DistributionProto::default_instance_->InitAsDefaultInstance();
  ::google::protobuf::internal::OnShutdown(&protobuf_ShutdownFile_summarizer_2fdistribution_2eproto);
}

// Force AddDescriptors() to be called at static initialization time.
struct StaticDescriptorInitializer_summarizer_2fdistribution_2eproto {
  StaticDescriptorInitializer_summarizer_2fdistribution_2eproto() {
    protobuf_AddDesc_summarizer_2fdistribution_2eproto();
  }
} static_descriptor_initializer_summarizer_2fdistribution_2eproto_;

// ===================================================================

#ifndef _MSC_VER
const int DistributionEntry::kWordFieldNumber;
const int DistributionEntry::kValueFieldNumber;
#endif  // !_MSC_VER

DistributionEntry::DistributionEntry()
  : ::google::protobuf::Message() {
  SharedCtor();
}

void DistributionEntry::InitAsDefaultInstance() {
}

DistributionEntry::DistributionEntry(const DistributionEntry& from)
  : ::google::protobuf::Message() {
  SharedCtor();
  MergeFrom(from);
}

void DistributionEntry::SharedCtor() {
  _cached_size_ = 0;
  word_ = const_cast< ::std::string*>(&::google::protobuf::internal::kEmptyString);
  value_ = 0;
  ::memset(_has_bits_, 0, sizeof(_has_bits_));
}

DistributionEntry::~DistributionEntry() {
  SharedDtor();
}

void DistributionEntry::SharedDtor() {
  if (word_ != &::google::protobuf::internal::kEmptyString) {
    delete word_;
  }
  if (this != default_instance_) {
  }
}

void DistributionEntry::SetCachedSize(int size) const {
  GOOGLE_SAFE_CONCURRENT_WRITES_BEGIN();
  _cached_size_ = size;
  GOOGLE_SAFE_CONCURRENT_WRITES_END();
}
const ::google::protobuf::Descriptor* DistributionEntry::descriptor() {
  protobuf_AssignDescriptorsOnce();
  return DistributionEntry_descriptor_;
}

const DistributionEntry& DistributionEntry::default_instance() {
  if (default_instance_ == NULL) protobuf_AddDesc_summarizer_2fdistribution_2eproto();
  return *default_instance_;
}

DistributionEntry* DistributionEntry::default_instance_ = NULL;

DistributionEntry* DistributionEntry::New() const {
  return new DistributionEntry;
}

void DistributionEntry::Clear() {
  if (_has_bits_[0 / 32] & (0xffu << (0 % 32))) {
    if (has_word()) {
      if (word_ != &::google::protobuf::internal::kEmptyString) {
        word_->clear();
      }
    }
    value_ = 0;
  }
  ::memset(_has_bits_, 0, sizeof(_has_bits_));
  mutable_unknown_fields()->Clear();
}

bool DistributionEntry::MergePartialFromCodedStream(
    ::google::protobuf::io::CodedInputStream* input) {
#define DO_(EXPRESSION) if (!(EXPRESSION)) return false
  ::google::protobuf::uint32 tag;
  while ((tag = input->ReadTag()) != 0) {
    switch (::google::protobuf::internal::WireFormatLite::GetTagFieldNumber(tag)) {
      // required string word = 1;
      case 1: {
        if (::google::protobuf::internal::WireFormatLite::GetTagWireType(tag) ==
            ::google::protobuf::internal::WireFormatLite::WIRETYPE_LENGTH_DELIMITED) {
          DO_(::google::protobuf::internal::WireFormatLite::ReadString(
                input, this->mutable_word()));
          ::google::protobuf::internal::WireFormat::VerifyUTF8String(
            this->word().data(), this->word().length(),
            ::google::protobuf::internal::WireFormat::PARSE);
        } else {
          goto handle_uninterpreted;
        }
        if (input->ExpectTag(17)) goto parse_value;
        break;
      }

      // optional double value = 2;
      case 2: {
        if (::google::protobuf::internal::WireFormatLite::GetTagWireType(tag) ==
            ::google::protobuf::internal::WireFormatLite::WIRETYPE_FIXED64) {
         parse_value:
          DO_((::google::protobuf::internal::WireFormatLite::ReadPrimitive<
                   double, ::google::protobuf::internal::WireFormatLite::TYPE_DOUBLE>(
                 input, &value_)));
          set_has_value();
        } else {
          goto handle_uninterpreted;
        }
        if (input->ExpectAtEnd()) return true;
        break;
      }

      default: {
      handle_uninterpreted:
        if (::google::protobuf::internal::WireFormatLite::GetTagWireType(tag) ==
            ::google::protobuf::internal::WireFormatLite::WIRETYPE_END_GROUP) {
          return true;
        }
        DO_(::google::protobuf::internal::WireFormat::SkipField(
              input, tag, mutable_unknown_fields()));
        break;
      }
    }
  }
  return true;
#undef DO_
}

void DistributionEntry::SerializeWithCachedSizes(
    ::google::protobuf::io::CodedOutputStream* output) const {
  // required string word = 1;
  if (has_word()) {
    ::google::protobuf::internal::WireFormat::VerifyUTF8String(
      this->word().data(), this->word().length(),
      ::google::protobuf::internal::WireFormat::SERIALIZE);
    ::google::protobuf::internal::WireFormatLite::WriteString(
      1, this->word(), output);
  }

  // optional double value = 2;
  if (has_value()) {
    ::google::protobuf::internal::WireFormatLite::WriteDouble(2, this->value(), output);
  }

  if (!unknown_fields().empty()) {
    ::google::protobuf::internal::WireFormat::SerializeUnknownFields(
        unknown_fields(), output);
  }
}

::google::protobuf::uint8* DistributionEntry::SerializeWithCachedSizesToArray(
    ::google::protobuf::uint8* target) const {
  // required string word = 1;
  if (has_word()) {
    ::google::protobuf::internal::WireFormat::VerifyUTF8String(
      this->word().data(), this->word().length(),
      ::google::protobuf::internal::WireFormat::SERIALIZE);
    target =
      ::google::protobuf::internal::WireFormatLite::WriteStringToArray(
        1, this->word(), target);
  }

  // optional double value = 2;
  if (has_value()) {
    target = ::google::protobuf::internal::WireFormatLite::WriteDoubleToArray(2, this->value(), target);
  }

  if (!unknown_fields().empty()) {
    target = ::google::protobuf::internal::WireFormat::SerializeUnknownFieldsToArray(
        unknown_fields(), target);
  }
  return target;
}

int DistributionEntry::ByteSize() const {
  int total_size = 0;

  if (_has_bits_[0 / 32] & (0xffu << (0 % 32))) {
    // required string word = 1;
    if (has_word()) {
      total_size += 1 +
        ::google::protobuf::internal::WireFormatLite::StringSize(
          this->word());
    }

    // optional double value = 2;
    if (has_value()) {
      total_size += 1 + 8;
    }

  }
  if (!unknown_fields().empty()) {
    total_size +=
      ::google::protobuf::internal::WireFormat::ComputeUnknownFieldsSize(
        unknown_fields());
  }
  GOOGLE_SAFE_CONCURRENT_WRITES_BEGIN();
  _cached_size_ = total_size;
  GOOGLE_SAFE_CONCURRENT_WRITES_END();
  return total_size;
}

void DistributionEntry::MergeFrom(const ::google::protobuf::Message& from) {
  GOOGLE_CHECK_NE(&from, this);
  const DistributionEntry* source =
    ::google::protobuf::internal::dynamic_cast_if_available<const DistributionEntry*>(
      &from);
  if (source == NULL) {
    ::google::protobuf::internal::ReflectionOps::Merge(from, this);
  } else {
    MergeFrom(*source);
  }
}

void DistributionEntry::MergeFrom(const DistributionEntry& from) {
  GOOGLE_CHECK_NE(&from, this);
  if (from._has_bits_[0 / 32] & (0xffu << (0 % 32))) {
    if (from.has_word()) {
      set_word(from.word());
    }
    if (from.has_value()) {
      set_value(from.value());
    }
  }
  mutable_unknown_fields()->MergeFrom(from.unknown_fields());
}

void DistributionEntry::CopyFrom(const ::google::protobuf::Message& from) {
  if (&from == this) return;
  Clear();
  MergeFrom(from);
}

void DistributionEntry::CopyFrom(const DistributionEntry& from) {
  if (&from == this) return;
  Clear();
  MergeFrom(from);
}

bool DistributionEntry::IsInitialized() const {
  if ((_has_bits_[0] & 0x00000001) != 0x00000001) return false;

  return true;
}

void DistributionEntry::Swap(DistributionEntry* other) {
  if (other != this) {
    std::swap(word_, other->word_);
    std::swap(value_, other->value_);
    std::swap(_has_bits_[0], other->_has_bits_[0]);
    _unknown_fields_.Swap(&other->_unknown_fields_);
    std::swap(_cached_size_, other->_cached_size_);
  }
}

::google::protobuf::Metadata DistributionEntry::GetMetadata() const {
  protobuf_AssignDescriptorsOnce();
  ::google::protobuf::Metadata metadata;
  metadata.descriptor = DistributionEntry_descriptor_;
  metadata.reflection = DistributionEntry_reflection_;
  return metadata;
}


// ===================================================================

#ifndef _MSC_VER
const int DistributionProto::kEntryFieldNumber;
#endif  // !_MSC_VER

DistributionProto::DistributionProto()
  : ::google::protobuf::Message() {
  SharedCtor();
}

void DistributionProto::InitAsDefaultInstance() {
}

DistributionProto::DistributionProto(const DistributionProto& from)
  : ::google::protobuf::Message() {
  SharedCtor();
  MergeFrom(from);
}

void DistributionProto::SharedCtor() {
  _cached_size_ = 0;
  ::memset(_has_bits_, 0, sizeof(_has_bits_));
}

DistributionProto::~DistributionProto() {
  SharedDtor();
}

void DistributionProto::SharedDtor() {
  if (this != default_instance_) {
  }
}

void DistributionProto::SetCachedSize(int size) const {
  GOOGLE_SAFE_CONCURRENT_WRITES_BEGIN();
  _cached_size_ = size;
  GOOGLE_SAFE_CONCURRENT_WRITES_END();
}
const ::google::protobuf::Descriptor* DistributionProto::descriptor() {
  protobuf_AssignDescriptorsOnce();
  return DistributionProto_descriptor_;
}

const DistributionProto& DistributionProto::default_instance() {
  if (default_instance_ == NULL) protobuf_AddDesc_summarizer_2fdistribution_2eproto();
  return *default_instance_;
}

DistributionProto* DistributionProto::default_instance_ = NULL;

DistributionProto* DistributionProto::New() const {
  return new DistributionProto;
}

void DistributionProto::Clear() {
  entry_.Clear();
  ::memset(_has_bits_, 0, sizeof(_has_bits_));
  mutable_unknown_fields()->Clear();
}

bool DistributionProto::MergePartialFromCodedStream(
    ::google::protobuf::io::CodedInputStream* input) {
#define DO_(EXPRESSION) if (!(EXPRESSION)) return false
  ::google::protobuf::uint32 tag;
  while ((tag = input->ReadTag()) != 0) {
    switch (::google::protobuf::internal::WireFormatLite::GetTagFieldNumber(tag)) {
      // repeated .topicsum.DistributionEntry entry = 1;
      case 1: {
        if (::google::protobuf::internal::WireFormatLite::GetTagWireType(tag) ==
            ::google::protobuf::internal::WireFormatLite::WIRETYPE_LENGTH_DELIMITED) {
         parse_entry:
          DO_(::google::protobuf::internal::WireFormatLite::ReadMessageNoVirtual(
                input, add_entry()));
        } else {
          goto handle_uninterpreted;
        }
        if (input->ExpectTag(10)) goto parse_entry;
        if (input->ExpectAtEnd()) return true;
        break;
      }

      default: {
      handle_uninterpreted:
        if (::google::protobuf::internal::WireFormatLite::GetTagWireType(tag) ==
            ::google::protobuf::internal::WireFormatLite::WIRETYPE_END_GROUP) {
          return true;
        }
        DO_(::google::protobuf::internal::WireFormat::SkipField(
              input, tag, mutable_unknown_fields()));
        break;
      }
    }
  }
  return true;
#undef DO_
}

void DistributionProto::SerializeWithCachedSizes(
    ::google::protobuf::io::CodedOutputStream* output) const {
  // repeated .topicsum.DistributionEntry entry = 1;
  for (int i = 0; i < this->entry_size(); i++) {
    ::google::protobuf::internal::WireFormatLite::WriteMessageMaybeToArray(
      1, this->entry(i), output);
  }

  if (!unknown_fields().empty()) {
    ::google::protobuf::internal::WireFormat::SerializeUnknownFields(
        unknown_fields(), output);
  }
}

::google::protobuf::uint8* DistributionProto::SerializeWithCachedSizesToArray(
    ::google::protobuf::uint8* target) const {
  // repeated .topicsum.DistributionEntry entry = 1;
  for (int i = 0; i < this->entry_size(); i++) {
    target = ::google::protobuf::internal::WireFormatLite::
      WriteMessageNoVirtualToArray(
        1, this->entry(i), target);
  }

  if (!unknown_fields().empty()) {
    target = ::google::protobuf::internal::WireFormat::SerializeUnknownFieldsToArray(
        unknown_fields(), target);
  }
  return target;
}

int DistributionProto::ByteSize() const {
  int total_size = 0;

  // repeated .topicsum.DistributionEntry entry = 1;
  total_size += 1 * this->entry_size();
  for (int i = 0; i < this->entry_size(); i++) {
    total_size +=
      ::google::protobuf::internal::WireFormatLite::MessageSizeNoVirtual(
        this->entry(i));
  }

  if (!unknown_fields().empty()) {
    total_size +=
      ::google::protobuf::internal::WireFormat::ComputeUnknownFieldsSize(
        unknown_fields());
  }
  GOOGLE_SAFE_CONCURRENT_WRITES_BEGIN();
  _cached_size_ = total_size;
  GOOGLE_SAFE_CONCURRENT_WRITES_END();
  return total_size;
}

void DistributionProto::MergeFrom(const ::google::protobuf::Message& from) {
  GOOGLE_CHECK_NE(&from, this);
  const DistributionProto* source =
    ::google::protobuf::internal::dynamic_cast_if_available<const DistributionProto*>(
      &from);
  if (source == NULL) {
    ::google::protobuf::internal::ReflectionOps::Merge(from, this);
  } else {
    MergeFrom(*source);
  }
}

void DistributionProto::MergeFrom(const DistributionProto& from) {
  GOOGLE_CHECK_NE(&from, this);
  entry_.MergeFrom(from.entry_);
  mutable_unknown_fields()->MergeFrom(from.unknown_fields());
}

void DistributionProto::CopyFrom(const ::google::protobuf::Message& from) {
  if (&from == this) return;
  Clear();
  MergeFrom(from);
}

void DistributionProto::CopyFrom(const DistributionProto& from) {
  if (&from == this) return;
  Clear();
  MergeFrom(from);
}

bool DistributionProto::IsInitialized() const {

  for (int i = 0; i < entry_size(); i++) {
    if (!this->entry(i).IsInitialized()) return false;
  }
  return true;
}

void DistributionProto::Swap(DistributionProto* other) {
  if (other != this) {
    entry_.Swap(&other->entry_);
    std::swap(_has_bits_[0], other->_has_bits_[0]);
    _unknown_fields_.Swap(&other->_unknown_fields_);
    std::swap(_cached_size_, other->_cached_size_);
  }
}

::google::protobuf::Metadata DistributionProto::GetMetadata() const {
  protobuf_AssignDescriptorsOnce();
  ::google::protobuf::Metadata metadata;
  metadata.descriptor = DistributionProto_descriptor_;
  metadata.reflection = DistributionProto_reflection_;
  return metadata;
}


// @@protoc_insertion_point(namespace_scope)

}  // namespace topicsum

// @@protoc_insertion_point(global_scope)
