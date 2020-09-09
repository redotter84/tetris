#include "mainwindow.h"
#include "ui_mainwindow.h"

using namespace std;

const int N = 20, M = 10;
const int side = 30, border = 2;
const int begx = 3, begy = 0;
const int speed = 350;
const int topx = 50, topy = 50;
const int lwidth = 270, lheight = 300;
const int bwidth = 160, bheight = 80, bdist = 20;

QColor getRgb(int r, int g, int b) {
    return QColor(r, g, b);
}

const vector<int> figures = { 15, 4369, 15, 4369, 275, 71, 802, 113, 547, 116, 785, 23, 114, 305, 39, 562, 54, 561, 54, 561, 99, 306, 99, 306, 51, 51, 51, 51 };
const vector<QColor> colors = { getRgb(30, 144, 255), getRgb(0, 0, 205), getRgb(239, 155, 0), getRgb(224, 224, 4), getRgb(34, 139, 34), getRgb(139, 25, 211), getRgb(220, 20, 60)};
const vector<pair<int, int>> dirs = {{0, +1}, {-1, 0}, {1, 0}};
vector<vector<int>> field(N + 8, vector<int>(M + 4, 1));
int cur_x, cur_y, cur_fig;
int score, max_score = 0;
int cur_speed;

void MainWindow::reset() {
    field.assign(N + 8, vector<int>(M + 4, 1));
    init();
}

void MainWindow::do_pause() {
    if (timer->isActive()) {
        timer->stop();
        pause->setText("Continue");
    } else {
        timer->start(cur_speed);
        pause->setText("Pause");
    }
}

void MainWindow::set_gui() {
    QFont font("Arial");
    font.setPixelSize(30);

    if (!label) {
        label = new QLabel(this);
        label->setFont(font);
        label->setGeometry(2*topx+M*(side+border), (2*topy+N*(side+border)-lheight)/2, lwidth, lheight);
    }
    label->setText(to_string(0).c_str());

    if (!pause) {
        pause = new QPushButton(this);
        pause->setFont(font);
        pause->setText("Start");
        pause->setGeometry((2*topx+M*(side+border)-bwidth)/2, 2*topy+N*(side+border), bwidth, bheight);
        pause->setFocusPolicy(Qt::NoFocus);
        connect(pause, SIGNAL(released()), this, SLOT(do_pause()));
    }

    if (!new_game) {
        new_game = new QPushButton(this);
        new_game->setFont(font);
        new_game->setText("New game");
        new_game->setGeometry(pause->x(), pause->y()+bheight+bdist, bwidth, bheight);
        new_game->setFocusPolicy(Qt::NoFocus);
        connect(new_game, SIGNAL(released()), this, SLOT(reset()));
    }
}

void MainWindow::init() {
    for (int i = 0; i < N+4; i++) {
        for (int j = 0; j < M; j++) {
            field[i][j] = 0;
        }
    }
    cur_x = begx;
    cur_y = begy;
    cur_fig = rand()%figures.size();
    score = 0;
    cur_speed = speed;
    if (!timer) {
        timer = new QTimer(this);
        connect(timer, SIGNAL(timeout()), this, SLOT(go()));
    }
    set_gui();
    repaint();
}

bool MainWindow::check(int a, int pos) {
    return (a & (1 << pos)) != 0;
}

int MainWindow::get_num(int x, int y) {
    if (y > N+4 || x > M) {
        return (1 << 16)-1;
    }
    int ans = 0;
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            ans += (field[y+i][x+j] << (4*i+j));
        }
    }
    return ans;
}

bool MainWindow::sift(int dx, int dy) {
    if (cur_x+dx < 0 || cur_y+dy < 0) {
        return false;
    }
    if ((get_num(cur_x+dx, cur_y+dy) & figures[cur_fig]) != 0) {
        return false;
    }
    cur_x += dx; cur_y += dy;
    return true;
}

void MainWindow::freeze() {
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            if (check(figures[cur_fig], 4*i+j)) {
                field[cur_y+i][cur_x+j] = 1;
            }
        }
    }
    int count = 0;
    bool change = true;
    while (change) {
        change = false;
        for (int i = N+3; i > 3; i--) {
            bool ok = true;
            for (int j = 0; j < M; j++) {
                ok &= field[i][j];
            }
            if (ok) {
                ++count;
                cur_speed *= 0.99;
                timer->setInterval(cur_speed);
                change = true;
                for (int k = i-1; k > 3; k--) {
                    field[k+1] = field[k];
                }
            }
        }
    }
    if (count == 1) {
        ++score;
    } else {
        score += 2*count;
    }
}

void MainWindow::the_end() {
    timer->stop();
    string s = "The End!\nYour score is " + to_string(score) + "!\n";
    s += "Max score is " + to_string(max_score) + ".";
    label->setText(s.c_str());
}

void MainWindow::go(int d) {
    if (!sift(dirs[d].first, dirs[d].second)) {
        if (d == 0) {
            freeze();
            cur_x = 5;
            cur_y = 0;
            cur_fig = rand()%figures.size();
        }
    }
    repaint();
}

bool MainWindow::rotate() {
    int fig = cur_fig/4*4 + (cur_fig+1)%4;
    if ((get_num(cur_x, cur_y) & figures[fig]) != 0) {
        return false;
    }
    cur_fig = fig;
    repaint();
    return true;
}

void MainWindow::paintEvent(QPaintEvent *event) {
    QPainter painter(this);
    bool ok = false;
    for (int i = 4; i < N+4; i++) {
        for (int j = 0; j < M; j++) {
            QColor color = QColor(150, 150, 150);
            if (field[i][j]) {
                if (i == 4) {
                    ok = true;
                }
                color = Qt::black;
            } else if (cur_y <= i && i < cur_y+4 &&
                       cur_x <= j && j < cur_x+4) {
                if (check(figures[cur_fig], 4*(i-cur_y)+(j-cur_x))) {
                    color = colors[cur_fig/4];
                }
            }
            painter.setPen(QPen(color, 1, Qt::SolidLine, Qt::FlatCap));
            painter.setBrush(QBrush(color, Qt::SolidPattern));
            painter.drawRect(topy+j*(side+border), topx+(i-4)*(side+border), side, side);
        }
    }    
    max_score = max(max_score, score);
    if (ok) {
        the_end();
    } else {
        string s = "Score: " + to_string(score) + "\n";
        s += "Max: " + to_string(max_score) + "\n";
        s += "Current speed: " +
                to_string(int(double(speed)/double(cur_speed)*100));
        label->setText(s.c_str());
    }
}

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow) {
    srand(time(0));
    ui->setupUi(this);
    setFixedSize(topx+M*(side+border)+topx+lwidth+topx,
                 topy+N*(side+border)+topy+bheight+bdist+bheight+topy);
    init();
    setWindowTitle("Tetris");
}

MainWindow::~MainWindow() {
    delete label;
    delete new_game;
    delete pause;
    delete timer;
    delete ui;
}

void MainWindow::keyPressEvent(QKeyEvent *event) {
    if (timer->isActive()) {
        if (event->key() == Qt::Key_Down) {
            go(0);
        } else if (event->key() == Qt::Key_Left) {
            go(1);
        } else if (event->key() == Qt::Key_Right) {
            go(2);
        } else if (event->key() == Qt::Key_Space) {
            rotate();
        }
    }
}
