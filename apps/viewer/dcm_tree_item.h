#ifndef DCM_TREE_ITEM_H_
#define DCM_TREE_ITEM_H_

#include <QList>
#include <QVariant>

class DcmTreeItem {
public:
  explicit DcmTreeItem(const QList<QVariant>& data,
                       DcmTreeItem* parent_item = nullptr)
      : data_(data), parent_item_(parent_item) {
  }

  ~DcmTreeItem() {
    qDeleteAll(child_items_);
  }

  QVariant data(int column) const {
    return data_.value(column);
  }

  DcmTreeItem* parent_item() const {
    return parent_item_;
  }

  void AppendChild(DcmTreeItem* item) {
    child_items_.append(item);
  }

  DcmTreeItem* child(int row) {
    return child_items_.value(row);
  }

  int GetChildCount() const {
    return child_items_.count();
  }

  int GetColumnCount() const {
    return data_.count();
  }

  int row() const {
    if (parent_item_ != nullptr) {
      return parent_item_->child_items_.indexOf(const_cast<DcmTreeItem*>(this));
    }
    return 0;
  }

private:
  QList<QVariant> data_;
  DcmTreeItem* parent_item_;
  QList<DcmTreeItem*> child_items_;
};

#endif // DCM_TREE_ITEM_H_
