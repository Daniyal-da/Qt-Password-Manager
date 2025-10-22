#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QListWidgetItem>
#include <QMessageBox>
#include <QMap>
#include <QPair>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QFile>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void addEntry();
    void deleteSelected();
    void updateSelected();
    void showDetails(QListWidgetItem *item);

private:
    Ui::MainWindow *ui;
    QMap<QString, QPair<QString, QString>> dataMap;

    void saveData();   // ذخیره در JSON
    void loadData();   // لود از JSON
};

#endif // MAINWINDOW_H
