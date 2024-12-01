#include "graphicedit.h"
#include "ui_graphicedit.h"

#include <QMessageBox>
#include <QPushButton>
#include <QCloseEvent>
#include <QSpinBox>
#include <QVBoxLayout>
#include <QLabel>
#include <QComboBox>
#include <QFormLayout>
#include <QColorDialog>
#include <QFileDialog>
#include <QTimer>


GraphicEdit::GraphicEdit(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::GraphicEdit), topWall(nullptr), bottomWall(nullptr), leftWall(nullptr), rightWall(nullptr)
{
    ui->setupUi(this);

    InitializeGraphicsView();
}


void GraphicEdit::InitializeGraphicsView(){

    scene = new QGraphicsScene(this);
    view = new GraphicsView(scene, this);
    setCentralWidget(view);
    view->setRenderHint(QPainter::Antialiasing);
    view->setRenderHint(QPainter::SmoothPixmapTransform);
    view->setAlignment(Qt::AlignTop | Qt::AlignLeft);
    view->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
    view->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);

    connect(view, &GraphicsView::viewportChanged, this, &GraphicEdit::updateWallPositions);
    connect(view, &GraphicsView::resized, this, &GraphicEdit::setupWalls);

    setupWalls();
    drawOganesyan();
    drawRakov(30,100);
    drawTashlykov(30,250);
    createMovingObject_Flower();
    createMovingObject_Car(200,200);
    createMovingObject_Phone(300,300);

    QTimer *moveTimer;
    moveTimer = new QTimer(this);
    connect(moveTimer, &QTimer::timeout, this, &GraphicEdit::moveObject);
    moveTimer->start(30); // Интервал обновления, например, 30 мс
}

void GraphicEdit::moveObject()
{
    int wallThickness = 10;

    for (int i = 0; i < movingItemGroups.size(); ++i) {
        if (movingStates[i]) {
            QGraphicsItemGroup *itemGroup = movingItemGroups[i];
            QPointF velocity = velocities[i];
            QPointF newPos = itemGroup->pos() + velocity;

            QRectF boundingRect = itemGroup->boundingRect();
            qreal left = newPos.x();
            qreal right = newPos.x() + boundingRect.width();
            qreal top = newPos.y();
            qreal bottom = newPos.y() + boundingRect.height();

            if (left <= wallThickness || right >= view->viewport()->width() - 2 * wallThickness) {
                velocity.setX(-velocity.x());
                //collisionSound.play();
            }

            if (top <= wallThickness || bottom >= view->viewport()->height() - 2 * wallThickness) {
                velocity.setY(-velocity.y());
                //collisionSound.play();
            }

            bool collisionDetected = false;
            QList<QGraphicsItem *> itemsAtNewPos = scene->items(QRectF(newPos, boundingRect.size()));

            for (QGraphicsItem *otherItem : itemsAtNewPos) {
                if (otherItem != itemGroup && otherItem->data(0) != "user" && otherItem->data(0) != "image") {
                    QRectF otherBoundingRect = otherItem->boundingRect().translated(otherItem->pos());
                    qreal otherLeft = otherBoundingRect.x();
                    qreal otherRight = otherBoundingRect.x() + otherBoundingRect.width();
                    qreal otherTop = otherBoundingRect.y();
                    qreal otherBottom = otherBoundingRect.y() + otherBoundingRect.height();

                    if (right > otherLeft && left < otherRight && bottom > otherTop && top < otherBottom) {
                        collisionDetected = true;

                        if (velocity.x() > 0) {
                            velocity.setX(-velocity.x());
                        } else if (velocity.x() < 0) {
                            velocity.setX(-velocity.x());
                        }

                        if (velocity.y() > 0) {
                            velocity.setY(-velocity.y());
                        } else if (velocity.y() < 0) {
                            velocity.setY(-velocity.y());
                        }

                        //collisionSound.play();
                        break;
                    }
                }
            }

            if (!collisionDetected) {
                itemGroup->setPos(newPos);
            }

            velocities[i] = velocity;
        }
    }
}


void GraphicEdit::setupWalls() {
    // Получаем размеры области просмотра
    int viewWidth = view->viewport()->width();
    int viewHeight = view->viewport()->height();
    int wallThickness = 10;

    // Загружаем изображение стены
    QPixmap wallImage(":/R.jfif");
    if (wallImage.isNull()) {
        return;
    }

    // Масштабируем изображение для горизонтальных и вертикальных стен
    QPixmap scaledTopBottom = wallImage.scaled(viewWidth, wallThickness);
    QPixmap scaledLeftRight = wallImage.scaled(wallThickness, viewHeight);

    // Функция для создания стены, если она не существует
    auto createOrUpdateWall = [this](QGraphicsPixmapItem*& wall, const QPixmap& pixmap) {
        if (!wall) {
            wall = scene->addPixmap(pixmap);
            wall->setFlag(QGraphicsItem::ItemIsMovable, false); // Делаем стену неподвижной
        } else {
            wall->setPixmap(pixmap); // Обновляем изображение стены
        }
    };

    // Создаём или обновляем стены
    createOrUpdateWall(topWall, scaledTopBottom);
    createOrUpdateWall(bottomWall, scaledTopBottom);
    createOrUpdateWall(leftWall, scaledLeftRight);
    createOrUpdateWall(rightWall, scaledLeftRight);

    // Обновляем позиции стен
    updateWallPositions();
}


void GraphicEdit::updateWallPositions(){
    int wallThickness = 10;

    int horizontalOffset = view->horizontalScrollBar()->value();
    int verticalOffset = view->verticalScrollBar()->value();

    if (topWall)
        topWall->setPos(horizontalOffset, verticalOffset);
    if (bottomWall)
        bottomWall->setPos(horizontalOffset, view->viewport()->height() - wallThickness + verticalOffset);
    if (leftWall)
        leftWall->setPos(horizontalOffset, verticalOffset);
    if (rightWall)
        rightWall->setPos(view->viewport()->width() - wallThickness + horizontalOffset, verticalOffset);
}


