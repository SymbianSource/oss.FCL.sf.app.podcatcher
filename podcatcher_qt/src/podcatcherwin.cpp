#include "podcatcherwin.h"
#include "ui_podcatcherwin.h"

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
