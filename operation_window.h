#ifndef OPERATION_WINDOW_H
#define OPERATION_WINDOW_H

#include <QWidget>

namespace Ui {
class operation_window;
}

class operation_window : public QWidget
{
    Q_OBJECT

public:
    explicit operation_window(const QString &filePath, QWidget *parent = nullptr);

    ~operation_window();

private slots:
    void on_pushButton_clicked();

    void on_pushButton_2_clicked();

private:
    QString filePath;
    Ui::operation_window *ui;
};

#endif // OPERATION_WINDOW_H


