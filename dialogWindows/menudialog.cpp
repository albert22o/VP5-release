#include "menudialog.h"
#include "ui_menudialog.h"

MenuDialog::MenuDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::MenuDialog)
{
    ui->setupUi(this);
}

MenuDialog::~MenuDialog()
{
    delete ui;
}

void MenuDialog::on_goGraphicEdit_clicked()
{
    RunInGraphicEditorMode();
}

void MenuDialog::RunInGraphicEditorMode(){

}

void MenuDialog::on_goTextEdit_clicked()
{
    RunInTextEditorMode();
}

void MenuDialog::RunInTextEditorMode(){

    textEditor = new MainWindow(this);
    textEditor->show();

    QObject::connect(textEditor, &MainWindow::onGoToMenuEmitted, this, &MenuDialog::on_back_to_menu);

    hide();
}

void MenuDialog::on_back_to_menu(){

    show();
}

void MenuDialog::on_exit_clicked()
{
    Exit();
}

void MenuDialog::Exit(){

    QApplication::closeAllWindows();
    QApplication::exit();
}

