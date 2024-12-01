#ifndef GRAPHICEDIT_H
#define GRAPHICEDIT_H

#include <QMainWindow>
#include <QGraphicsPixmapItem>
//#include <QSound>

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

    void on_Figure_triggered();

    void on_SaveScene_triggered();

    void on_AddImage_triggered();

    void on_DeleteFigure_triggered();

private:
    Ui::GraphicEdit *ui;
    void stopMovingObjects();
    GraphicsView *view;
    QGraphicsScene *scene;
    QPen currentPen;

    QGraphicsPixmapItem *topWall;
    QGraphicsPixmapItem *bottomWall;
    QGraphicsPixmapItem *leftWall;
    QGraphicsPixmapItem *rightWall;


    QList<QGraphicsItemGroup*> movingItemGroups;
    void addShape(const QString &shapeType, const QRectF &rect, const QColor &fillColor, Qt::BrushStyle brushStyle, const QColor &strokeColor, int strokeWidth);
    Qt::BrushStyle stringToBrushStyle(const QString &styleStr);
    QList<QPointF> velocities;
    void InitializeGraphicsView();
    void setupWalls();
    void moveObject();
    void updateWallPositions();
    void resumeMovingObjects();
    void groupSetFlags(QGraphicsItemGroup *group);

    void drawOganesyan();
    void drawRakov(int startX, int startY);

    void textSetFlags(QGraphicsTextItem *item);

    void createMovingObject_Flower();
    void createMovingObject_Car(int startX, int startY);

    QList<bool> movingStates;
    //QSound collisionSound;
};

#endif // GRAPHICEDIT_H
