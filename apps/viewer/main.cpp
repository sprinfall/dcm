#include "apps/viewer/main_window.h"

#include <QApplication>
#include <QCommandLineParser>
#include <QDebug>

int main(int argc, char* argv[]) {
  QApplication app(argc, argv);

  QCommandLineParser parser;
  parser.setApplicationDescription("DICOM Viewer");
  parser.addHelpOption();
  parser.addPositionalArgument("file", QCoreApplication::translate("main", "The file to open."));

  // Process the actual command line arguments given by the user.
  parser.process(app);

  QStringList args = parser.positionalArguments();
  if (!args.empty()) {
    qDebug() << "File: " << args;
  }

  MainWindow main_window;
  main_window.OpenFiles(args);

  main_window.showMaximized();

  return app.exec();
}