void GraphicEdit::groupSetFlags(QGraphicsItemGroup *group){
    group->setFlag(QGraphicsItem::ItemIsMovable, true);
    group->setFlag(QGraphicsItem::ItemIsSelectable, true);
    group->setFlag(QGraphicsItem::ItemSendsGeometryChanges, true);
}

void setItemFlags(QGraphicsItem* item) {
    item->setFlags(QGraphicsItem::ItemIsSelectable | QGraphicsItem::ItemIsMovable);
}

QGraphicsRectItem* createRect(QGraphicsScene* scene, int x, int y, int width, int height, QPen pen, QBrush brush) {
    QGraphicsRectItem* rect = scene->addRect(x, y, width, height, pen, brush);
    setItemFlags(rect);
    return rect;
}

QGraphicsEllipseItem* createEllipse(QGraphicsScene* scene, int x, int y, int width, int height, QPen pen, QBrush brush) {
    QGraphicsEllipseItem* ellipse = scene->addEllipse(x, y, width, height, pen, brush);
    setItemFlags(ellipse);
    return ellipse;
}

QGraphicsLineItem* createLine(QGraphicsScene* scene, QLineF line, QPen pen) {
    QGraphicsLineItem* item = scene->addLine(line, pen);
    setItemFlags(item);
    return item;
}

QGraphicsTextItem* createText(QGraphicsScene* scene, const QString& text, int x, int y, QFont font, QColor color) {
    QGraphicsTextItem* item = new QGraphicsTextItem(text);
    item->setFont(font);
    item->setDefaultTextColor(color);
    item->setPos(x, y);
    setItemFlags(item);
    scene->addItem(item);
    return item;
}



void GraphicEdit::drawRakov(int startX, int startY) {

    QPen pen(Qt::black, 2); // Толщина линии 2 пикселя, черный цвет
    QBrush brush(Qt::cyan); // Кисть синего цвета
    QFont font("Arial", 24); // Шрифт Arial, размер 24
    int xOffset = startX;
    int yOffset = startY;
    int letterSpacing = 10; // Расстояние между буквами

    // R
    QGraphicsItemGroup* groupR = new QGraphicsItemGroup();
    groupR->addToGroup(createEllipse(scene, xOffset, yOffset, 40, 40, pen, brush));
    groupR->addToGroup(createRect(scene, xOffset -10, yOffset, 10, 70, pen, QBrush(Qt::black)));
    setItemFlags(groupR);
    scene->addItem(groupR);
    xOffset += 50 + letterSpacing;

    // А
    QGraphicsItemGroup* groupA = new QGraphicsItemGroup();
    groupA->addToGroup(createLine(scene, QLineF(xOffset, yOffset + 40, xOffset + 20, yOffset + 10), QPen(Qt::red, 6)));
    groupA->addToGroup(createLine(scene, QLineF(xOffset + 20, yOffset + 10, xOffset + 40, yOffset + 40), QPen(Qt::red, 6)));
    groupA->addToGroup(createLine(scene, QLineF(xOffset + 10, yOffset + 30, xOffset + 30, yOffset + 30), QPen(Qt::red, 6)));
    setItemFlags(groupA);
    scene->addItem(groupA);
    xOffset += 50 + letterSpacing;

    QGraphicsItemGroup *group_K = new QGraphicsItemGroup();

    QGraphicsItem *K_1 = scene->addRect(QRectF(xOffset,yOffset,10,50), QPen(Qt::green, 2), QBrush(Qt::black, Qt::SolidPattern));
    group_K->addToGroup(K_1);
    QGraphicsItem *K_2 = scene->addRect(QRectF(15,15,30,10), QPen(Qt::green, 2), QBrush(Qt::black, Qt::SolidPattern));
    K_2->setTransformOriginPoint(15, 15);
    K_2->setRotation(-45);
    K_2->setPos(xOffset - 10, yOffset + 5);
    group_K->addToGroup(K_2);
    QGraphicsItem *K_3 = scene->addRect(QRectF(15,15,30,10), QPen(Qt::green, 2), QBrush(Qt::black, Qt::SolidPattern));
    K_3->setTransformOriginPoint(15, 15);
    K_3->setRotation(45);
    K_3->setPos(xOffset - 5, yOffset + 5);
    group_K->addToGroup(K_3);
    groupSetFlags(group_K);
    scene->addItem(group_K);

    xOffset += 50 + letterSpacing;

    // О
    QGraphicsItemGroup* groupO = new QGraphicsItemGroup();
    groupO->addToGroup(createEllipse(scene, xOffset, yOffset + 10, 40, 40, QPen(Qt::magenta, 6), QBrush(Qt::white)));
    setItemFlags(groupO);
    scene->addItem(groupO);
    xOffset += 60 + letterSpacing;

    // V
    QGraphicsItemGroup *group_V = new QGraphicsItemGroup();
    QGraphicsItem *V_1 = scene->addLine(QLineF(xOffset, yOffset + 45, xOffset - 20, yOffset), QPen(Qt::blue, 6));
    group_V->addToGroup(V_1);
    QGraphicsItem *V_2 = scene->addLine(QLineF(xOffset, yOffset + 45, xOffset + 20, yOffset), QPen(Qt::blue, 6));
    group_V->addToGroup(V_2);
    groupSetFlags(group_V);
    scene->addItem(group_V);

    xOffset += 30 + letterSpacing;

    createText(scene, "АРТЕМ", xOffset, yOffset + 15, font, Qt::green);
}

void GraphicEdit::textSetFlags(QGraphicsTextItem *item){
    item->setFlag(QGraphicsItem::ItemIsMovable, true);
    item->setFlag(QGraphicsItem::ItemIsSelectable, true);
    item->setFlag(QGraphicsItem::ItemSendsGeometryChanges, true);
}

