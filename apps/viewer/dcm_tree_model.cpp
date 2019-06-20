#include "apps/viewer/dcm_tree_model.h"

#include <sstream>

#include "dcm/data_dict.h"
#include "dcm/data_element.h"
#include "dcm/data_sequence.h"
#include "dcm/dicom_file.h"

#if !ADD_TREE_ITEM_RECURSIVELY
#include "dcm/visitor.h"
#endif

#include "apps/print.h"
#include "apps/viewer/dcm_tree_item.h"

// Get column data from a given data element.
static QList<QVariant> GetColumnData(const dcm::DataElement* data_element) {
  QList<QVariant> column_data;

  std::stringstream ss;
  data_element->tag().Print(ss);
  QString tag = QString::fromStdString("(" + ss.str() + ")");

  QString vr(2, ' ');
  vr[0] = data_element->vr().byte1();
  vr[1] = data_element->vr().byte2();

  QString name = QString::fromStdString(dcm::dict::GetName(data_element->tag()));

  column_data << tag << vr << data_element->length() << name;

  // Value
  if (data_element->vr() == dcm::VR::SQ) {
    column_data << "";
  } else {
    std::stringstream value_ss;
    PrintValue(value_ss, data_element);
    column_data << QString::fromStdString(value_ss.str());
  }

  return column_data;
}

#if !ADD_TREE_ITEM_RECURSIVELY

// TODO: Define the Visitor class.

#endif  // if !ADD_TREE_ITEM_RECURSIVELY

DcmTreeModel::DcmTreeModel(QObject* parent)
    : QAbstractItemModel(parent), root_item_(nullptr) {
}

DcmTreeModel::~DcmTreeModel() {
  if (root_item_ != nullptr) {
    delete root_item_;
  }
}

bool DcmTreeModel::LoadFile(const QString& file_path) {
  dcm::DicomFile dicom_file(dcm::Path(file_path.toStdString()));

  if (!dicom_file.Load()) {
    // Cannot load the file.
    return false;
  }
  
  // TODO: Value before Name.

  QList<QVariant> root_data;
  root_data << "Tag" << "VR" << "Size" << "Name" << "Value";
  root_item_ = new DcmTreeItem(root_data);

#if ADD_TREE_ITEM_RECURSIVELY

  SetupModelData(&dicom_file, root_item_);

#else

  // Using visitor avoids casting DataElement to DataSet.
  // TODO

#endif  // ADD_TREE_ITEM_RECURSIVELY

  return true;
}

int DcmTreeModel::columnCount(const QModelIndex& parent) const {
  if (parent.isValid()) {
    return static_cast<DcmTreeItem*>(
        parent.internalPointer())->GetColumnCount();
  } else {
    return root_item_->GetColumnCount();
  }
}

QVariant DcmTreeModel::data(const QModelIndex& index, int role) const {
  if (!index.isValid()) {
    return QVariant();
  }

  if (role != Qt::DisplayRole) {
    return QVariant();
  }

  DcmTreeItem* item = static_cast<DcmTreeItem*>(index.internalPointer());

  return item->data(index.column());
}

Qt::ItemFlags DcmTreeModel::flags(const QModelIndex& index) const {
  if (!index.isValid()) {
    return 0;
  }

  return QAbstractItemModel::flags(index);
}

QVariant DcmTreeModel::headerData(int section, Qt::Orientation orientation,
                                  int role) const {
  if (orientation == Qt::Horizontal && role == Qt::DisplayRole) {
    return root_item_->data(section);
  }

  return QVariant();
}

QModelIndex DcmTreeModel::index(int row, int column,
                                const QModelIndex& parent) const {
  if (!hasIndex(row, column, parent)) {
    return QModelIndex();
  }

  DcmTreeItem* parent_item = nullptr;

  if (!parent.isValid()) {
    parent_item = root_item_;
  } else {
    parent_item = static_cast<DcmTreeItem*>(parent.internalPointer());
  }

  DcmTreeItem* child_item = parent_item->child(row);
  if (child_item) {
    return createIndex(row, column, child_item);
  } else {
    return QModelIndex();
  }
}

QModelIndex DcmTreeModel::parent(const QModelIndex& index) const {
  if (!index.isValid()) {
    return QModelIndex();
  }

  DcmTreeItem* child_item = static_cast<DcmTreeItem*>(index.internalPointer());
  DcmTreeItem* parent_item = child_item->parent_item();

  if (parent_item == root_item_) {
    return QModelIndex();
  }

  return createIndex(parent_item->row(), 0, parent_item);
}

int DcmTreeModel::rowCount(const QModelIndex& parent) const {
  if (parent.column() > 0) {
    return 0;
  }

  DcmTreeItem* parent_item = nullptr;

  if (!parent.isValid()) {
    parent_item = root_item_;
  } else {
    parent_item = static_cast<DcmTreeItem*>(parent.internalPointer());
  }

  return parent_item->GetChildCount();
}

#if ADD_TREE_ITEM_RECURSIVELY

void DcmTreeModel::SetupModelData(const dcm::DataSet* data_set,
                                  DcmTreeItem* parent_item) {
  for (std::size_t i = 0; i < data_set->size(); ++i) {
    const dcm::DataElement* data_element = data_set->At(i);

    QList<QVariant> column_data = GetColumnData(data_element);

    DcmTreeItem* item = new DcmTreeItem(column_data, parent_item);

    if (data_element->vr() == dcm::VR::SQ) {
      const dcm::DataSequence* data_seq =
          dynamic_cast<const dcm::DataSequence*>(data_element);
      if (data_seq != nullptr) {
        SetupModelData(data_seq, item);
      }
    }

    parent_item->AppendChild(item);
  }
}

void DcmTreeModel::SetupModelData(const dcm::DataSequence* data_sequence,
                                  DcmTreeItem* parent_item) {
  for (std::size_t i = 0; i < data_sequence->size(); ++i) {
    const auto& data_item = data_sequence->At(i);

    if (data_item.prefix != nullptr) {
      QList<QVariant> column_data = GetColumnData(data_item.prefix);
      DcmTreeItem* item = new DcmTreeItem(column_data, parent_item);
      parent_item->AppendChild(item);
    }

    SetupModelData(data_sequence->At(i).data_set, parent_item);

    if (data_item.delimitation != nullptr) {
      QList<QVariant> column_data = GetColumnData(data_item.delimitation);
      DcmTreeItem* item = new DcmTreeItem(column_data, parent_item);
      parent_item->AppendChild(item);
    }
  }
}

#endif  // ADD_TREE_ITEM_RECURSIVELY
