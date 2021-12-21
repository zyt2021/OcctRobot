#ifndef MENUTOOLBAR_H
#define MENUTOOLBAR_H

#include <QToolBar>

class MenuToolBar : public QToolBar
{
    Q_OBJECT
public:
    static void Install(QMainWindow *parent);
    static MenuToolBar* Instance();

private:
    explicit MenuToolBar(QWidget *parent = nullptr);
    class CGabor
    {
    public:
        ~CGabor(){
            if (MenuToolBar::ptrSelf){
                delete MenuToolBar::ptrSelf;
                MenuToolBar::ptrSelf = nullptr;
            }
        }
    };
    static void initStyle();
    static void dummySpacing();

    static MenuToolBar* ptrSelf;

signals:
    void requestImport();
    void requestOccView();
    void requestCtrlCenter(bool show);
    void requestTest();
};

#endif // MENUTOOLBAR_H
