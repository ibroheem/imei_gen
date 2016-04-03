#ifndef MAINWINDOW_DROID_H
#define MAINWINDOW_DROID_H

using namespace mvc_utils::io;
using namespace mvc_utils::string_operations_Qt;
using namespace mvc_utils::patches;
using namespace pugi_operations;
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

      void retranslate_ui();
      void set_buttons();
      void set_lineedit();
      void set_menus();
      void set_widgets();
      void set_window_title(const QString &_str)
      { setWindowTitle(QString("[%1] - IMEI GENERATOR").arg(_str)); }

   public :
      void clear_input_imei() { lineedit_imei->clear(); }
      void display_imei_gen();
      void generate_imei();
      void validate_imei();

      void quit() { qApp->quit(); }

   private:
      QWidget     *widget_imei = new QWidget;
      QGridLayout *grid_layout_widget_imei = new QGridLayout,
                  *grid_layout_buttons = new QGridLayout;

      QLineEdit   *lineedit_imei = new QLineEdit,
                  *lineedit_generations = new QLineEdit;

      QLabel      *label_imei = new QLabel,
                  *label_generations = new QLabel;

      QPushButton *button_generate_imei  =  new QPushButton,
                  *button_verify_imei =  new QPushButton,
                  *button_clear_imei  =  new QPushButton,
                  *button_exit        =  new QPushButton;

      QSizePolicy size_policy = QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

      TheTabs *m_tabs = new TheTabs;
      TheMenu *m_menus = new TheMenu(1, {"File"});
      QMenu   *new_menu = m_menus->add_menu_to(0, "New");
      vector<QTextEdit*> m_displays = {};

      vector<cchar*> generations;

   private:
      luhn::luhn ln;

      QRegExp rx{"[0-9]\\d{0,18}"};
      QIntValidator validator_int;
      QValidator *validator = new QRegExpValidator(rx, this);
};

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent)
{
   QApplication::setPalette(QApplication::style()->standardPalette());
   setWindowTitle("IMEI GENERATOR");
   set_buttons();
   set_lineedit();
   set_menus();
   set_widgets();
   setCentralWidget(widget_imei);
   retranslate_ui();
}

void MainWindow::generate_imei()
{
   int gens = lineedit_generations->text().toInt();
   if (gens > 2000)
   {
     QMessageBox::information(nullptr, "Information", "Number Of Generations too High!");
     return ;
   }

   if (m_tabs->count() <= 0)
   {
     QMessageBox::information(nullptr, "Information", "NO Tabs, Open A tab Using  Menu-> New-> Tab!");
     return;
   }

   std::string _digits = lineedit_imei->text().toStdString();

   ln.set_digits(_digits); //354103039861234 -- test

   if (ln.state() == luhn::luhn::digits_state::valid)
   {
      m_tabs->setTabText(m_tabs->currentIndex(), _digits.c_str());
      set_window_title(_digits.c_str());

      ln.calculate();
      if (!ln.is_valid())
         ln.set_digits(ln.complete());

      m_tabs->current_widget<QTextEdit>()->clear();

      for (int i = 0; i < gens; ++i)
      {
         generations.push_back(ln.digits().c_str());
         m_tabs->current_widget<QTextEdit>()->append(generations.back());
         ln.set_digits(ln.next_luhn());
      }
      generations.clear();
      ln.set_digits(_digits); //set it to original state
   }
   else
      m_tabs->current_widget<QTextEdit>()->setText("Invalid Digits, Check IMEI");
}

void MainWindow::validate_imei()
{
   std::string _digits = lineedit_imei->text().toStdString();

   ln.set_digits(_digits); //354103039861234 -- test

   if (ln.state() == luhn::luhn::digits_state::valid)
   {
      ln.calculate();
      if (!ln.is_valid())
      {
         m_tabs->current_widget<QTextEdit>()->setText("Invalid IMEI!");
         return ;
      }
      m_tabs->current_widget<QTextEdit>()->setText("Valid IMEI!");
   }
   return ;
}

