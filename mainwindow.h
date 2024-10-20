#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTextEdit>
#include <QTemporaryFile>

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_newFile_triggered();
    void on_saveTxtFile_triggered();
    void on_openExisting_triggered();

    void on_clean_triggered();

private:
    void Setup();
    void SetupTabWidget();
    void CreateNewTxtFile();
    void SaveTxtFile();
    QString GenerateNameForNewFile();

    void CloseTab(int tabIndex);

    void CloseTextEditTab(QTextEdit* textEdit, int tabIndex);
    void SaveOrJustCloseFile(QWidget* widget, int tabIndex);

    bool IsFileExists(QString filePath);
    void SaveWithoutDialogWindow(QString filePath, QTextEdit* textEdit);
    void SaveWithDialogWindow(QString filePath, QTextEdit* textEdit);

    void SaveTextEditSettings(const QString& filePath);
    void LoadTextSettings(const QString& filePath);
    void OpenExistingFile();
    int currentTabIndex;
    static QTemporaryFile tempFile;
    Ui::MainWindow *ui;
};
#endif // MAINWINDOW_H