void GraphicEdit::drawTashlykov(int startX, int startY){

    QPen pen(Qt::black, 2);
    QBrush brush(Qt::cyan);
    QFont font("Arial", 24);
    int xOffset = startX;
    int yOffset = startY;
    int letterSpacing = 10;

    // T
    QGraphicsItemGroup* groupT = new QGraphicsItemGroup();
    groupT->addToGroup(createLine(scene, QLineF(xOffset + 5, yOffset, xOffset + 5, yOffset + 40), QPen(Qt::darkGreen, 4)));
    groupT->addToGroup(createLine(scene, QLineF(xOffset - 10, yOffset, xOffset + 20, yOffset), QPen(Qt::darkGreen, 4)));
    setItemFlags(groupT);
    scene->addItem(groupT);
    xOffset += 20 + letterSpacing;

    // А
    QGraphicsItemGroup* groupA = new QGraphicsItemGroup();
    groupA->addToGroup(createLine(scene, QLineF(xOffset, yOffset + 40, xOffset + 20, yOffset + 10), QPen(Qt::red, 6)));
    groupA->addToGroup(createLine(scene, QLineF(xOffset + 20, yOffset + 10, xOffset + 40, yOffset + 40), QPen(Qt::red, 6)));
    groupA->addToGroup(createLine(scene, QLineF(xOffset + 10, yOffset + 30, xOffset + 30, yOffset + 30), QPen(Qt::red, 6)));
    setItemFlags(groupA);
    scene->addItem(groupA);
    xOffset += 50 + letterSpacing;


    // Ш
    QGraphicsItemGroup* groupSh = new QGraphicsItemGroup();

    groupSh->addToGroup(createLine(scene, QLineF(xOffset, yOffset + 40, xOffset + 20, yOffset + 40), QPen(Qt::blue, 6)));
    groupSh->addToGroup(createLine(scene, QLineF(xOffset, yOffset + 40, xOffset, yOffset), QPen(Qt::blue, 6)));
    groupSh->addToGroup(createLine(scene, QLineF(xOffset + 10, yOffset + 40, xOffset + 10, yOffset), QPen(Qt::blue, 6)));
    groupSh->addToGroup(createLine(scene, QLineF(xOffset + 20, yOffset + 40, xOffset + 20, yOffset), QPen(Qt::blue, 6)));

    setItemFlags(groupSh);
    scene->addItem(groupSh);
    xOffset += 30 + letterSpacing;

    // л
    QGraphicsItemGroup* groupL = new QGraphicsItemGroup();
    groupL->addToGroup(createLine(scene, QLineF(xOffset, yOffset + 40, xOffset + 20, yOffset + 10), QPen(Qt::darkGreen, 6)));
    groupL->addToGroup(createLine(scene, QLineF(xOffset + 20, yOffset + 10, xOffset + 40, yOffset + 40), QPen(Qt::darkGreen, 6)));
    setItemFlags(groupL);
    scene->addItem(groupL);
    xOffset += 50 + letterSpacing;


    // Y
    QGraphicsItemGroup* groupY = new QGraphicsItemGroup();
    groupY->addToGroup(createLine(scene, QLineF(xOffset, yOffset + 40, xOffset, yOffset + 20), QPen(Qt::darkMagenta, 6)));
    groupY->addToGroup(createLine(scene, QLineF(xOffset, yOffset + 20, xOffset + 20, yOffset), QPen(Qt::darkMagenta, 6)));
    groupY->addToGroup(createLine(scene, QLineF(xOffset, yOffset + 20, xOffset - 20, yOffset), QPen(Qt::darkMagenta, 6)));
    setItemFlags(groupY);
    scene->addItem(groupY);
    xOffset += 20 + letterSpacing;


    QGraphicsItemGroup *group_K = new QGraphicsItemGroup();

    QGraphicsItem *K_1 = scene->addRect(QRectF(xOffset,yOffset,10,50), QPen(Qt::green, 2), QBrush(Qt::black, Qt::SolidPattern));
    group_K->addToGroup(K_1);
    QGraphicsItem *K_2 = scene->addRect(QRectF(15,15,30,10), QPen(Qt::green, 2), QBrush(Qt::black, Qt::SolidPattern));
    K_2->setTransformOriginPoint(15, 15);
    K_2->setRotation(-45);
    K_2->setPos(xOffset - 10, yOffset + 5);
    group_K->addToGroup(K_2);
    QGraphicsItem *K_3 = scene->addRect(QRectF(15,15,30,10), QPen(Qt::green, 2), QBrush(Qt::black, Qt::SolidPattern));
    K_3->setTransformOriginPoint(15, 15);
    K_3->setRotation(45);
    K_3->setPos(xOffset - 5, yOffset + 5);
    group_K->addToGroup(K_3);
    groupSetFlags(group_K);
    scene->addItem(group_K);

    xOffset += 30 + letterSpacing;

    // О
    QGraphicsItemGroup* groupO = new QGraphicsItemGroup();
    groupO->addToGroup(createEllipse(scene, xOffset, yOffset + 10, 30, 30, QPen(Qt::magenta, 6), QBrush(Qt::white)));
    setItemFlags(groupO);
    scene->addItem(groupO);
    xOffset += 45 + letterSpacing;


    // V
    QGraphicsItemGroup *group_V = new QGraphicsItemGroup();
    QGraphicsItem *V_1 = scene->addLine(QLineF(xOffset, yOffset + 45, xOffset - 20, yOffset), QPen(Qt::blue, 6));
    group_V->addToGroup(V_1);
    QGraphicsItem *V_2 = scene->addLine(QLineF(xOffset, yOffset + 45, xOffset + 20, yOffset), QPen(Qt::blue, 6));
    group_V->addToGroup(V_2);
    groupSetFlags(group_V);
    scene->addItem(group_V);

    xOffset += 30 + letterSpacing;

    createText(scene, "ПАВЕЛ", xOffset, yOffset + 15, font, Qt::green);
}

