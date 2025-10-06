#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QListWidgetItem>   // ← این خط اضافه شود

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void addUser();
    void updateUser();
    void deleteUser();
    void onSelectionChanged();
    void onItemDoubleClicked(QListWidgetItem *item);   // ← این خط اضافه شود

protected:
    void closeEvent(QCloseEvent *event) override;

private:
    void loadData();
    void saveData() const;
    QString dataFilePath() const;

    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
