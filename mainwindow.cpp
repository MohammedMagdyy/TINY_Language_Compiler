#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QFileDialog>
#include <QMessageBox>
#include "operation_window.h"


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

}


MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_pushButton_clicked()
{
    QString fileName = QFileDialog::getOpenFileName(this, "Choose a File", "", "All Files (*.*)");

    if (!fileName.isEmpty()) {
        this->close();

        // Open the new window
        operation_window *operationWindow = new operation_window(fileName);
        operationWindow->show();
    }

}