void GraphicEdit::drawOganesyan() {
    // О
    QGraphicsItemGroup *group_O = new QGraphicsItemGroup();
    QGraphicsEllipseItem *O_1 = scene->addEllipse(QRectF(10, 15, 50, 50), QPen(Qt::blue, 6), QBrush(Qt::yellow, Qt::SolidPattern));
    group_O->addToGroup(O_1);
    groupSetFlags(group_O);
    scene->addItem(group_O);

    // Г
    QGraphicsItemGroup *group_G = new QGraphicsItemGroup();
    QGraphicsItem *G_1 = scene->addLine(QLineF(70, 15, 120, 15), QPen(Qt::red, 6));
    group_G->addToGroup(G_1);
    QGraphicsItem *G_2 = scene->addLine(QLineF(70, 15, 70, 60), QPen(Qt::red, 6));
    group_G->addToGroup(G_2);
    groupSetFlags(group_G);
    scene->addItem(group_G);

    // А
    QGraphicsItemGroup *group_A = new QGraphicsItemGroup();
    QGraphicsItem *A_1 = scene->addLine(QLineF(130, 60, 150, 15), QPen(Qt::green, 6));
    group_A->addToGroup(A_1);
    QGraphicsItem *A_2 = scene->addLine(QLineF(150, 15, 170, 60), QPen(Qt::green, 6));
    group_A->addToGroup(A_2);
    QGraphicsItem *A_3 = scene->addLine(QLineF(140, 40, 160, 40), QPen(Qt::green, 6));
    group_A->addToGroup(A_3);
    groupSetFlags(group_A);
    scene->addItem(group_A);

    // Н
    QGraphicsItemGroup *group_N = new QGraphicsItemGroup();
    QGraphicsItem *N_1 = scene->addLine(QLineF(180, 15, 180, 60), QPen(Qt::blue, 6));
    group_N->addToGroup(N_1);
    QGraphicsItem *N_2 = scene->addLine(QLineF(180, 15, 200, 60), QPen(Qt::blue, 6));
    group_N->addToGroup(N_2);
    QGraphicsItem *N_3 = scene->addLine(QLineF(200, 15, 200, 60), QPen(Qt::blue, 6));
    group_N->addToGroup(N_3);
    groupSetFlags(group_N);
    scene->addItem(group_N);

    // Е
    QGraphicsItemGroup *group_E = new QGraphicsItemGroup();
    QGraphicsItem *E_1 = scene->addLine(QLineF(210, 15, 210, 60), QPen(Qt::red, 6));
    group_E->addToGroup(E_1);
    QGraphicsItem *E_2 = scene->addLine(QLineF(210, 15, 240, 15), QPen(Qt::red, 6));
    group_E->addToGroup(E_2);
    QGraphicsItem *E_3 = scene->addLine(QLineF(210, 40, 230, 40), QPen(Qt::red, 6));
    group_E->addToGroup(E_3);
    QGraphicsItem *E_4 = scene->addLine(QLineF(210, 60, 240, 60), QPen(Qt::red, 6));
    group_E->addToGroup(E_4);
    groupSetFlags(group_E);
    scene->addItem(group_E);

    // С
    QGraphicsItemGroup *group_C = new QGraphicsItemGroup();
    QGraphicsLineItem *C_1 = scene->addLine(QLineF(250, 15, 280, 15), QPen(Qt::yellow, 6));
    group_C->addToGroup(C_1);
    QGraphicsLineItem *C_2 = scene->addLine(QLineF(250, 15, 250, 60), QPen(Qt::yellow, 6));
    group_C->addToGroup(C_2);
    QGraphicsLineItem *C_3 = scene->addLine(QLineF(250, 60, 280, 60), QPen(Qt::yellow, 6));
    group_C->addToGroup(C_3);
    groupSetFlags(group_C);
    scene->addItem(group_C);

    // Я
    QGraphicsItemGroup *group_YA = new QGraphicsItemGroup();

    // Вертикальная линия (левая часть)
    QGraphicsItem *YA_1 = scene->addLine(QLineF(330, 40, 300, 40), QPen(Qt::green, 6));
    group_YA->addToGroup(YA_1);

    // Верхняя горизонтальная линия
    QGraphicsItem *YA_2 = scene->addLine(QLineF(330, 15, 300, 15), QPen(Qt::green, 6));
    group_YA->addToGroup(YA_2);

    // Правая вертикальная линия (перпендикулярная, идет вниз)
    QGraphicsItem *YA_3 = scene->addLine(QLineF(330, 15, 330, 60), QPen(Qt::green, 6));
    group_YA->addToGroup(YA_3);

    // Нижняя горизонтальная линия
    QGraphicsItem *YA_4 = scene->addLine(QLineF(330, 15, 330, 40), QPen(Qt::green, 6));
    group_YA->addToGroup(YA_4);

    // Наклонная линия
    QGraphicsItem *YA_5 = scene->addLine(QLineF(300, 60, 330, 40), QPen(Qt::green, 6));
    group_YA->addToGroup(YA_5);

    // Наклонная линия
    QGraphicsItem *YA_6 = scene->addLine(QLineF(300, 15, 300, 40), QPen(Qt::green, 6));
    group_YA->addToGroup(YA_6);


    // Устанавливаем флаги для взаимодействия
    groupSetFlags(group_YA);

    // Добавляем группу на сцену
    scene->addItem(group_YA);



    // Н
    QGraphicsItemGroup *group_N2 = new QGraphicsItemGroup();
    QGraphicsItem *N2_1 = scene->addLine(QLineF(340, 15, 340, 60), QPen(Qt::blue, 6));
    group_N2->addToGroup(N2_1);
    QGraphicsItem *N2_2 = scene->addLine(QLineF(340, 15, 360, 60), QPen(Qt::blue, 6));
    group_N2->addToGroup(N2_2);
    QGraphicsItem *N2_3 = scene->addLine(QLineF(360, 15, 360, 60), QPen(Qt::blue, 6));
    group_N2->addToGroup(N2_3);
    groupSetFlags(group_N2);
    scene->addItem(group_N2);

    scene->update();  // Обновить всю сцену
}


void GraphicEdit::closeEvent(QCloseEvent *event){

    QMessageBox msgBox(this);
    msgBox.setWindowTitle("Подтверждение");
    msgBox.setText("Выберите действие:");

    QPushButton *mainMenuButton = msgBox.addButton("Выйти в главное меню", QMessageBox::ActionRole);
    QPushButton *exitButton = msgBox.addButton("Завершить приложение", QMessageBox::ActionRole);
    QPushButton *cancelButton = msgBox.addButton("Отмена", QMessageBox::RejectRole);

    msgBox.exec();

    if (msgBox.clickedButton() == mainMenuButton) {

        emit this->onGoToMenuEmitted();
        event->accept();

    } else if (msgBox.clickedButton() == exitButton) {

        event->accept();

        QApplication::closeAllWindows();
        QApplication::exit();

    } else {
        event->ignore();
    }
}

