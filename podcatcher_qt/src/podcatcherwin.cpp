#include "podcatcherwin.h"
#include "ui/ui_podcatcherwin.h"
#include <QModelIndex>

PodcatcherWin::PodcatcherWin(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::PodcatcherWin)
{
    ui->setupUi(this); 
}

PodcatcherWin::~PodcatcherWin()
{
    delete ui;
}

void PodcatcherWin::on_listView_customContextMenuRequested(QPoint pos)
{

}

void PodcatcherWin::on_listView_clicked(QModelIndex index)
{

}


void PodcatcherWin::on_Add_clicked()
{

}

void PodcatcherWin::on_Update_clicked()
{

}

void PodcatcherWin::on_Settings_clicked()
{

}
