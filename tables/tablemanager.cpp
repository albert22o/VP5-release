#include "tablemanager.h"

#include <QFile>
#include <QFileInfo>
#include <QDir>
#include <QJsonArray>
#include <QJsonObject>
#include <QJsonDocument>
#include <QMessageBox>

TableManager::TableManager(QObject *parent)
    : QObject{parent}
{}

QTableWidget* TableManager::CreateTable(const TableParams& tableParams){
    // доделать
}

QTableWidget* TableManager::OpenExistingTable(const QString& path){

    QFile file(path);

    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        return nullptr;
    }

    QTextStream in(&file);
    QStringList lines;

    while (!in.atEnd())
    {
        lines.append(in.readLine());
    }

    file.close();

    int rows = lines.size();
    int columns = !lines.isEmpty() ? lines.first().split(",").size() : 0;

    if (rows == 0 || columns == 0)
    {
        return nullptr;
    }

    QTableWidget *newTableWidget = new QTableWidget(rows, columns);
    newTableWidget->setWindowTitle(path);

    for (int i = 0; i < rows; ++i)
    {
        QStringList cells = lines.at(i).split(",");
        if (cells.size() != columns)
        {
            QMessageBox::warning
            (
                nullptr,
                QObject::tr("Ошибка"),
                QObject::tr("Некорректный CSV файл: строки содержат разное количество столбцов")
            );

            delete newTableWidget;

            return nullptr;
        }

        for (int j = 0; j < cells.size(); ++j)
        {
            newTableWidget->setItem(i, j, new QTableWidgetItem(cells.at(j)));
        }
    }

    QFileInfo fileInfo(path);
    QDir settingsDir(settingsPath);

    QString jsonFilePath = settingsDir.absoluteFilePath(fileInfo.fileName() + ".json");
    QFile settingsFile(jsonFilePath);

    if (settingsFile.exists() && settingsFile.open(QIODevice::ReadOnly)){

        QByteArray settingsData = settingsFile.readAll();

        settingsFile.close();

        QJsonDocument settingsDoc = QJsonDocument::fromJson(settingsData);
        QJsonArray cellSettingsArray = settingsDoc.array();

        for (int i = 0; i < cellSettingsArray.size(); ++i)
        {
            QJsonArray rowSettings = cellSettingsArray[i].toArray();

            for (int j = 0; j < rowSettings.size(); ++j)
            {
                QTableWidgetItem *item = newTableWidget->item(i, j);

                if (item)
                {
                    QJsonObject cellSettings = rowSettings[j].toObject();

                    item->setForeground(QColor(cellSettings["textColor"].toString()));
                    item->setBackground(QColor(cellSettings["backgroundColor"].toString()));

                    QFont font;

                    font.fromString(cellSettings["font"].toString());

                    item->setFont(font);
                    item->setTextAlignment(cellSettings["alignment"].toInt());
                }
            }
        }
    }

    return newTableWidget;
}

SaveTableResults TableManager::SaveTable(QTableWidget* table, const QString& path){

    QFile file(path);

    if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        return SaveTableResults::CantOpenFileForWrite;
    }

    auto cellSettingsArray = SaveToTextStream(file, table);

    SaveToFile(cellSettingsArray, path);
    file.close();
}

QJsonArray TableManager::SaveToTextStream(QFile& file, QTableWidget* table){

    QTextStream out(&file);

    int rows = table->rowCount();
    int columns = table->columnCount();

    QJsonArray cellSettingsArray;

    for (int i = 0; i < rows; ++i)
    {
        QStringList rowContents;
        QJsonArray rowCellSettings;

        for (int j = 0; j < columns; ++j)
        {
            QTableWidgetItem *item = table->item(i, j);

            if (!item)
            {
                item = new QTableWidgetItem("");
                table->setItem(i, j, item);
            }

            QString cellText = item->text();
            rowContents << cellText;

            QJsonObject cellSettings;
            cellSettings["textColor"] = item->foreground().color().name();
            cellSettings["backgroundColor"] = item->background().color().name();
            cellSettings["font"] = item->font().toString();
            qDebug() << item->font().toString();
            cellSettings["alignment"] = item->textAlignment();
            rowCellSettings.append(cellSettings);
        }

        out << rowContents.join(",") << "\n";
        cellSettingsArray.append(rowCellSettings);
    }

    return cellSettingsArray;
}

void TableManager::SaveToFile(QJsonArray& cellSettingsArray, const QString& path){

    QFileInfo fileInfo(path);
    QDir settingsDir(settingsPath);

    if (!settingsDir.exists() && !settingsDir.mkpath("."))
    {
        auto error = "Unable to create directory: " + settingsDir.absolutePath();

        throw std::runtime_error(error.toStdString());
    }

    QString jsonFilePath = settingsDir.absoluteFilePath(fileInfo.fileName() + ".json");
    QFile settingsFile(jsonFilePath);

    if (settingsFile.open(QIODevice::WriteOnly))
    {
        QJsonDocument settingsDoc(cellSettingsArray);
        settingsFile.write(settingsDoc.toJson());
        settingsFile.close();
    }
}