GraphicEdit::~GraphicEdit()
{
    delete ui;
}

void GraphicEdit::on_Eraser_triggered()
{
    QDialog dialog(this);
    dialog.setWindowTitle(tr("Размер ластика"));

    QLabel *label = new QLabel(tr("Выбери размер ластика:"), &dialog);
    QSpinBox *sizeSpinBox = new QSpinBox(&dialog);
    sizeSpinBox->setRange(1, 100);
    sizeSpinBox->setValue(10);

    QPushButton *okButton = new QPushButton(tr("OK"), &dialog);
    connect(okButton, &QPushButton::clicked, &dialog, &QDialog::accept);

    QVBoxLayout *layout = new QVBoxLayout(&dialog);
    layout->addWidget(label);
    layout->addWidget(sizeSpinBox);
    layout->addWidget(okButton);

    if (dialog.exec() == QDialog::Accepted) {
        int eraserSize = sizeSpinBox->value();

        QPen eraserPen;
        eraserPen.setColor(scene->backgroundBrush().color());
        eraserPen.setWidth(eraserSize);

        view->setPen(eraserPen);
        view->setEraserMode(true);
    }
}

void GraphicEdit::on_Feather_triggered()
{
    QDialog dialog(this);
    dialog.setWindowTitle(tr("Настройка пера"));

    QVBoxLayout *layout = new QVBoxLayout(&dialog);
    QFormLayout *formLayout = new QFormLayout();

    QComboBox *styleComboBox = new QComboBox();
    styleComboBox->setIconSize(QSize(64, 64));
    styleComboBox->addItem(QIcon(":/icons/icons/penIcons/Solid.png"), "Solid", QVariant::fromValue(Qt::SolidLine));
    styleComboBox->addItem(QIcon(":/icons/icons/penIcons/Dash"), "Dash", QVariant::fromValue(Qt::DashLine));
    styleComboBox->addItem(QIcon(":/icons/icons/penIcons/DotLine"), "Dot", QVariant::fromValue(Qt::DotLine));
    styleComboBox->addItem(QIcon(":/icons/icons/penIcons/DashDotLine"), "Dash Dot", QVariant::fromValue(Qt::DashDotLine));
    styleComboBox->addItem(QIcon(":/icons/icons/penIcons/DashDotDotLine"), "Dash Dot Dot", QVariant::fromValue(Qt::DashDotDotLine));
    styleComboBox->setCurrentIndex(styleComboBox->findData(static_cast<int>(currentPen.style())));
    formLayout->addRow(tr("Стиль:"), styleComboBox);

    QSpinBox *widthSpinBox = new QSpinBox();
    widthSpinBox->setRange(1, 20);
    widthSpinBox->setValue(currentPen.width());
    formLayout->addRow(tr("Ширина:"), widthSpinBox);

    QPushButton *colorButton = new QPushButton(tr("Выбрать цвет"));
    QColor penColor = currentPen.color();
    colorButton->setStyleSheet(QString("background-color: %1").arg(penColor.name()));
    connect(colorButton, &QPushButton::clicked, [&]()
            {
                QColor color = QColorDialog::getColor(penColor, this, tr("Выберите цвет пера"));
                if (color.isValid()) {
                    penColor = color;
                    colorButton->setStyleSheet(QString("background-color: %1").arg(color.name()));
                } });
    formLayout->addRow(tr("Цвет:"), colorButton);

    QComboBox *capStyleComboBox = new QComboBox();
    capStyleComboBox->setIconSize(QSize(64, 64));
    capStyleComboBox->addItem(QIcon(":/icons/icons/penIcons/FlatCap"), "Flat", QVariant::fromValue(Qt::FlatCap));
    capStyleComboBox->addItem(QIcon(":/icons/icons/penIcons/RoundCap"), "Round", QVariant::fromValue(Qt::RoundCap));
    capStyleComboBox->addItem(QIcon(":/icons/icons/penIcons/SquareCap"), "Square", QVariant::fromValue(Qt::SquareCap));
    capStyleComboBox->setCurrentIndex(capStyleComboBox->findData(static_cast<int>(currentPen.capStyle())));
    formLayout->addRow(tr("Стиль конца:"), capStyleComboBox);

    QComboBox *joinStyleComboBox = new QComboBox();
    joinStyleComboBox->setIconSize(QSize(64, 64));
    joinStyleComboBox->addItem(QIcon(":/icons/icons/penIcons/MilterJoin"), "Miter", QVariant::fromValue(Qt::MiterJoin));
    joinStyleComboBox->addItem(QIcon(":/icons/icons/penIcons/BevelJoin"), "Bevel", QVariant::fromValue(Qt::BevelJoin));
    joinStyleComboBox->addItem(QIcon(":/icons/icons/penIcons/RoundJoin"), "Round", QVariant::fromValue(Qt::RoundJoin));
    joinStyleComboBox->setCurrentIndex(joinStyleComboBox->findData(static_cast<int>(currentPen.joinStyle())));
    formLayout->addRow(tr("Стиль соединения:"), joinStyleComboBox);

    layout->addLayout(formLayout);

    QPushButton *okButton = new QPushButton(tr("ОК"));
    layout->addWidget(okButton);

    connect(okButton, &QPushButton::clicked, [&]()
            {
                currentPen.setStyle(static_cast<Qt::PenStyle>(styleComboBox->currentData().value<int>()));
                currentPen.setWidth(widthSpinBox->value());
                currentPen.setColor(penColor);
                currentPen.setCapStyle(static_cast<Qt::PenCapStyle>(capStyleComboBox->currentData().value<int>()));
                currentPen.setJoinStyle(static_cast<Qt::PenJoinStyle>(joinStyleComboBox->currentData().value<int>()));

                view->setPen(currentPen);
                dialog.accept();
            });

    dialog.exec();
    view->setEraserMode(false);
}


