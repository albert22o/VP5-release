#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QMessageBox>

#include <QFile>
#include <QFileInfo>
#include <QDir>
#include <QFileDialog>

#include <QJsonObject>
#include <QJsonDocument>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    Setup();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::Setup(){
    SetupTabWidget();
}

void MainWindow::SetupTabWidget(){
    ui->tabWidget->setTabsClosable(true);
    connect(ui->tabWidget, &QTabWidget::tabCloseRequested, this, &MainWindow::CloseTab);
}

void MainWindow::CloseTab(int tabIndex){

    auto widget = ui->tabWidget->widget(tabIndex);

    if(widget != nullptr){

        auto textEdit = qobject_cast<QTextEdit*>(widget);

        if(textEdit != nullptr && !textEdit->document()->isModified()){
            CloseTextEditTab(textEdit, tabIndex);
        }
        else{
            SaveOrJustCloseFile(widget, tabIndex);
        }
    }
}

void MainWindow::SaveOrJustCloseFile(QWidget* widget, int tabIndex){

    auto reply = QMessageBox::question
    (
        this,
        "Закрыть файл",
        "Файл имеет не сохраненные изменения, хотите сохранить его перед закрытием?",
        QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel
    );

    if(reply == QMessageBox::Yes){
        SaveTxtFile();
    }

    ui->tabWidget->removeTab(tabIndex);
    delete widget;
}

void MainWindow::CloseTextEditTab(QTextEdit* textEdit, int tabIndex){
    ui->tabWidget->removeTab(tabIndex);
    delete textEdit;
}

bool IsWidgetTextEdit(QWidget* widget);

void MainWindow::CreateNewTxtFile()
{
    QTextEdit *newEdit = new QTextEdit(this);

    currentTabIndex = ui->tabWidget->addTab(newEdit, GenerateNameForNewFile());

    ui->tabWidget->setCurrentIndex(currentTabIndex);
    QTextCharFormat format;
    format.setBackground(Qt::white);
    newEdit->setCurrentCharFormat(format);
    newEdit->document()->setModified(false);
}

QString MainWindow::GenerateNameForNewFile()
{

    if(ui->tabWidget->count() == 0)
    {
        return QString("Новый файл");
    }

    return QString("Новый файл")
        .append(" ").
        append(QString::number(ui->tabWidget->count()));

}

void MainWindow::on_newFile_triggered()
{
    CreateNewTxtFile();
}

void MainWindow::SaveTxtFile(){

    auto currentWidget = ui->tabWidget->currentWidget();

    if (!currentWidget) {
        QMessageBox::warning(this, tr("Ошибка сохранения файла"), tr("Нет открытой вкладки для сохранения"));
        return;
    }

    auto textEdit = qobject_cast<QTextEdit*>(currentWidget);
    auto filePath = ui->tabWidget->tabToolTip(ui->tabWidget->currentIndex());

    if(textEdit != nullptr){

        if(IsFileExists(filePath)){
            SaveWithoutDialogWindow(filePath, textEdit);
        }
        else{
            SaveWithDialogWindow(filePath, textEdit);
        }
    }

    textEdit->document()->setModified(false);
}

bool MainWindow::IsFileExists(QString filePath){

    if(filePath.isEmpty()){
        return false;
    }

    return true;
}

void MainWindow::SaveWithoutDialogWindow(QString filePath, QTextEdit* textEdit){

    QFile file(filePath);

    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QMessageBox::warning(nullptr, "Ошибка сохранения файла", "Не удалось сохранить текстовый файл");
        return;
    }

    QTextStream out(&file);
    out << textEdit->toPlainText();
    file.close();

    SaveTextEditSettings(filePath);
}

