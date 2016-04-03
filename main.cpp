//#ifndef __cplusplus
//  #define __cplusplus 201402L
//#endif

#include <QApplication>
//#include <QtConcurrent/QtConcurrent>
#include <QTextStream>
#include <QMainWindow>
#include <QGridLayout>
#include <QMenuBar>
#include <QFileDialog>
#include <QComboBox>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QTextEdit>

#include "headers/generic_tab_non_cont_win.h"
#include "headers/string.h"
#include "headers/io.h"
#include "headers/string_operations.h"
#include "headers/pugi_operations.h"
#include "headers/luhn.h"

#include "generic_menu.h"
#include "mainwindow_droid.h"
//#include "mainwindow.h"

int main(int argc, char *argv[])
{
  QApplication app(argc, argv);

  app.setStyle("Oxygen");
  MainWindow *w = new MainWindow;

  w->resize(480, 480);
  w->show();

  return app.exec();
}