void GraphicEdit::on_Figure_triggered()
{
    QDialog dialog(this);
    dialog.setWindowTitle(tr("Добавить фигуру"));

    QVBoxLayout *layout = new QVBoxLayout(&dialog);
    QFormLayout *formLayout = new QFormLayout();

    // Выбор типа фигуры
    QComboBox *shapeComboBox = new QComboBox();
    shapeComboBox->addItem("Квадрат", "Square");
    shapeComboBox->addItem("Прямоугольник", "Rectangle");
    shapeComboBox->addItem("Треугольник", "Triangle");
    shapeComboBox->addItem("Круг", "Circle");
    shapeComboBox->addItem("Эллипс", "Ellipse");
    formLayout->addRow(tr("Тип фигуры:"), shapeComboBox);

    // Ввод размера (ширина и высота)
    QSpinBox *widthSpinBox = new QSpinBox();
    widthSpinBox->setRange(1, 1000);
    widthSpinBox->setValue(100);
    formLayout->addRow(tr("Ширина:"), widthSpinBox);

    QSpinBox *heightSpinBox = new QSpinBox();
    heightSpinBox->setRange(1, 1000);
    heightSpinBox->setValue(100);
    formLayout->addRow(tr("Высота:"), heightSpinBox);

    // Обновление полей при изменении типа фигуры
    auto updateShapeInputs = [&]() {
        QString shapeType = shapeComboBox->currentData().toString();

        if (shapeType == "Square" || shapeType == "Circle") {
            heightSpinBox->setValue(widthSpinBox->value());
            heightSpinBox->setEnabled(false); // Выключаем поле высоты
        } else {
            heightSpinBox->setEnabled(true);  // Включаем поле высоты
        }
    };

    connect(shapeComboBox, QOverload<int>::of(&QComboBox::currentIndexChanged), updateShapeInputs);
    updateShapeInputs(); // Инициализация

    // Выбор цвета заливки
    QPushButton *fillColorButton = new QPushButton(tr("Цвет заливки"));
    QColor fillColor = Qt::yellow;
    fillColorButton->setStyleSheet(QString("background-color: %1").arg(fillColor.name()));
    connect(fillColorButton, &QPushButton::clicked, [&]() {
        QColor color = QColorDialog::getColor(fillColor, this, tr("Выберите цвет заливки"));
        if (color.isValid()) {
            fillColor = color;
            fillColorButton->setStyleSheet(QString("background-color: %1").arg(color.name()));
        }
    });
    formLayout->addRow(tr("Цвет заливки:"), fillColorButton);

    // Выбор цвета обводки
    QPushButton *strokeColorButton = new QPushButton(tr("Цвет обводки"));
    QColor strokeColor = Qt::black;
    strokeColorButton->setStyleSheet(QString("background-color: %1").arg(strokeColor.name()));
    connect(strokeColorButton, &QPushButton::clicked, [&]() {
        QColor color = QColorDialog::getColor(strokeColor, this, tr("Выберите цвет обводки"));
        if (color.isValid()) {
            strokeColor = color;
            strokeColorButton->setStyleSheet(QString("background-color: %1").arg(color.name()));
        }
    });
    formLayout->addRow(tr("Цвет обводки:"), strokeColorButton);

    // Выбор ширины обводки
    QSpinBox *strokeWidthSpinBox = new QSpinBox();
    strokeWidthSpinBox->setRange(1, 20);
    strokeWidthSpinBox->setValue(2);
    formLayout->addRow(tr("Ширина обводки:"), strokeWidthSpinBox);

    // Выбор стиля заливки
    QComboBox *brushComboBox = new QComboBox();
    brushComboBox->addItem("Кисть не задана", "NoBrush");
    brushComboBox->addItem("Однородный цвет", "SolidPattern");
    brushComboBox->addItem("Чрезвычайно плотная кисть", "Dense1Pattern");
    brushComboBox->addItem("Очень плотная кисть", "Dense2Pattern");
    brushComboBox->addItem("Довольно плотная кисть", "Dense3Pattern");
    brushComboBox->addItem("Наполовину плотная кисть", "Dense4Pattern");
    brushComboBox->addItem("Довольно редкая кисть", "Dense5Pattern");
    brushComboBox->addItem("Очень редкая кисть", "Dense6Pattern");
    brushComboBox->addItem("Чрезвычайно редкая кисть", "Dense7Pattern");
    brushComboBox->addItem("Горизонтальные линии", "HorPattern");
    brushComboBox->addItem("Вертикальные линии", "VerPattern");
    brushComboBox->addItem("Пересекающиеся вертикальные и горизонтальные линии", "CrossPattern");
    brushComboBox->addItem("Обратные диагональные линии", "BDiagPattern");
    brushComboBox->addItem("Прямые диагональные линии", "FDiagPattern");
    brushComboBox->addItem("Пересекающиеся диагональные линии", "DiagCrossPattern");
    formLayout->addRow(tr("Тип заливки:"), brushComboBox);

    layout->addLayout(formLayout);

    // Кнопка ОК
    QPushButton *okButton = new QPushButton(tr("ОК"));
    layout->addWidget(okButton);

    connect(okButton, &QPushButton::clicked, [&]() {
        QString shapeType = shapeComboBox->currentData().toString();
        int width = widthSpinBox->value();
        int height = (shapeType == "Square" || shapeType == "Circle") ? width : heightSpinBox->value();
        int x = (view->viewport()->width() - width) / 2;
        int y = (view->viewport()->height() - height) / 2;

        Qt::BrushStyle brushStyle = stringToBrushStyle(brushComboBox->currentData().toString());

        // Добавляем фигуру через функцию addShape
        addShape(shapeType, QRectF(x, y, width, height), fillColor, brushStyle, strokeColor, strokeWidthSpinBox->value());

        dialog.accept();
    });

    dialog.exec();
}


