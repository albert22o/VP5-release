#include "graphicedit.h"
#include "ui_graphicedit.h"

GraphicEdit::GraphicEdit(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::GraphicEdit)
{
    ui->setupUi(this);
}

GraphicEdit::~GraphicEdit()
{
    delete ui;
}
