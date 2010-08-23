#ifndef PODCATCHERWIN_H
#define PODCATCHERWIN_H

#include <QMainWindow>

namespace Ui {
    class PodcatcherWin;
}

class PodcatcherWin : public QMainWindow
{
    Q_OBJECT

public:
    explicit PodcatcherWin(QWidget *parent = 0);
    ~PodcatcherWin();

private:
    Ui::PodcatcherWin *ui;
};

#endif // PODCATCHERWIN_H