Qt::BrushStyle GraphicEdit::stringToBrushStyle(const QString &styleStr) {
    static const QHash<QString, Qt::BrushStyle> brushStyleMap = {
        {"SolidPattern", Qt::SolidPattern},
        {"Dense1Pattern", Qt::Dense1Pattern},
        {"Dense2Pattern", Qt::Dense2Pattern},
        {"Dense3Pattern", Qt::Dense3Pattern},
        {"Dense4Pattern", Qt::Dense4Pattern},
        {"Dense5Pattern", Qt::Dense5Pattern},
        {"Dense6Pattern", Qt::Dense6Pattern},
        {"Dense7Pattern", Qt::Dense7Pattern},
        {"HorPattern", Qt::HorPattern},
        {"VerPattern", Qt::VerPattern},
        {"CrossPattern", Qt::CrossPattern},
        {"BDiagPattern", Qt::BDiagPattern},
        {"FDiagPattern", Qt::FDiagPattern},
        {"DiagCrossPattern", Qt::DiagCrossPattern}
    };

    return brushStyleMap.value(styleStr, Qt::NoBrush); // Возвращает NoBrush, если стиль не найден
}

void GraphicEdit::addShape(const QString &shapeType, const QRectF &rect, const QColor &fillColor, Qt::BrushStyle brushStyle, const QColor &strokeColor, int strokeWidth)
{
    static const QMap<QString, std::function<QGraphicsItem *(QGraphicsScene *, const QRectF &, const QPen &, const QBrush &)>> shapeCreators = {
        {"Square", [](QGraphicsScene *scene, const QRectF &rect, const QPen &pen, const QBrush &brush) {
            return scene->addRect(rect, pen, brush); // Квадрат
        }},
        {"Rectangle", [](QGraphicsScene *scene, const QRectF &rect, const QPen &pen, const QBrush &brush) {
            return scene->addRect(rect, pen, brush); // Прямоугольник
        }},
        {"Circle", [](QGraphicsScene *scene, const QRectF &rect, const QPen &pen, const QBrush &brush) {
            return scene->addEllipse(rect, pen, brush); // Круг
        }},
        {"Ellipse", [](QGraphicsScene *scene, const QRectF &rect, const QPen &pen, const QBrush &brush) {
            return scene->addEllipse(rect, pen, brush); // Эллипс
        }},
        {"Triangle", [](QGraphicsScene *scene, const QRectF &rect, const QPen &pen, const QBrush &brush) {
            QPolygonF triangle;
            triangle << QPointF(rect.left() + rect.width() / 2, rect.top())
                     << QPointF(rect.bottomLeft())
                     << QPointF(rect.bottomRight());
            return scene->addPolygon(triangle, pen, brush); // Треугольник
        }}
    };

    QPen pen(strokeColor, strokeWidth);
    QBrush brush(fillColor, brushStyle);

    // Найти функцию для создания фигуры
    auto it = shapeCreators.find(shapeType);
    QGraphicsItem *shape = (it != shapeCreators.end()) ? it.value()(scene, rect, pen, brush) : nullptr;

    if (shape) {
        shape->setFlag(QGraphicsItem::ItemIsMovable, true);           // Фигуры можно перемещать
        shape->setFlag(QGraphicsItem::ItemIsSelectable, true);        // Фигуры можно выделять
        shape->setFlag(QGraphicsItem::ItemSendsGeometryChanges, true); // Отправка изменений геометрии
    }
}

void GraphicEdit::on_SaveScene_triggered()
    {
        // Остановить все анимации или таймеры
        stopMovingObjects();

        try {
            // Открыть диалог для выбора пути сохранения
            QString filePath = QFileDialog::getSaveFileName(this, "Сохранить изображение", "", "Images (*.jpg *.png *.bmp)");
            if (filePath.isEmpty()) {
                return; // Если пользователь закрыл диалог, ничего не делаем
            }

            // Определяем размеры сцены и учитываем возможный масштаб экрана
            QRectF sceneRect = scene->sceneRect();
            QSize imageSize = sceneRect.size().toSize();
            qreal devicePixelRatio = this->devicePixelRatioF();
            imageSize *= devicePixelRatio;

            // Создаём изображение с учётом масштаба устройства
            QImage image(imageSize, QImage::Format_ARGB32);
            image.setDevicePixelRatio(devicePixelRatio);
            image.fill(Qt::white); // Задаём белый фон

            // Рисуем содержимое сцены в изображение
            QPainter painter(&image);
            scene->render(&painter);

            // Сохраняем изображение в файл
            if (!image.save(filePath)) {
                throw std::runtime_error("Ошибка сохранения файла.");
            }

            QMessageBox::information(this, "Успех", "Изображение успешно сохранено.");
        } catch (const std::exception &e) {
            QMessageBox::warning(this, "Ошибка", QString("Не удалось сохранить изображение: %1").arg(e.what()));
        }

        // Возобновить все анимации или таймеры
        resumeMovingObjects();
    }
// Функция для остановки всех движущихся объектов
void GraphicEdit::stopMovingObjects()
{
    for (int i = 0; i < movingStates.size(); ++i) {
        movingStates[i] = false;  // Устанавливаем состояние "остановлен"
    }
}
// Функция для возобновления движения всех движущихся объектов
void GraphicEdit::resumeMovingObjects()
{
    for (int i = 0; i < movingStates.size(); ++i) {
        movingStates[i] = true;  // Устанавливаем состояние "движется"
    }
}

void GraphicEdit::on_AddImage_triggered()
{
    // Остановить анимации или таймеры
    stopMovingObjects();

    try {
        // Открыть диалог для выбора изображения
        QString filePath = QFileDialog::getOpenFileName(this, "Выберите изображение", "", "Images (*.png *.jpg *.bmp *.jpeg)");
        if (filePath.isEmpty()) {
            return; // Пользователь отменил выбор
        }

        // Загружаем изображение
        QPixmap pixmap(filePath);
        if (pixmap.isNull()) {
            throw std::runtime_error("Не удалось загрузить изображение.");
        }

        // Создаём элемент для отображения изображения
        auto* item = new QGraphicsPixmapItem(pixmap);
        item->setData(0, "image");
        item->setFlags(QGraphicsItem::ItemIsMovable | QGraphicsItem::ItemIsSelectable | QGraphicsItem::ItemSendsGeometryChanges);

        // Центрируем изображение на сцене с учётом текущего масштаба
        QPointF sceneCenter = scene->sceneRect().center();
        item->setPos(sceneCenter - QPointF(pixmap.width() / 2.0, pixmap.height() / 2.0));

        // Добавляем элемент на сцену
        scene->addItem(item);

        // Выводим сообщение об успешной загрузке
        QMessageBox::information(this, "Успех", "Изображение добавлено.");
    } catch (const std::exception& e) {
        QMessageBox::warning(this, "Ошибка", QString("Не удалось добавить изображение: %1").arg(e.what()));
    }

    // Возобновить анимации или таймеры
    resumeMovingObjects();
}

