#include "graphicsview.h"
#include "graphicEditor/graphicedit.h"

GraphicsView::GraphicsView(QGraphicsScene *scene, QWidget *parent) : QGraphicsView(scene, parent),
    currentColor(Qt::black),
    isDrawing(false),
    isMovingShape(false)
{
    setRenderHint(QPainter::Antialiasing);
    setRenderHint(QPainter::SmoothPixmapTransform);
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
    setDragMode(QGraphicsView::NoDrag);
}

GraphicsView::~GraphicsView()
{
}

void GraphicsView::resizeEvent(QResizeEvent *event){

    QGraphicsView::resizeEvent(event);
    emit resized();
}

void GraphicsView::setPen(const QPen &pen)
{
    currentPen = pen;
}

void GraphicsView::setEraserMode(bool mode)
{
    isEraserMode = mode;
}


void GraphicsView::scrollContentsBy(int dx, int dy)
{
    QGraphicsView::scrollContentsBy(dx, dy);
    emit viewportChanged();
}

void GraphicsView::mousePressEvent(QMouseEvent *event)
{
    QGraphicsItem *selectedItem = scene()->itemAt(mapToScene(event->pos()), QTransform());

    if(selectedItem){
        QGraphicsItemGroup *itemGroup = dynamic_cast<QGraphicsItemGroup *>(selectedItem->topLevelItem());
        if (selectedItem->flags().testFlag(QGraphicsItem::ItemIsMovable) || (itemGroup && itemGroup->flags().testFlag(QGraphicsItem::ItemIsMovable))) {
            isMovingShape = true;
            selectedItem->setZValue(1);
            lastMousePos = event->pos();
        }
    }
    else if (event->button() == Qt::LeftButton && viewport()->rect().adjusted(
                                                                        10, 10,
                                                                        -10, -10).contains(event->pos()))
    {
        isDrawing = true;
        isMovingShape = false;
        lastPoint = mapToScene(event->pos()).toPoint();
    }
    else if (isEraserMode && event->button() == Qt::LeftButton)
    {
        isDrawing = true;
        lastPoint = mapToScene(event->pos()).toPoint();
    }

    QGraphicsView::mousePressEvent(event);
}

void GraphicsView::mouseMoveEvent(QMouseEvent *event)
{
    if (isEraserMode && isDrawing) {
        QPoint currentPoint = mapToScene(event->pos()).toPoint();
        QPainterPath eraserPath;
        eraserPath.addEllipse(currentPoint, currentPen.width() / 2, currentPen.width() / 2);

        QList<QGraphicsItem*> itemsToErase = scene()->items(eraserPath);

        for (QGraphicsItem* item : itemsToErase) {
            bool isUserCreated = item->data(0) == "user";

            if (isUserCreated) {
                scene()->removeItem(item);
                delete item;
            }
        }

        lastPoint = currentPoint;
    }
    else if (isDrawing && !isMovingShape)
    {
        if (!viewport()->rect().adjusted(
                                   10, 10,
                                   -10, -10).contains(event->pos()))
        {
            isDrawing = false;
            return;
        }

        QPoint currentPoint = mapToScene(event->pos()).toPoint();
        QPainterPath path;
        path.moveTo(lastPoint);
        path.lineTo(currentPoint);

        QGraphicsPathItem *line = scene()->addPath(path, currentPen);
        line->setZValue(1);
        line->setData(0, "user");

        lastPoint = currentPoint;
    }
    else if (isMovingShape) {
        QGraphicsItem *selectedItem = scene()->itemAt(mapToScene(event->pos()), QTransform());

        if (selectedItem) {

            QGraphicsItemGroup *itemGroup = dynamic_cast<QGraphicsItemGroup *>(selectedItem->topLevelItem());
            if (itemGroup) {

                QPointF currentPos = itemGroup->pos();
                QPointF delta = mapToScene(event->pos()) - mapToScene(lastMousePos);

                GraphicEdit* editor = qobject_cast<GraphicEdit*>(parent());
                if (editor) {
                    if (itemGroup->collidesWithItem(editor->getTopWall()) ||
                        itemGroup->collidesWithItem(editor->getBottomWall()) ||
                        itemGroup->collidesWithItem(editor->getLeftWall()) ||
                        itemGroup->collidesWithItem(editor->getRightWall())) {
                        return;
                    }
                }

                itemGroup->setPos(currentPos + delta);
            }
            else
            {
                QPointF currentPos = selectedItem->pos();
                QPointF delta = mapToScene(event->pos()) - mapToScene(lastMousePos);

                GraphicEdit* editor = qobject_cast<GraphicEdit*>(parent());

                if (editor) {
                    if (selectedItem->collidesWithItem(editor->getTopWall()) ||
                        selectedItem->collidesWithItem(editor->getBottomWall()) ||
                        selectedItem->collidesWithItem(editor->getLeftWall()) ||
                        selectedItem->collidesWithItem(editor->getRightWall())) {
                    }
                }

                selectedItem->setPos(currentPos + delta);
            }
        }
    }

    lastMousePos = event->pos();
    QGraphicsView::mouseMoveEvent(event);
}

void GraphicsView::mouseReleaseEvent(QMouseEvent *event)
{

    isDrawing = false;
    isMovingShape = false;
    QGraphicsView::mouseReleaseEvent(event);
}

