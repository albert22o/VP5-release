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

