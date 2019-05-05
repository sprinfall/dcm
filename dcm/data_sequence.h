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
  DataSequence(Tag tag = Tag(), Endian endian = Endian::Little());

  ~DataSequence() override;

  void Accept(Visitor& visitor) const override;

  std::size_t size() const { return items_.size(); }

  const Item& GetItem(std::size_t index) const {
    assert(index < items_.size());
    return items_[index];
  }

  bool delimitated() const { return delimitated_; }
  void set_delimitated(bool delimitated) { delimitated_ = delimitated; }

  // Start a new item.
  void NewItem(DataElement* prefix);

  // End the current item with a delimitation.
  // This call is optional. If the prefix has a value length other than -1,
  // a sequence item normally doesn't have any delimitation.
  void EndItem(DataElement* delimitation);

  // Get the last item's data set.
  DataSet* LastDataSet();

private:
  // Sequence items.
  std::vector<Item> items_;

  // With (fffe,e0dd) ending the sequence or not.
  bool delimitated_;
};

}  // namespace dcm

#endif  // DCM_DATA_SEQUENCE_H_
