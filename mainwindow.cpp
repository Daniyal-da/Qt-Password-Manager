#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QFile>
#include <QDir>
#include <QStandardPaths>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <QListWidgetItem>
#include <QCloseEvent>
#include <QMessageBox>
#include <QClipboard>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // اتصال دکمه‌ها
    connect(ui->btnAddUser, &QPushButton::clicked, this, &MainWindow::addUser);
    connect(ui->btnUpdateUser, &QPushButton::clicked, this, &MainWindow::updateUser);
    connect(ui->btnDeleteUser, &QPushButton::clicked, this, &MainWindow::deleteUser);
    connect(ui->listUsers, &QListWidget::itemSelectionChanged, this, &MainWindow::onSelectionChanged);
    connect(ui->listUsers, &QListWidget::itemDoubleClicked, this, &MainWindow::onItemDoubleClicked);

    ui->inputPassword->setEchoMode(QLineEdit::Password);

    loadData();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    saveData();
    QMainWindow::closeEvent(event);
}

QString MainWindow::dataFilePath() const
{
    const QString dir = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    QDir().mkpath(dir);
    return dir + "/users.json";
}

void MainWindow::onItemDoubleClicked(QListWidgetItem *item)
{
    if (!item) return;

    // نام کاربری و رمز عبور را از آیتم انتخاب‌شده می‌گیریم
    const QString username = item->text();
    const QString password = item->data(Qt::UserRole).toString();

    // متن اطلاعات حساب برای نمایش در پنجره
    QString infoText = QString("Username: %1\nPassword: %2")
                           .arg(username, password);

    // ساخت پنجره پیام با دکمه‌های جدا
    QMessageBox msgBox(this);
    msgBox.setWindowTitle("Account Information");
    msgBox.setText(infoText);

    // اضافه کردن دکمه‌های جدا برای کپی و بستن
    QPushButton *copyUserBtn = msgBox.addButton("Copy Username", QMessageBox::ActionRole);
    QPushButton *copyPassBtn = msgBox.addButton("Copy Password", QMessageBox::ActionRole);
    QPushButton *closeBtn = msgBox.addButton("Close", QMessageBox::RejectRole);

    msgBox.exec();

    // دسترسی به کلیپ‌بورد برای کپی کردن داده
    QClipboard *clipboard = QApplication::clipboard();

    // بررسی اینکه کدام دکمه فشرده شده
    if (msgBox.clickedButton() == copyUserBtn) {
        clipboard->setText(username);
        QMessageBox::information(this, "Copied ✅", "Username has been copied to clipboard.");
    }
    else if (msgBox.clickedButton() == copyPassBtn) {
        clipboard->setText(password);
        QMessageBox::information(this, "Copied ✅", "Password has been copied to clipboard.");
    }
}
void MainWindow::loadData()
{
    QFile f(dataFilePath());
    if (!f.open(QIODevice::ReadOnly)) return;

    const QJsonDocument doc = QJsonDocument::fromJson(f.readAll());
    f.close();
    if (!doc.isArray()) return;

    const QJsonArray arr = doc.array();
    ui->listUsers->clear();
    for (const QJsonValue &v : arr) {
        const QJsonObject obj = v.toObject();
        QString user = obj.value("username").toString();
        QString pass = obj.value("password").toString(); // متن ساده
        QListWidgetItem *item = new QListWidgetItem(user);
        item->setData(Qt::UserRole, pass);
        ui->listUsers->addItem(item);
    }
}

void MainWindow::saveData() const
{
    QJsonArray arr;
    for (int i = 0; i < ui->listUsers->count(); ++i) {
        QListWidgetItem *item = ui->listUsers->item(i);
        QJsonObject obj;
        obj["username"] = item->text();
        obj["password"] = item->data(Qt::UserRole).toString();
        arr.append(obj);
    }

    QFile f(dataFilePath());
    if (f.open(QIODevice::WriteOnly)) {
        QJsonDocument doc(arr);
        f.write(doc.toJson(QJsonDocument::Indented));
        f.close();
    }
}

void MainWindow::addUser()
{
    const QString username = ui->inputUsername->text().trimmed();
    const QString password = ui->inputPassword->text();

    if (username.isEmpty()) {
        QMessageBox::warning(this, "خطا", "نام کاربری نمی‌تواند خالی باشد.");
        return;
    }

    // اگر نام کاربری تکراری بود، هشدار بده
    for (int i = 0; i < ui->listUsers->count(); ++i) {
        if (ui->listUsers->item(i)->text().compare(username, Qt::CaseInsensitive) == 0) {
            QMessageBox::warning(this, "خطا", "این نام کاربری قبلاً وجود دارد.");
            return;
        }
    }

    QListWidgetItem *item = new QListWidgetItem(username);
    item->setData(Qt::UserRole, password);
    ui->listUsers->addItem(item);

    ui->inputUsername->clear();
    ui->inputPassword->clear();
    ui->inputUsername->setFocus();

    saveData();
}

void MainWindow::updateUser()
{
    const auto items = ui->listUsers->selectedItems();
    if (items.isEmpty()) {
        QMessageBox::information(this, "اطلاع", "ابتدا یک نام کاربری را از لیست انتخاب کنید.");
        return;
    }

    QListWidgetItem *item = items.first();
    const QString newUser = ui->inputUsername->text().trimmed();
    const QString newPass = ui->inputPassword->text();

    if (newUser.isEmpty()) {
        QMessageBox::warning(this, "خطا", "نام کاربری نمی‌تواند خالی باشد.");
        return;
    }

    // اگر نام کاربری جدید با نام دیگر تداخل دارد (غیر از خود آیتم انتخاب‌شده)
    for (int i = 0; i < ui->listUsers->count(); ++i) {
        if (ui->listUsers->item(i) == item) continue;
        if (ui->listUsers->item(i)->text().compare(newUser, Qt::CaseInsensitive) == 0) {
            QMessageBox::warning(this, "خطا", "این نام کاربری قبلاً وجود دارد.");
            return;
        }
    }

    item->setText(newUser);
    item->setData(Qt::UserRole, newPass);

    saveData();
}

void MainWindow::deleteUser()
{
    const auto items = ui->listUsers->selectedItems();
    if (items.isEmpty()) {
        QMessageBox::information(this, "اطلاع", "ابتدا یک نام کاربری را از لیست انتخاب کنید.");
        return;
    }

    // حذف هر آیتم انتخاب شده
    for (QListWidgetItem *it : items) {
        delete ui->listUsers->takeItem(ui->listUsers->row(it));
    }

    ui->inputUsername->clear();
    ui->inputPassword->clear();

    saveData();
}

void MainWindow::onSelectionChanged()
{
    const auto items = ui->listUsers->selectedItems();
    if (items.isEmpty()) {
        ui->inputUsername->clear();
        ui->inputPassword->clear();
        return;
    }

    QListWidgetItem *item = items.first();
    ui->inputUsername->setText(item->text());
    ui->inputPassword->setText(item->data(Qt::UserRole).toString());
}
