#ifndef GENERIC_MENU_H
#define GENERIC_MENU_H

using namespace std;

QVector<QMenu*> init_menus(int _sz)
{
  QVector<QMenu*> v;
  for (int i = 0; i < _sz; ++i)
    v.append(new QMenu);
  return v;
}

QVector<QMenu*> init_menus(int _sz, initializer_list<QString>& _title)
{
  QVector<QMenu*> v;
  auto b = _title.begin();

  for (int i = 0; i < _sz; ++i)
    v.append(new QMenu(*b++));

  return v;
}

class TheMenu : public QObject
{
  public:
    TheMenu();
    TheMenu(int, initializer_list<QString>&&);

    bool crazy(int i) { return i < 0 || i >= m_menus.size(); }

    void test2();

    QAction* action(int i, int j) { return m_menus[i]->actions()[j]; }
    QList<QAction*> actions(int i) { return m_menus[i]->actions(); }

    void add_action(int i, const QString&);
    void add_actions(int i, initializer_list<QString>&&);

    template <class ...Args>
    QMenu* add_menu_to(int i, Args&&... args)
    {
      QMenu *tmp = new QMenu(args...);
      m_menus[i]->addMenu(tmp);
      return tmp;
    }

    template <class ...Args>
    void add_menu(Args&&...);
    void add_menus(int i, initializer_list<QString>&&);

    template <class ...Args>
    void insert_menu(int, Args&&...);

    QMenu*& menu_at(int i) { return m_menus[i]; }
    QVector<QMenu*>& menus() { return m_menus; }

    void set_shortcut(int top_menu_i, int action_i, const QString& _key)
         { action(top_menu_i, action_i)->setShortcut(QKeySequence(_key)); }
    void set_shortcuts(int, initializer_list<QString>&&);

  public slots:
    void alert() { QMessageBox::information(0, "Alert!", "End Of Time Is near, So Prepare!"); }

  private:
    QVector<QMenu*> m_menus = {};
    QList<QAction*> m_actions = {};
};

TheMenu::TheMenu()
{
  //
  //
}

/** Initialize a container menus with _no_of_top_menus of menus */
TheMenu::TheMenu(int _no_of_top_menus, initializer_list<QString>&& _title)
            : m_menus(init_menus(_no_of_top_menus, _title))
{
  //test2();
}

void TheMenu::add_action(int _menu_at, const QString& _text)
{
  m_menus[_menu_at]->addAction(_text);
}

void TheMenu::add_actions(int _menu_at, initializer_list<QString>&& _texts)
{
  auto b = _texts.begin();
  while (b != _texts.end())
    m_menus[_menu_at]->addAction(*b++);
}

template <class ...Args>
void TheMenu::add_menu(Args&&... args)
{
  m_menus.append(new QMenu(args...));
}

void TheMenu::set_shortcuts(int i, initializer_list<QString>&& _key)
{
  int j = 0;
  auto b = _key.begin();
  while (b != _key.end())
  {
    action(i, j++)->setShortcut(QKeySequence(*b++));
  }
}

void TheMenu::test2()
{
  add_action(0, "New Test 2");
  add_actions(0, {"New", "Open"});

  connect(m_menus[0]->actions()[0], &QAction::triggered, [&]() { alert();} );
}

#endif//GENERIC_MENU_H
