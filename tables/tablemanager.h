#ifndef TABLEMANAGER_H
#define TABLEMANAGER_H

#include <QObject>
#include <QTableWidget>
#include <QFile>

#include "tables/tableparams.h"
#include "tables/savetableresults.h"

class TableManager : public QObject
{
    Q_OBJECT
public:
    explicit TableManager(QObject *parent = nullptr);

    QTableWidget* CreateTable(const TableParams& tableParams);
    QTableWidget* OpenExistingTable(const QString& path);

    SaveTableResults SaveTable(QTableWidget* table, const QString& path);
private:
    // сохраняет в file данные из table
    QJsonArray SaveToTextStream(QFile& file, QTableWidget* table);
    void SaveToFile(QJsonArray& cellSettingsArray, const QString& path);

    const QString settingsPath = "../SimpleWord/settings/tableSettings";
signals:
};

#endif // TABLEMANAGER_H
