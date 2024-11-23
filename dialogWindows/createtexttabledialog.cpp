#include "createtexttabledialog.h"
#include "ui_createtexttabledialog.h"

CreateTextTableDialog::CreateTextTableDialog(QTextEdit* textEdit, QWidget *parent)
    : QDialog(parent),
    textEdit(textEdit)
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

    auto table = CreateHtmlTable();
    emit this->NewTableCreated(table, textEdit);

    this->deleteLater();
    this->close();
}

QString CreateTextTableDialog::CreateHtmlTable()
{
    auto rows = ui->rowsSpinBox->value();
    auto columns = ui->columnsSpinBox->value();
    auto indent = ui->indentSpinBox->value();

    QString htmlTable = QString("<table border='1' cellspacing='%1' cellpadding='4'>").arg(indent);

    for (int row = 0; row < rows; ++row)
    {
        htmlTable += "<tr>";

        for (int col = 0; col < columns; ++col)
        {
            htmlTable += "<td></td>";
        }

        htmlTable += "</tr>";
    }

    htmlTable += "</table>";

    return htmlTable;
}

CreateTextTableDialog::~CreateTextTableDialog()
{
    delete ui;
}
