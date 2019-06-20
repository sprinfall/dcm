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

  // Central widgets

  QWidget* central_widget = new QWidget(this);
  setCentralWidget(central_widget);

  resize(800, 600);
}

void MainWindow::OpenFiles(const QStringList& files) {
  for (const QString& file : files) {
    DcmTreeModel* tree_model = new DcmTreeModel();

    if (!tree_model->LoadFile(file)) {
      delete tree_model;
      continue;
    }

    QTreeView* tree_view = new QTreeView(centralWidget());
    tree_view->setModel(tree_model);

    // Configure tree view.

    tree_view->setWindowTitle("DICOM Data Set");

    // TODO: Right-align the Size column.
    tree_view->setColumnWidth(0, 200);    // Tag
    tree_view->setColumnWidth(1, 40);     // VR
    tree_view->setColumnWidth(2, 80);     // Size
    tree_view->setColumnWidth(3, 300);    // Name

    tree_view->expandAll();

    // Create image display widget.

    QWidget* image_widget = new QWidget(centralWidget());

    QHBoxLayout* hlayout = new QHBoxLayout();

    hlayout->addWidget(tree_view, 1);
    hlayout->addWidget(image_widget, 1);

    centralWidget()->setLayout(hlayout);

    // TODO: Use tab control to display multiple files.
    break;
  }
}
