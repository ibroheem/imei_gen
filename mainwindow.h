#ifndef MAINWINDOW_H
#define MAINWINDOW_H

using namespace mvc_utils::io;
using namespace mvc_utils::string_operations_Qt;
using namespace mvc_utils::patches;
using namespace pugi_operations::write;
using namespace pugi;
using namespace std;

QString load_file(const QString &fl)
{
   QString tmp;
   if (!fl.isEmpty())
   {
      QFile file(fl);
      if (file.open(QIODevice::ReadOnly))
      {
         QTextStream in(&file);
         tmp = in.readAll();
      }
   }
   return tmp;
}

class MainWindow : public QMainWindow
{
   public:
      MainWindow(QWidget *parent = 0);

      void set_buttons();
      void set_combo();
      void set_lineedit();
      void set_menus();
      void set_widgets();
      void set_window_title(const QString &_str)
      { setWindowTitle(QString("[%1] - ATNG").arg(_str)); }

   public :
      void calc_luhn();
      void clear_combo_digits() { m_combo_digits->lineEdit()->clear(); }
      void next_luhn() {  }
      void prev_luhn() {  }

      bool read_settings();
      bool write_settings();

      void open_as();
      void save_as();
      void save_results();
      void quit() { write_settings(); cout<<m_config_file; qApp->quit(); }

   private:
      QWidget     *m_widget = new QWidget;
      QGridLayout *m_grid_layout = new QGridLayout;
      QComboBox   *m_combo_digits = new QComboBox, *m_combo_gens   = new QComboBox;
      QLineEdit   *m_lineedit_digits = new QLineEdit;

      QPushButton *m_button_calc   = new QPushButton("Calculate"),
                  *m_button_prev =  new QPushButton("Prev"),
                  *m_button_next =  new QPushButton("Next"),
                  *m_button_clear =  new QPushButton("Clear"),
                  *m_button_exit =  new QPushButton("Exit");

      TheTabs     *m_tabs = new TheTabs;
      QTextEdit   *m_display = new QTextEdit;
      vector<QTextEdit*> m_displays = {};

   private:
      luhn::luhn ln;
      xml_document doc;
      xml_node header;

      QIntValidator validator_int;
      QFileDialog *m_file_dialog = new QFileDialog;
      TheMenu *m_menus = new TheMenu(1, {"File"});
      QMenu   *new_menu = m_menus->add_menu_to(0, "New");
      QString m_config_path = QDir::homePath().append("/.config/ATNG");
      string  m_config_file  = m_config_path.append("/default.conf").toStdString();
};

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent)
{
   setWindowTitle("ATNG");
   set_buttons();
   set_combo();
   set_lineedit();
   set_menus();
   set_widgets();
   setCentralWidget(m_widget);

   read_settings();
}

void MainWindow::calc_luhn()
{
   int gens = m_combo_gens->currentText().toInt();
   std::string _digits = m_combo_digits->currentText().toStdString(), tmp;

   ln.set_digits(_digits); //354103039861234 -- test

   if (ln.state() == luhn::luhn::digits_state::valid)
   {
      m_combo_digits->addItem(_digits.c_str());
      ln.calculate();
      if (!ln.is_valid())
         ln.set_digits(ln.complete());

      for (int i = 0; i < gens; ++i)
      {
         tmp += ln.digits() + "\n";
         ln.set_digits(ln.next_luhn());
      }
      ln.set_digits(_digits); //set it to original state

      m_tabs->current_widget<QTextEdit>()->setText(tmp.c_str());
      m_tabs->setTabText(m_tabs->currentIndex(), m_combo_digits->currentText());
      set_window_title(m_combo_digits->currentText());
   }
   else
      m_display->setText("Invalid Digits, Check Digits");
}

bool MainWindow::read_settings()
{
   xml_parse_result rs = doc.load_file(m_config_file.c_str());
   if (rs.status == xml_parse_status::status_file_not_found)
      doc.save_file(m_config_file.c_str());

   header = doc.find_child_by_attribute("header", "type", "config");
   if (header.empty())
   {
      header = doc.append_child("header");
      append_attribute(header, "type", "config");
   }

   m_tabs->current_widget<QTextEdit>()->setText(load_file(""));
   return false;
}

bool MainWindow::write_settings()
{
   doc.save_file(m_config_file.c_str());
   return false;
}

