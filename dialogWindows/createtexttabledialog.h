#ifndef CREATETEXTTABLEDIALOG_H
#define CREATETEXTTABLEDIALOG_H

#include <QDialog>
#include <QTableWidget>

namespace Ui {
class CreateTextTableDialog;
}

class CreateTextTableDialog : public QDialog
{
    Q_OBJECT

public:
    explicit CreateTextTableDialog(QWidget *parent = nullptr);
    ~CreateTextTableDialog();

private:
    void ConnectSlotsWithSignals();
    void SetupWidgets();
    void SetupSpinboxes();

    QTableWidget* CreateTableWidget();
    Ui::CreateTextTableDialog *ui;
signals:
    void NewTableCreated(QTableWidget* newTable);
private slots:
    void OnCreateNewTableButtonClicked();
};

#endif // CREATETEXTTABLEDIALOG_H
