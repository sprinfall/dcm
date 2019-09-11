#include "apps/viewer/main_window.h"

#include <QtWidgets>

#include "apps/viewer/dcm_tree_model.h"

MainWindow::MainWindow(QWidget* parent) : QMainWindow(parent) {
  setWindowTitle(tr("DICOM Viewer"));

  // Actions

  QAction* open_action = new QAction(tr("&Open..."), this);
  open_action->setShortcuts(QKeySequence::Open);
  open_action->setStatusTip(tr("Open an existing file"));
  //    connect(open_action, &QAction::triggered, this, &MainWindow::open);

  // Menu

  QMenu* file_menu = menuBar()->addMenu(tr("&File"));
  file_menu->addAction(open_action);
  file_menu->addSeparator();

  // ToolBar
#if 0
  QToolBar* file_tool_bar = addToolBar(tr("File"));
  file_tool_bar->addAction(open_action);
  file_tool_bar->setAllowedAreas(Qt::TopToolBarArea);
  addToolBar(Qt::TopToolBarArea, file_tool_bar);
#endif

  // Central widget
  QWidget* central_widget = new QWidget(this);
  setCentralWidget(central_widget);
}

void MainWindow::OpenFiles(const QStringList& files) {
  for (const QString& file : files) {
    DcmTreeModel* tree_model = new DcmTreeModel();

    if (!tree_model->LoadFile(file)) {
      delete tree_model;
      continue;
    }

    QSplitter* splitter = new QSplitter();
    QHBoxLayout* layout = new QHBoxLayout();
    layout->addWidget(splitter, 1);
    centralWidget()->setLayout(layout);

    QTreeView* tree_view = new QTreeView(splitter);
    tree_view->setModel(tree_model);

    // Configure tree view.

    tree_view->setWindowTitle("DICOM Data Set");

    tree_view->setColumnWidth(0, 200);  // Tag
    tree_view->setColumnWidth(1, 40);   // VR
    tree_view->setColumnWidth(2, 80);   // Size
    tree_view->setColumnWidth(3, 300);  // Name

    // Use different background colors for even and odd rows.
    // tree_view->setAlternatingRowColors(true);

    // Forbid to move the headers.
    // The first column of tree view is always unmovable.
    // tree_view->header()->setSectionsMovable(false);

    tree_view->expandAll();

    // Create image display widget.
    // TODO
    QWidget* image_widget = new QWidget(splitter);

    splitter->addWidget(tree_view);
    splitter->addWidget(image_widget);

    // TODO: Use tab control to display multiple files.
    break;
  }
}