void MainWindow::set_buttons()
{
   size_policy.setHorizontalStretch(0);
   size_policy.setVerticalStretch(0);

   size_policy.setHeightForWidth(button_clear_imei->sizePolicy().hasHeightForWidth());
   button_clear_imei->setSizePolicy(size_policy);

   size_policy.setHeightForWidth(button_generate_imei->sizePolicy().hasHeightForWidth());
   button_generate_imei->setSizePolicy(size_policy);

   size_policy.setHeightForWidth(button_verify_imei->sizePolicy().hasHeightForWidth());
   button_verify_imei->setSizePolicy(size_policy);

   size_policy.setHeightForWidth(button_exit->sizePolicy().hasHeightForWidth());
   button_exit->setSizePolicy(size_policy);

   size_policy.setHeightForWidth(button_generate_imei->sizePolicy().hasHeightForWidth());
   button_generate_imei->setSizePolicy(size_policy);

   connect(button_generate_imei, &QPushButton::clicked, [&]() { generate_imei(); });
   connect(button_verify_imei, &QPushButton::clicked,
           [&]() { validate_imei(); });

   connect(button_clear_imei, &QPushButton::clicked, [&]() { clear_input_imei(); });
   connect(button_exit, &QPushButton::clicked, [&]() { quit(); });
}

void MainWindow::set_lineedit()
{
   lineedit_imei->setValidator(validator);
   lineedit_generations->setValidator(&validator_int);

   lineedit_generations->resize(lineedit_generations->width(), 20);
   lineedit_generations->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
}

void MainWindow::set_menus()
{
   menuBar()->addMenu(m_menus->menu_at(0));
   new_menu->addAction("Tab")->setShortcut(QKeySequence("Ctrl+T"));
   new_menu->addAction("Window")->setShortcut(QKeySequence("Ctrl+N"));
   m_menus->add_actions(0, {"Open...", "Save...", "Quit"});

   m_menus->set_shortcuts(0, {"", "Ctrl+O", "Ctrl+Alt+S"});
   m_menus->set_shortcut(0, 3, "Ctrl+Q");

   connect(m_menus->actions(0)[3], &QAction::triggered, [&](bool) { quit();});

   connect(new_menu->actions()[0], &QAction::triggered, [&]() { m_tabs->add_tab<QTextEdit>("New Tab"); });
#ifdef Q_OS_ANDROID
   menuBar()->setVisible(false);
#endif // Q_OS_ANDROID
}

void MainWindow::set_widgets()
{
   grid_layout_widget_imei->addWidget(label_imei, 0, 0, 1, 1);
   grid_layout_widget_imei->addWidget(lineedit_imei, 0, 1, 1, 1);
   grid_layout_widget_imei->addWidget(lineedit_generations, 1, 1, 1, 1);
   grid_layout_widget_imei->addWidget(m_tabs, 2, 0, 1, 2);
   grid_layout_widget_imei->addWidget(label_generations, 1, 0, 1, 1);

   grid_layout_buttons->addWidget(button_clear_imei, 0, 1, 1, 1);
   grid_layout_buttons->addWidget(button_verify_imei, 1, 0, 1, 1);
   grid_layout_buttons->addWidget(button_exit, 1, 1, 1, 1);
   grid_layout_buttons->addWidget(button_generate_imei, 0, 0, 1, 1);

   grid_layout_widget_imei->addLayout(grid_layout_buttons, 3, 0, 1, 2);
   widget_imei->setLayout(grid_layout_widget_imei);

   for (int i = 0; i < 2; ++i)
   {
      m_displays.push_back(new QTextEdit(this));
      m_tabs->add_tab(m_displays[i], "New Tab");
   }

   connect(m_tabs, &TheTabs::currentChanged,
           [&](int i) { setWindowTitle(QString("[%2] - %1").arg("IMEI GENERATOR").arg(m_tabs->tabText(i))); });
}

void MainWindow::retranslate_ui()
{
   widget_imei->setWindowTitle(trUtf8("Form"));
   label_imei->setText(trUtf8("Start IMEI:"));
   label_generations->setText(trUtf8("No Of Generations: "));
   button_clear_imei->setText(trUtf8("Clear IMEI Input"));
   button_verify_imei->setText(trUtf8("Verify IMEI"));
   button_exit->setText(trUtf8("Exit"));
   button_generate_imei->setText(trUtf8("Generate"));
}

#endif//MAINWINDOW_DROID_H
