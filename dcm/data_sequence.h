#ifndef DCM_DATA_SEQUENCE_H_
#define DCM_DATA_SEQUENCE_H_

#include <cstdint>
#include <string>
#include <vector>

#include "dcm/data_element.h"

namespace dcm {

class DataSet;
class Visitor;

class DataSequence : public DataElement {
public:
  struct Item {
    // Sequence item prefix tag (fffe,e000).
    DataElement* prefix;

    // Sequence item delimitation/end tag (fffe,e00d).
    // If the prefix tag has a value length other than -1, the delimiation
    // normally is absent.
    DataElement* delimitation;

    DataSet* data_set;
  };

public:
  DataSequence(Tag tag, bool explicit_vr, ByteOrder byte_order);

  ~DataSequence() override;

  void Accept(Visitor& visitor) const override;

  std::size_t size() const { return items_.size(); }

  const Item& GetItem(std::size_t index) const {
    assert(index < items_.size());
    return items_[index];
  }

  const DataElement* delimitation() const { return delimitation_; }

  void set_delimitation(DataElement* delimitation) {
    delimitation_ = delimitation;
  }

  // Start a new item.
  void NewItem(DataElement* prefix);

  // End the last item with a delimitation.
  // This call is optional. If the prefix has a value length other than -1,
  // a sequence item normally doesn't have any delimitation.
  void EndItem(DataElement* delimitation);

  // Append the data element to the last item's data set.
  bool AppendToLastItem(DataElement* data_element);

  // Clear all items.
  void Clear();

private:
  bool explicit_vr_;

  // Sequence items.
  std::vector<Item> items_;

  // Sequence delimitation/end tag (fffe,e0dd).
  // If the sequence tag has a value length other than -1, the delimiation
  // normally is absent.
  DataElement* delimitation_;
};

}  // namespace dcm

#endif  // DCM_DATA_SEQUENCE_H_
