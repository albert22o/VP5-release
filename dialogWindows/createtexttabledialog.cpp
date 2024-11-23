#include "createtexttabledialog.h"
#include "ui_createtexttabledialog.h"

CreateTextTableDialog::CreateTextTableDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::CreateTextTableDialog)
{
    ui->setupUi(this);

    setFixedHeight(150);
    setFixedWidth(300);

    SetupWidgets();
}

void CreateTextTableDialog::SetupWidgets(){
    SetupSpinboxes();
    ConnectSlotsWithSignals();
}

void CreateTextTableDialog::SetupSpinboxes(){

    size_t minTableColumns = 1;
    size_t maxTableColumns = 100;

    size_t minTableRows = 1;
    size_t maxTableRows = 100;

    size_t minIndentValue = 1;
    size_t maxIndentValue = 100;

    ui->columnsSpinBox->setRange(minTableColumns, maxTableColumns);
    ui->rowsSpinBox->setRange(minTableRows, maxTableRows);
    ui->indentSpinBox->setRange(minIndentValue, maxIndentValue);
}

void CreateTextTableDialog::ConnectSlotsWithSignals(){
    connect(ui->createTable, &QPushButton::clicked, this, &CreateTextTableDialog::OnCreateNewTableButtonClicked);
}

void CreateTextTableDialog::OnCreateNewTableButtonClicked(){

    auto table = CreateTableWidget();
    emit this->NewTableCreated(table);

    this->deleteLater();
    this->close();
}

QTableWidget* CreateTextTableDialog::CreateTableWidget(){
    auto rows = ui->rowsSpinBox->value();
    auto columns = ui->columnsSpinBox->value();
    auto indent = ui->indentSpinBox->value();

    auto table = new QTableWidget(rows, columns);
    table->setStyleSheet(QString("QTableWidget::item { padding: %1px; }").arg(indent));

    return table;
}

CreateTextTableDialog::~CreateTextTableDialog()
{
    delete ui;
}
