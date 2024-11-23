#ifndef CREATETEXTTABLEDIALOG_H
#define CREATETEXTTABLEDIALOG_H

#include <QDialog>
#include <QTextEdit>

namespace Ui {
class CreateTextTableDialog;
}

class CreateTextTableDialog : public QDialog
{
    Q_OBJECT

public:
    explicit CreateTextTableDialog(QTextEdit* textEdit, QWidget *parent = nullptr);
    ~CreateTextTableDialog();

private:
    QTextEdit* textEdit;

    void ConnectSlotsWithSignals();
    void SetupWidgets();
    void SetupSpinboxes();

    QString CreateHtmlTable();
    Ui::CreateTextTableDialog *ui;
signals:
    void NewTableCreated(QString newHtmlTable, QTextEdit* textEdit);
private slots:
    void OnCreateNewTableButtonClicked();
};

#endif // CREATETEXTTABLEDIALOG_H