void MainWindow::open_as()
{
   QString fname = m_file_dialog->getOpenFileName(this, "Open As", m_config_path,
         "All Files(*.*);;Text files (*.txt);;XML files (*.xml)"),
         fname_wo_ext = file_name_wo_ext(fname);
   if (!fname.isEmpty())
   {
      QFile file(fname);
      if (!file.open(QIODevice::ReadOnly))
      {
         QMessageBox::information(this, tr("Warning!"), tr("Could not open file"));
         return;
      }
      QTextStream in(&file);
      m_tabs->current_widget<QTextEdit>()->setText(in.readAll());
      file.close();

      m_combo_digits->setCurrentText(fname_wo_ext);
      set_window_title(fname_wo_ext);
      xml_node rc = header.append_child("recent");
      append_attributes(rc, {"type", "digits"}, {"imei", fname_wo_ext.toUtf8().constData()});
   }
}

void MainWindow::save_as()
{
   QString fname = m_file_dialog->getSaveFileName(this, "Save As", m_config_path,
         "All Files(*.*);;Text files (*.txt);;XML files (*.xml)");
   if (!fname.isEmpty())
   {
      QFile file(fname);
      if (!file.open(QIODevice::WriteOnly))
      {
         QMessageBox::information(this, "Warning", "File In Read-Only Mode");
      }
      else
      {
         QTextStream stream(&file);
         stream << m_tabs->current_widget<QTextEdit>()->toPlainText();
         stream.flush();
         file.close();

         setWindowTitle(QString("[%2] - %1").arg(windowTitle()).arg(file_name_w_ext(fname)));
      }
   }
}

void MainWindow::set_buttons()
{
   connect(m_button_calc, &QPushButton::clicked, [&]() { calc_luhn(); });
   connect(m_button_clear, &QPushButton::clicked, [&]() { clear_combo_digits(); });
   connect(m_button_next, &QPushButton::clicked, [&]() { next_luhn(); });
   connect(m_button_prev, &QPushButton::clicked, [&]() { prev_luhn(); });
   connect(m_button_exit, &QPushButton::clicked, [&]() { quit(); });
}

void MainWindow::set_combo()
{
   for (int i = 1; i < 10; ++i)
      m_combo_gens->addItem(QString("%1").arg(i*5));

   m_combo_digits->setEditable(true);
   m_combo_gens->setEditable(true);
}

void MainWindow::set_lineedit()
{
   m_lineedit_digits->setValidator(&validator_int);
}

void MainWindow::set_menus()
{
   menuBar()->addMenu(m_menus->menu_at(0));
   new_menu->addAction("Tab")->setShortcut(QKeySequence("Ctrl+T"));
   new_menu->addAction("Window")->setShortcut(QKeySequence("Ctrl+N"));
   m_menus->add_actions(0, {"Open...", "Save...", "Quit"});

   m_menus->set_shortcuts(0, {"", "Ctrl+O", "Ctrl+Alt+S"});
   m_menus->set_shortcut(0, 3, "Ctrl+Q");

   connect(m_menus->actions(0)[1], &QAction::triggered, [&](bool) { open_as(); });
   connect(m_menus->actions(0)[2], &QAction::triggered, [&](bool) { save_as(); });
   connect(m_menus->actions(0)[3], &QAction::triggered, [&](bool) { quit();});

   connect(new_menu->actions()[0], &QAction::triggered, [&]() { m_tabs->add_tab<QTextEdit>("New Tab"); });
#ifdef Q_OS_ANDROID
   menuBar()->setVisible(false);
#endif // Q_OS_ANDROID
}

void MainWindow::set_widgets()
{
#ifdef Q_OS_ANDROID
   m_grid_layout->addWidget(m_lineedit_digits, 0, 0, 1, 2);
#else
   m_grid_layout->addWidget(m_combo_digits, 0, 0, 1, 2);
#endif // Q_OS_ANDROID

   m_grid_layout->addWidget(m_combo_gens, 0, 2, 1, 1);

   m_grid_layout->addWidget(m_button_calc, 1, 0, 1, 1);
   m_grid_layout->addWidget(m_button_clear, 1, 1, 1, 1);
   m_grid_layout->addWidget(m_button_exit, 1, 2, 1, 1);

   m_grid_layout->addWidget(m_button_prev, 2, 0, 1, 1);
   m_grid_layout->addWidget(m_button_next, 2, 2, 1, 1);

   m_grid_layout->addWidget(m_tabs, 3, 0, 1, 5);
   m_widget->setLayout(m_grid_layout);

   m_tabs->add_tab(m_display, "Imei Gen 1");
   for (int i = 0; i < 1; ++i)
   {
      m_displays.push_back(new QTextEdit(this));
      m_tabs->add_tab(m_displays[i], "New Tab");
   }

   connect(m_tabs, &TheTabs::currentChanged,
   [&](int i) { setWindowTitle(QString("[%2] - %1").arg("ATNG").arg(m_tabs->tabText(i))); });
}

#endif // MAINWINDOW_H
