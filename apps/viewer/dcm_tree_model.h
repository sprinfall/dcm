#ifndef DCM_TREE_MODEL_H_
#define DCM_TREE_MODEL_H_

#include <QAbstractItemModel>
#include <QModelIndex>
#include <QVariant>

// 1: Use recursive function to add items.
// 0: Use visitor pattern instead.
#define ADD_TREE_ITEM_RECURSIVELY 1

namespace dcm {
class DataSequence;
class DataSet;
}

class DcmTreeItem;

class DcmTreeModel : public QAbstractItemModel {
  Q_OBJECT

public:
  explicit DcmTreeModel(QObject* parent = nullptr);

  ~DcmTreeModel();

  // Load tree model from a file.
  bool LoadFile(const QString& file_path);

  QVariant data(const QModelIndex& index, int role) const override;

  Qt::ItemFlags flags(const QModelIndex& index) const override;

  QVariant headerData(int section, Qt::Orientation orientation,
                      int role = Qt::DisplayRole) const override;

  QModelIndex index(int row, int column,
                    const QModelIndex& parent = QModelIndex()) const override;

  QModelIndex parent(const QModelIndex& index) const override;

  int rowCount(const QModelIndex& parent = QModelIndex()) const override;
  int columnCount(const QModelIndex& parent = QModelIndex()) const override;

private:

#if ADD_TREE_ITEM_RECURSIVELY

  void SetupModelData(const dcm::DataSet* data_set, DcmTreeItem* parent_item);

  void SetupModelData(const dcm::DataSequence* data_sequence,
                      DcmTreeItem* parent_item);

#endif  // ADD_TREE_ITEM_RECURSIVELY

  DcmTreeItem* root_item_;
};

#endif  // DCM_TREE_MODEL_H_
