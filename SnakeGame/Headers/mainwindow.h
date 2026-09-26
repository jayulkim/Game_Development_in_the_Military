#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTimer>
#include <QVector>
#include <QPoint>
#include <QKeyEvent>

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

protected:
    void paintEvent(QPaintEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;

private:
    static constexpr int CELL_SIZE = 20;
    static constexpr int BOARD_WIDTH = 50;
    static constexpr int BOARD_HEIGHT = 40;

    bool gameOver = false;

    Ui::MainWindow *ui;

    QVector<QPoint> snake;
    QTimer *timer;

    QPoint direction;
    QPoint food;

    int score = 0;

    int gameSpeed = 100;

    bool directionChanged = false;

    void resetGame();
    void moveSnake();
    void spawnFood();
};

#endif // MAINWINDOW_H
