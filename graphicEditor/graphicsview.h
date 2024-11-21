#ifndef GRAPHICSVIEW_H
#define GRAPHICSVIEW_H

#include <QGraphicsView>
#include <QMouseEvent>
#include <QPainter>
#include <QGraphicsScene>
#include <QPainterPath>
#include <QPen>
#include <QScrollBar>
#include <QGraphicsItem>

class GraphicsView : public QGraphicsView
{
    Q_OBJECT
public:
    explicit GraphicsView(QGraphicsScene *scene, QWidget *parent = nullptr);
    ~GraphicsView() override;
    void setPen(const QPen &pen);
    void setEraserMode(bool mode);

signals:
    void resized();
    void viewportChanged();

protected:
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;

    void scrollContentsBy(int dx, int dy) override;
    bool isWithinBounds(QGraphicsItem* item, QPointF newPos);

private:
    QPoint lastPoint;
    QColor currentColor;
    bool isDrawing;
    bool isMovingShape;
    QPen currentPen;
    QPoint lastMousePos;
    bool isEraserMode = false;
};

#endif // GRAPHICSVIEW_H
