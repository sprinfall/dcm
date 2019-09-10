// A read handler to read data elements only for the specific tags.
// It avoids loading the full data set, so it's very fast.
class TagsReadHandler : public ReadHandler {
public:
  explicit TagsReadHandler(DataSet* data_set);

  ~TagsReadHandler() override;

  void OnEndian(Endian endian) override;
  void OnExplicitVR(bool explicit_vr) override;

  bool OnElementStart(Tag tag) override;
  void OnElementEnd(DataElement* data_element) override;

  void OnSequenceStart(DataSet* data_set) override;
  void OnSequenceEnd() override;

  // Add a tag to read.
  TagsReadHandler& AddTag(Tag tag);

  // Delete all tags.
  void ClearTags();

private:
  // Tags (sorted) to read.
  std::vector<Tag> tags_;

  DataSet* data_set_;
  std::vector<DataSet*> data_set_stack_;
};


TagsReadHandler::TagsReadHandler(DataSet* data_set)
    : data_set_(data_set) {
  data_set_stack_.push_back(data_set);
}

TagsReadHandler::~TagsReadHandler() {
  ClearTags();
}

void TagsReadHandler::OnEndian(Endian endian) {
  data_set_->set_endian(endian);
}

void TagsReadHandler::OnExplicitVR(bool explicit_vr) {
  data_set_->set_explicit_vr(explicit_vr);
}

bool TagsReadHandler::OnElementStart(Tag tag) {
  if (tags_.empty()) {
    // All tags have been read, stop the process.
    should_stop_ = true;
    return false;
  }

  if (tag.group() != 0xFFFE && tag > tags_.back()) {
    // Larger than the last tag to read, stop the process.
    should_stop_ = true;
    return false;
  }

  auto it = std::find(tags_.begin(), tags_.end(), tag);

  if (it == tags_.end()) {
    // Not a tag to read, skip it.
    return false;
  }

  // Remove the tag.
  tags_.erase(it);

  return true;
}

void TagsReadHandler::OnElementEnd(DataElement* data_element) {
  assert(!data_set_stack_.empty());
  data_set_stack_.back()->AppendElement(data_element);
}

void TagsReadHandler::OnSequenceStart(DataSet* data_set) {
  assert(!data_set_stack_.empty());
  data_set_stack_.back()->AppendElement(data_set);
  data_set_stack_.push_back(data_set);
}

void TagsReadHandler::OnSequenceEnd(DataSet* /*data_set*/) {
  data_set_stack_.pop_back();
}

TagsReadHandler& TagsReadHandler::AddTag(Tag tag) {
  if (tags_.empty()) {
    tags_.push_back(tag);
    return *this;
  }

  // Make sure the tags are sorted.
  auto lower = std::lower_bound(tags_.begin(), tags_.end(), tag);
  if (lower == tags_.end() || *lower != tag) {
    tags_.insert(lower, tag);
  }

  return *this;
}

void TagsReadHandler::ClearTags() {
  tags_.clear();
}
