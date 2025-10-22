#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QClipboard>
#include <QGuiApplication>
#include <QPushButton>
#include <QFontDatabase>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // اتصالات دکمه‌ها
    connect(ui->btnAddUser, &QPushButton::clicked, this, &MainWindow::addEntry);
    connect(ui->btnDeleteUser, &QPushButton::clicked, this, &MainWindow::deleteSelected);
    connect(ui->btnUpdateUser, &QPushButton::clicked, this, &MainWindow::updateSelected);
    connect(ui->listUsers, &QListWidget::itemDoubleClicked, this, &MainWindow::showDetails);

    // تنظیم placeholder ها
    ui->inputAppName->setPlaceholderText("App name (e.g., Gmail)");
    ui->inputUsername->setPlaceholderText("Username or email");
    ui->inputPassword->setPlaceholderText("Password");
    ui->inputPassword->setEchoMode(QLineEdit::Password);

    ui->inputAppName->setClearButtonEnabled(true);
    ui->inputUsername->setClearButtonEnabled(true);
    ui->inputPassword->setClearButtonEnabled(true);

    // فونت زیبا و خوانا
    QFont font("Segoe UI", 11, QFont::Normal);
    QApplication::setFont(font);

    // استایل (QSS)
    this->setStyleSheet(R"(
        QWidget {
            background-color: #1c1f2b;
            color: #e8e8e8;
            font-family: 'Segoe UI';
            font-size: 14px;
        }

        QLineEdit {
            background-color: #2a2d3a;
            border: 1px solid #3d4152;
            border-radius: 6px;
            padding: 6px;
            color: #ffffff;
        }

        QPushButton {
            background-color: #4b6bfb;
            border: none;
            border-radius: 6px;
            padding: 7px 14px;
            color: #ffffff;
            font-weight: 600;
        }

        QPushButton:hover {
            background-color: #6b84ff;
        }

        QListWidget {
            background-color: #242633;
            border: 1px solid #3a3d4f;
            border-radius: 8px;
            color: #ffffff;
        }
    )");

    // لود داده‌ها از فایل
    loadData();
}

MainWindow::~MainWindow()
{
    saveData(); // ذخیره‌ی خودکار هنگام خروج
    delete ui;
}

// افزودن رکورد جدید
void MainWindow::addEntry()
{
    QString app = ui->inputAppName->text().trimmed();
    QString user = ui->inputUsername->text().trimmed();
    QString pass = ui->inputPassword->text().trimmed();

    if (app.isEmpty() || user.isEmpty() || pass.isEmpty()) {
        QMessageBox::warning(this, "Incomplete", "Please fill in all fields!");
        return;
    }

    dataMap[app] = qMakePair(user, pass);

    if (!ui->listUsers->findItems(app, Qt::MatchExactly).isEmpty()) return;

    ui->listUsers->addItem(app);

    ui->inputAppName->clear();
    ui->inputUsername->clear();
    ui->inputPassword->clear();

    saveData();
}

// حذف آیتم انتخابی
void MainWindow::deleteSelected()
{
    QListWidgetItem *item = ui->listUsers->currentItem();
    if (!item) return;

    QString app = item->text();
    dataMap.remove(app);
    delete item;

    saveData();
}

// ویرایش آیتم انتخابی
void MainWindow::updateSelected()
{
    QListWidgetItem *item = ui->listUsers->currentItem();
    if (!item) {
        QMessageBox::warning(this, "Select item", "Please select an app to update!");
        return;
    }

    QString oldApp = item->text();
    QString newApp = ui->inputAppName->text().trimmed();
    QString user = ui->inputUsername->text().trimmed();
    QString pass = ui->inputPassword->text().trimmed();

    if (newApp.isEmpty() || user.isEmpty() || pass.isEmpty()) {
        QMessageBox::warning(this, "Incomplete", "Please fill in all fields!");
        return;
    }

    dataMap.remove(oldApp);
    dataMap[newApp] = qMakePair(user, pass);
    item->setText(newApp);

    ui->inputAppName->clear();
    ui->inputUsername->clear();
    ui->inputPassword->clear();

    saveData();
}

// نمایش جزئیات هنگام دوبار کلیک
void MainWindow::showDetails(QListWidgetItem *item)
{
    QString app = item->text();
    if (!dataMap.contains(app)) return;

    QString user = dataMap[app].first;
    QString pass = dataMap[app].second;

    QMessageBox msgBox;
    msgBox.setWindowTitle(app);
    msgBox.setText(QString("Username: %1\nPassword: %2").arg(user, pass));

    QPushButton *copyUserBtn = msgBox.addButton("Copy Username", QMessageBox::ActionRole);
    QPushButton *copyPassBtn = msgBox.addButton("Copy Password", QMessageBox::ActionRole);
    msgBox.addButton("Close", QMessageBox::RejectRole);

    msgBox.exec();

    QClipboard *clipboard = QGuiApplication::clipboard();

    if (msgBox.clickedButton() == copyUserBtn) {
        clipboard->setText(user);
    } else if (msgBox.clickedButton() == copyPassBtn) {
        clipboard->setText(pass);
    }
}

// ذخیره‌سازی داده‌ها در فایل JSON
void MainWindow::saveData()
{
    QJsonArray jsonArray;
    for (auto it = dataMap.begin(); it != dataMap.end(); ++it) {
        QJsonObject obj;
        obj["app"] = it.key();
        obj["username"] = it.value().first;
        obj["password"] = it.value().second;
        jsonArray.append(obj);
    }

    QJsonDocument doc(jsonArray);
    QFile file("data.json");
    if (file.open(QIODevice::WriteOnly)) {
        file.write(doc.toJson());
        file.close();
    }
}

// بارگذاری داده‌ها از فایل JSON
void MainWindow::loadData()
{
    QFile file("data.json");
    if (!file.exists()) return;

    if (file.open(QIODevice::ReadOnly)) {
        QByteArray data = file.readAll();
        file.close();

        QJsonDocument doc = QJsonDocument::fromJson(data);
        if (doc.isArray()) {
            QJsonArray jsonArray = doc.array();
            for (auto val : jsonArray) {
                QJsonObject obj = val.toObject();
                QString app = obj["app"].toString();
                QString user = obj["username"].toString();
                QString pass = obj["password"].toString();

                dataMap[app] = qMakePair(user, pass);
                ui->listUsers->addItem(app);
            }
        }
    }
}
