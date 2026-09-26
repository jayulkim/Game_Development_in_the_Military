#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include <QKeyEvent>

#include <QTimer>

#include <QVector>
#include <QRect>

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
    void keyReleaseEvent(QKeyEvent *event) override;

private:
    Ui::MainWindow *ui;

    int level = 1;

    static constexpr int MAX_LEVEL = 3;

    void nextLevel();

    int timeLeft = 180;
    int frameCount = 0;

    void setLevelTime();

    int lives = 3;
    bool gameOver = false;

    void resetBall();
    void resetGame();

    bool gameWon = false;

    // 벽돌
    QVector<QRect> bricks;

    static constexpr int BRICK_WIDTH = 100;
    static constexpr int BRICK_HEIGHT = 30;
    static constexpr int BRICK_GAP = 10;

    void createBricks();

    bool leftPressed = false;
    bool rightPressed = false;

    // 게임 화면 크기
    static constexpr int GAME_WIDTH = 1000;
    static constexpr int GAME_HEIGHT = 800;

    // 패들
    int paddleX = 400;
    int paddleY = 740;

    static constexpr int PADDLE_WIDTH = 200;
    static constexpr int PADDLE_HEIGHT = 20;
    static constexpr int PADDLE_SPEED = 8;

    // 공
    int ballX = 490;
    int ballY = 400;

    static constexpr int BALL_SIZE = 20;

    int ballDX = 4;
    int ballDY = -4;

    int score = 0;

    // 게임 타이머
    QTimer *timer;

    void gameLoop();
};

#endif // MAINWINDOW_H
