#ifndef MENUDIALOG_H
#define MENUDIALOG_H

#include <QDialog>
#include "mainwindow.h"

namespace Ui {
class MenuDialog;
}

class MenuDialog : public QDialog
{
    Q_OBJECT

public:
    explicit MenuDialog(QWidget *parent = nullptr);
    ~MenuDialog();

private slots:
    void on_goGraphicEdit_clicked();

    void on_goTextEdit_clicked();

    void on_exit_clicked();

    void on_back_to_menu();

private:
    void RunInGraphicEditorMode();
    void RunInTextEditorMode();
    void Exit();

    Ui::MenuDialog *ui;

    MainWindow* textEditor;
};

#endif // MENUDIALOG_H
