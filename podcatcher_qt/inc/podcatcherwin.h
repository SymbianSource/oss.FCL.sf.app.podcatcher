#ifndef PODCATCHERWIN_H
#define PODCATCHERWIN_H

#include <QMainWindow>
#include <QModelIndex>
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

private slots:
    void on_Settings_clicked();
    void on_Update_clicked();
    void on_Add_clicked();    
    void on_listView_clicked(QModelIndex index);
    void on_listView_customContextMenuRequested(QPoint pos);
};

#endif // PODCATCHERWIN_H
