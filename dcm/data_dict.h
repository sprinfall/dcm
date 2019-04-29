#ifndef DCM_DATA_DICT_H_
#define DCM_DATA_DICT_H_

// Data dictionary singleton.

#include "dcm/data_entry.h"
#include "dcm/singleton_base.h"
#include "dcm/tag.h"

namespace dcm {

class DataDict : public SingletonBase<DataDict> {
public:
  ~DataDict() = default;

  const DataEntry* FindEntry(Tag tag) const;

private:
  DataDict() = default;

  friend class SingletonBase<DataDict>;
};

}  // namespace dcm

#endif  // DCM_DATA_DICT_H_