void MainWindow::SaveWithDialogWindow(QString filePath, QTextEdit* textEdit){

    filePath = QFileDialog::getSaveFileName(this, tr("Сохранить файл"), "", tr("Text Files (*.txt);;All Files (*)"));

    if(!filePath.isEmpty()){

        QFile file(filePath);

        if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
            QMessageBox::warning(nullptr, "Ошибка", "Не удалось сохранить файл");
            return;
        }

        QTextStream out(&file);
        out << textEdit->toPlainText();
        file.close();
        SaveTextEditSettings(filePath);

        ui->tabWidget->setTabToolTip(ui->tabWidget->currentIndex(), filePath);
        ui->tabWidget->setTabText(ui->tabWidget->currentIndex(), QFileInfo(filePath).fileName());
    }
}

void MainWindow::SaveTextEditSettings(const QString& filePath){

    auto textEdit = qobject_cast<QTextEdit*>(ui->tabWidget->currentWidget());

    if(textEdit != nullptr){

        QFileInfo fileInfo(filePath);
        QString relativePath = "../SimpleWord/settings";
        QDir settingsDir(relativePath);

        if (!settingsDir.exists() && !settingsDir.mkpath(".")) {
            throw std::runtime_error("Невозможно создать папку, SaveTextEditSettings()");
        }

        QString settingsFilePath = settingsDir.absoluteFilePath(fileInfo.fileName() + ".html");
        auto documentHtml = textEdit->toHtml();

        QJsonObject settingsObject;
        settingsObject["html"] = documentHtml;

        QJsonDocument settingsDocument(settingsObject);
        QFile settingsFile(settingsFilePath);

        if (settingsFile.open(QIODevice::WriteOnly)) {

            settingsFile.write(settingsDocument.toJson());
            settingsFile.close();
        }
        else {
            throw std::runtime_error(
                QString("Невозможно сохранить файл по пути: ").append(settingsFilePath).toStdString());
        }
    }
}

void MainWindow::on_saveTxtFile_triggered()
{
    SaveTxtFile();
}

void MainWindow::OpenExistingFile(){

    auto fileName = QFileDialog::getOpenFileName
        (this, tr("Открыть файл"), "", tr("Text Files (*.txt);;Table Files(*.csv);;All Files (*)"));

    if (!fileName.isEmpty()) {

        QFile file(fileName);

        if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {

            QMessageBox::warning(nullptr, QObject::tr("Ошибка открытия файла"), QObject::tr("Не удалось открыть файл"));
            return;
        }

        QTextStream in(&file);
        QString fileContent = in.readAll();
        file.close();

        auto textEdit = new QTextEdit();
        textEdit->setText(fileContent);

        int pageIndex = ui->tabWidget->addTab(textEdit, QFileInfo(fileName).fileName());
        ui->tabWidget->setCurrentIndex(pageIndex);
        currentTabIndex = ui->tabWidget->currentIndex();

        LoadTextSettings(fileName);
        textEdit->document()->setModified(false);
    }
}

void MainWindow::LoadTextSettings(const QString& filePath) {

    auto textEdit = qobject_cast<QTextEdit*>(ui->tabWidget->currentWidget());

    if(textEdit != nullptr){

        QFileInfo fileInfo(filePath);
        QString relativePath = "../SimpleWord/settings";
        QDir settingsDir(relativePath);
        QString settingsFilePath = settingsDir.absoluteFilePath(fileInfo.fileName() + ".html");

        QFile settingsFile(settingsFilePath);

        if (!settingsFile.open(QIODevice::ReadOnly)) {
            throw std::runtime_error("Невозможно прочитать файл с настройками. LoadTextSettings()");
        }

        QByteArray settings = settingsFile.readAll();
        settingsFile.close();

        QJsonDocument loadDoc(QJsonDocument::fromJson(settings));
        QJsonObject settingsObject = loadDoc.object();

        QString documentHtml = settingsObject["html"].toString();

        textEdit->clear();
        textEdit->setHtml(documentHtml);
    }
}

void MainWindow::on_openExisting_triggered()
{
    OpenExistingFile();
}