void GraphicEdit::createMovingObject_Car(int startX, int startY){

    QGraphicsEllipseItem* wheel1 = new QGraphicsEllipseItem(0, 20, 15, 15);
    wheel1->setBrush(Qt::black);

    QGraphicsEllipseItem* wheel2 = new QGraphicsEllipseItem(50, 20, 15, 15);
    wheel2->setBrush(Qt::black);

    QGraphicsRectItem* body = new QGraphicsRectItem(10, 10, 40, 20);
    body->setBrush(Qt::red);

    QGraphicsRectItem* window = new QGraphicsRectItem(15, 12, 30, 10);
    window->setBrush(Qt::blue);

    QGraphicsItemGroup* car = new QGraphicsItemGroup();
    car->addToGroup(wheel1);
    car->addToGroup(wheel2);
    car->addToGroup(body);
    car->addToGroup(window);

    car->setFlag(QGraphicsItem::ItemIsSelectable, true);
    scene->addItem(car);

    car->setPos(startX, startY);

    movingItemGroups.append(car);
    velocities.append(QPointF(-2, 2));
    movingStates.append(true);
}

void GraphicEdit::createMovingObject_Phone(int startX, int startY){

    // Создание корпуса телефона
    QGraphicsRectItem* body = new QGraphicsRectItem(10, 10, 50, 100);
    body->setBrush(Qt::darkGray);
    body->setPen(QPen(Qt::black));

    // Создание экрана
    QGraphicsRectItem* screen = new QGraphicsRectItem(15, 15, 40, 60);
    screen->setBrush(Qt::lightGray);
    screen->setPen(QPen(Qt::black));

    // Создание кнопки "домой"
    QGraphicsEllipseItem* homeButton = new QGraphicsEllipseItem(30, 80, 10, 10);
    homeButton->setBrush(Qt::black);

    // Создание кнопок громкости
    QGraphicsRectItem* volumeUp = new QGraphicsRectItem(5, 25, 3, 10);
    volumeUp->setBrush(Qt::black);

    QGraphicsRectItem* volumeDown = new QGraphicsRectItem(5, 40, 3, 10);
    volumeDown->setBrush(Qt::black);

    // Группируем фигуры в один объект
    QGraphicsItemGroup* phone = new QGraphicsItemGroup();
    phone->addToGroup(body);
    phone->addToGroup(screen);
    phone->addToGroup(homeButton);
    phone->addToGroup(volumeUp);
    phone->addToGroup(volumeDown);

    // Устанавливаем флаг для выбора объекта
    phone->setFlag(QGraphicsItem::ItemIsSelectable, true);

    // Добавляем объект на сцену
    scene->addItem(phone);

    // Устанавливаем начальную позицию телефона, используя параметры функции
    phone->setPos(startX, startY);

    // Добавляем объект и его начальную скорость в соответствующие списки
    movingItemGroups.append(phone);
    velocities.append(QPointF(2, -2)); // Скорость по осям X и Y
    movingStates.append(true);
}

void GraphicEdit::createMovingObject_Flower()
{
    // Создание лепестков
    QGraphicsEllipseItem *petal1 = new QGraphicsEllipseItem(20,20,20,30);  // Верхний лепесток
    petal1->setBrush(Qt::red);
    QGraphicsEllipseItem *petal2 = new QGraphicsEllipseItem(20,10,20,30); // Правый лепесток
    petal2->setBrush(Qt::red);
    QGraphicsEllipseItem *petal3 = new QGraphicsEllipseItem(10,20,30,20); // Нижний лепесток
    petal3->setBrush(Qt::red);
    QGraphicsEllipseItem *petal4 = new QGraphicsEllipseItem(20,20,30,20);
    petal4->setBrush(Qt::red);

    // Создание центра цветка
    QGraphicsEllipseItem *center = new QGraphicsEllipseItem(20, 20, 20, 20);
    center->setBrush(Qt::black);

    // Создание стебля
    QGraphicsRectItem *stem = new QGraphicsRectItem(25, 30, 10, 50);
    stem->setBrush(Qt::darkGreen);


    // Группируем фигуры в один объект (цветок)
    QGraphicsItemGroup *flower = new QGraphicsItemGroup();
    flower->addToGroup(stem);
    flower->addToGroup(petal1);
    flower->addToGroup(petal2);
    flower->addToGroup(petal3);
    flower->addToGroup(petal4);
    flower->addToGroup(center);

    // Добавляем объект на сцену
    flower->setFlag(QGraphicsItem::ItemIsSelectable, true);
    scene->addItem(flower);

    flower->setPos(200, 400); // Начальная позиция объекта

    // Добавляем объект и его начальную скорость в соответствующие списки
    movingItemGroups.append(flower);
    velocities.append(QPointF(-2, 2)); // Скорость по осям X и Y
    movingStates.append(true);
}


void GraphicEdit::on_DeleteFigure_triggered()
{
    // Получаем список всех выбранных объектов на сцене
    QList<QGraphicsItem *> selectedItems = scene->selectedItems();



    foreach (QGraphicsItem *item, selectedItems) {

        if (QGraphicsItemGroup *group = qgraphicsitem_cast<QGraphicsItemGroup *>(item)) {

            movingItemGroups.removeAll(group);

            QList<QGraphicsItem *> children = group->childItems();
            for (QGraphicsItem *child : children) {
                scene->removeItem(child);  // Убираем элемент из сцены
                delete child;              // Удаляем элемент
            }

            scene->removeItem(group);  // Убираем саму группу из сцены
            delete group;              // Удаляем саму группу
        } else {

            scene->removeItem(item);  // Убираем объект из сцены
            delete item;              // Удаляем объект
        }
    }

    // Обновляем сцену и выводим сообщение
    scene->update();
}
