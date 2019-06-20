#ifndef MAIN_WINDOW_H_
#define MAIN_WINDOW_H_

#include <QMainWindow>

class MainWindow : public QMainWindow {
  Q_OBJECT

public:
  explicit MainWindow(QWidget* parent = nullptr);

  ~MainWindow() = default;

  // Open DICOM files.
  void OpenFiles(const QStringList& files);
};

#endif  // MAIN_WINDOW_H_
