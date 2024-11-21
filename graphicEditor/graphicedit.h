#ifndef GRAPHICEDIT_H
#define GRAPHICEDIT_H

#include <QMainWindow>
#include <QGraphicsPixmapItem>

#include "graphicEditor/graphicsview.h"

namespace Ui {
class GraphicEdit;
}

class GraphicEdit : public QMainWindow
{
    Q_OBJECT

protected:
    void closeEvent(QCloseEvent *event);

public:
    explicit GraphicEdit(QWidget *parent = nullptr);
    ~GraphicEdit();

    QGraphicsPixmapItem* getTopWall() const { return topWall; }
    QGraphicsPixmapItem* getBottomWall() const { return bottomWall; }
    QGraphicsPixmapItem* getLeftWall() const { return leftWall; }
    QGraphicsPixmapItem* getRightWall() const { return rightWall; }

    QList<QGraphicsItemGroup*> getMovingItemGroups() const { return movingItemGroups; }


signals:
    void onGoToMenuEmitted();

private slots:
    void on_Eraser_triggered();

    void on_Feather_triggered();

private:
    Ui::GraphicEdit *ui;

    GraphicsView *view;
    QGraphicsScene *scene;
    QPen currentPen;

    QGraphicsPixmapItem *topWall;
    QGraphicsPixmapItem *bottomWall;
    QGraphicsPixmapItem *leftWall;
    QGraphicsPixmapItem *rightWall;

    QList<QGraphicsItemGroup*> movingItemGroups;

    void InitializeGraphicsView();
    void setupWalls();
    void updateWallPositions();
};

#endif // GRAPHICEDIT_H
