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

GraphicEdit::GraphicEdit(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::GraphicEdit)
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
}

void GraphicEdit::setupWalls(){

}

void GraphicEdit::updateWallPositions(){

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
