#ifndef GRAPHICEDIT_H
#define GRAPHICEDIT_H

#include <QMainWindow>

namespace Ui {
class GraphicEdit;
}

class GraphicEdit : public QMainWindow
{
    Q_OBJECT

public:
    explicit GraphicEdit(QWidget *parent = nullptr);
    ~GraphicEdit();

private:
    Ui::GraphicEdit *ui;
};

#endif // GRAPHICEDIT_H
