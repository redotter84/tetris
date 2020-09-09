#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QPainter>
#include <QTimer>
#include <QColor>
#include <QKeyEvent>
#include <QLabel>
#include <QPushButton>
#include <QDesktopWidget>
#include <vector>
#include <algorithm>
#include <string>
#include <time.h>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

protected:
    void paintEvent(QPaintEvent* event);
    void keyPressEvent(QKeyEvent *event);
    bool rotate();
    void the_end();
    bool check(int a, int pos);
    int get_num(int x, int y);
    bool sift(int dx, int dy);
    void freeze();
    void init();
    void set_gui();
    QTimer *timer = nullptr;
    QLabel *label = nullptr;
    QPushButton *new_game = nullptr;
    QPushButton *pause = nullptr;

public slots:
    void go(int d=0);
    void reset();
    void do_pause();

private:
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
