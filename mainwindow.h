#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTextEdit>
#include <QTemporaryFile>
#include <QLineEdit>
#include <QCheckBox>
#include <QLabel>
#include <QPushButton>
#include <QRegularExpression>
#include <QColorDialog>
#include <QFontDialog>
#include <QTableWidget>

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

signals:
    void onGoToMenuEmitted();

protected:
    void closeEvent(QCloseEvent *event) override;

private slots:
    void on_newFile_triggered();
    void on_saveTxtFile_triggered();
    void on_openExisting_triggered();

    void on_clean_triggered();

    void on_search_triggered();

    void on_undo_triggered();

    void on_redo_triggered();

    void on_color_triggered();

    void on_font_triggered();

    void on_newTable_triggered();
    void on_NewTableCreted(QString table, QTextEdit* textEdit);

    void onCurrentTabChanged(int index);

    void SetTableActionsPannelVisible(bool flag);

    void on_newRow_clicked();

    void on_deleteRow_clicked();

    void on_newColum_clicked();

    void on_deleteColumn_clicked();

    void on_Copy_triggered();

    void on_Paste_triggered();

    void on_Cut_triggered();

private:
    void Setup();
    void SetupTabWidget();
    void CreateNewTxtFile();
    void SaveTxtFile();

    QString GenerateNameForNewFile();

    void CloseTab(int tabIndex);

    void CloseTextEditTab(QTextEdit* textEdit, int tabIndex);
    void CloseTableWidgetTab(QTableWidget* tableWidget, int tabIndex);

    void SaveOrJustCloseFile(QWidget* widget, int tabIndex);

    bool IsFileExists(QString filePath);

    void SaveWithoutDialogWindow(QString filePath, QTextEdit* textEdit);
    void SaveWithDialogWindow(QString filePath, QTextEdit* textEdit);

    void SaveTextEditSettings(const QString& filePath);
    void LoadTextSettings(const QString& filePath);
    void OpenExistingFile();
    int currentTabIndex;
    QFont currentFont;
    static QTemporaryFile tempFile;
    Ui::MainWindow *ui;
    QTextEdit *editor;
};
#endif // MAINWINDOW_H
