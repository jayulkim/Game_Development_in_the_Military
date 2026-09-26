#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QPainter>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    setFixedSize(GAME_WIDTH, GAME_HEIGHT);

    createBricks();

    setLevelTime();

    timer = new QTimer(this);

    connect(
        timer,
        &QTimer::timeout,
        this,
        &MainWindow::gameLoop
    );

    timer->start(16);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);

    // 검은색 배경
    painter.fillRect(rect(), Qt::black);

    // 패들
    painter.setBrush(Qt::white);

    painter.drawRect(
        paddleX,
        paddleY,
        PADDLE_WIDTH,
        PADDLE_HEIGHT
    );

    // 공
    painter.setBrush(Qt::red);

    painter.drawEllipse(
        ballX,
        ballY,
        BALL_SIZE,
        BALL_SIZE
    );

    // 벽돌
    painter.setBrush(Qt::blue);
    painter.setPen(Qt::white);

    for (const QRect &brick : bricks)
    {
        painter.drawRect(brick);
    }

    // 점수
    painter.setPen(Qt::white);

    painter.drawText(
        GAME_WIDTH - 120,
        30,
        "Lives: " + QString::number(lives)
    );

    QFont font = painter.font();
    font.setPointSize(16);
    painter.setFont(font);

    painter.drawText(
        20,
        30,
        "Score: " + QString::number(score)
    );

    painter.drawText(
        GAME_WIDTH / 2 - 40,
        30,
        "Level: " + QString::number(level)
    );

    if (gameOver)
    {
        QFont gameOverFont = painter.font();
        gameOverFont.setPointSize(32);
        painter.setFont(gameOverFont);

        painter.setPen(Qt::white);

        painter.drawText(
            rect(),
            Qt::AlignCenter,
            "GAME OVER\nPress R to Restart"
        );
    }

    if (gameWon)
    {
        QFont winFont = painter.font();
        winFont.setPointSize(32);
        painter.setFont(winFont);

        painter.setPen(Qt::white);

        painter.drawText(
            rect(),
            Qt::AlignCenter,
            "YOU WIN!\nPress R to Restart"
        );
    }

    int minutes = timeLeft / 60;
    int seconds = timeLeft % 60;

    QString timeText = QString("Time: %1:%2")
                           .arg(minutes, 2, 10, QChar('0'))
                           .arg(seconds, 2, 10, QChar('0'));

    painter.drawText(
        GAME_WIDTH / 2 - 60,
        55,
        timeText
    );
}

void MainWindow::keyPressEvent(QKeyEvent *event)
{
    if ((gameOver || gameWon) &&
        event->key() == Qt::Key_R)
    {
        resetGame();
        return;
    }
    if (event->key() == Qt::Key_Left)
    {
        leftPressed = true;
    }
    else if (event->key() == Qt::Key_Right)
    {
        rightPressed = true;
    }
}

void MainWindow::keyReleaseEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_Left)
    {
        leftPressed = false;
    }
    else if (event->key() == Qt::Key_Right)
    {
        rightPressed = false;
    }
}

void MainWindow::gameLoop()
{
    frameCount++;

    if (frameCount >= 62)
    {
        frameCount = 0;
        timeLeft--;

        if (timeLeft <= 0)
        {
            timeLeft = 0;
            gameOver = true;
            timer->stop();

            update();
            return;
        }
    }

    // 패들 이동
    if (leftPressed)
    {
        paddleX -= PADDLE_SPEED;
    }

    if (rightPressed)
    {
        paddleX += PADDLE_SPEED;
    }

    // 왼쪽 벽 제한
    if (paddleX < 0)
    {
        paddleX = 0;
    }

    // 오른쪽 벽 제한
    if (paddleX + PADDLE_WIDTH > GAME_WIDTH)
    {
        paddleX = GAME_WIDTH - PADDLE_WIDTH;
    }

    ballX += ballDX;
    ballY += ballDY;

    // 왼쪽 벽
    if (ballX <= 0)
    {
        ballX = 0;
        ballDX = -ballDX;
    }

    // 오른쪽 벽
    if (ballX + BALL_SIZE >= GAME_WIDTH)
    {
        ballX = GAME_WIDTH - BALL_SIZE;
        ballDX = -ballDX;
    }

    // 위쪽 벽
    if (ballY <= 0)
    {
        ballY = 0;
        ballDY = -ballDY;
    }

    // 공과 패들 충돌
    if (ballY + BALL_SIZE >= paddleY &&
        ballY + BALL_SIZE <= paddleY + PADDLE_HEIGHT &&
        ballX + BALL_SIZE >= paddleX &&
        ballX <= paddleX + PADDLE_WIDTH &&
        ballDY > 0)
    {
        // 공을 패들 위로 이동
        ballY = paddleY - BALL_SIZE;

        // 공의 중심 위치
        int ballCenterX = ballX + BALL_SIZE / 2;

        // 패들의 중심 위치
        int paddleCenterX = paddleX + PADDLE_WIDTH / 2;

        // 패들 중심에서 공이 얼마나 떨어져 있는지
        int hitPosition = ballCenterX - paddleCenterX;

        // 맞은 위치에 따라 X 방향 변경
        ballDX = hitPosition / 15;

        // 너무 수직으로만 올라가는 것을 방지
        if (ballDX >= 0 && ballDX < 2)
            ballDX = 2;

        if (ballDX < 0 && ballDX > -2)
            ballDX = -2;

        // 위쪽으로 튕김
        ballDY = -qAbs(ballDY);
    }

    QRect ballRect(
        ballX,
        ballY,
        BALL_SIZE,
        BALL_SIZE
    );

    for (int i = 0; i < bricks.size(); i++)
    {
        if (ballRect.intersects(bricks[i]))
        {
            QRect brick = bricks[i];

            // 공 중심
            int ballCenterX = ballX + BALL_SIZE / 2;
            int ballCenterY = ballY + BALL_SIZE / 2;

            // 벽돌 중심
            int brickCenterX = brick.x() + brick.width() / 2;
            int brickCenterY = brick.y() + brick.height() / 2;

            // 중심 사이의 거리
            int dx = ballCenterX - brickCenterX;
            int dy = ballCenterY - brickCenterY;

            // 충돌 방향 판정
            if (qAbs(dx) * brick.height() >
                qAbs(dy) * brick.width())
            {
                // 왼쪽 또는 오른쪽 충돌
                ballDX = -ballDX;
            }
            else
            {
                // 위쪽 또는 아래쪽 충돌
                ballDY = -ballDY;
            }

            // 벽돌 제거
            bricks.remove(i);

            // 점수 증가
            score += 10;

            // 모든 벽돌을 깼는지 확인
            if (bricks.isEmpty())
            {
                if (level >= MAX_LEVEL)
                {
                    gameWon = true;
                    timer->stop();

                    update();
                    return;
                }
                else
                {
                    nextLevel();
                    return;
                }
            }
            break;
        }
    }

    // 공을 아래로 놓쳤을 때
    if (ballY > GAME_HEIGHT)
    {
        lives--;

        if (lives <= 0)
        {
            gameOver = true;
            timer->stop();
        }
        else
        {
            resetBall();
        }

        update();
        return;
    }

    update();
}

void MainWindow::createBricks()
{
    bricks.clear();

    int startY = 70;

    // =========================
    // LEVEL 1 - 기본 직사각형
    // =========================
    if (level == 1)
    {
        int rows = 5;
        int columns = 8;
        int startX = 65;

        for (int row = 0; row < rows; row++)
        {
            for (int col = 0; col < columns; col++)
            {
                int x = startX +
                        col * (BRICK_WIDTH + BRICK_GAP);

                int y = startY +
                        row * (BRICK_HEIGHT + BRICK_GAP);

                bricks.append(
                    QRect(
                        x,
                        y,
                        BRICK_WIDTH,
                        BRICK_HEIGHT
                    )
                );
            }
        }
    }

    // =========================
    // LEVEL 2 - 피라미드
    // =========================
    else if (level == 2)
    {
        int rows = 6;

        for (int row = 0; row < rows; row++)
        {
            int brickCount = row + 3;

            int totalWidth =
                brickCount * BRICK_WIDTH +
                (brickCount - 1) * BRICK_GAP;

            int startX =
                (GAME_WIDTH - totalWidth) / 2;

            for (int col = 0; col < brickCount; col++)
            {
                int x = startX +
                        col * (BRICK_WIDTH + BRICK_GAP);

                int y = startY +
                        row * (BRICK_HEIGHT + BRICK_GAP);

                bricks.append(
                    QRect(
                        x,
                        y,
                        BRICK_WIDTH,
                        BRICK_HEIGHT
                    )
                );
            }
        }
    }

    // =========================
    // LEVEL 3 - 체크무늬
    // =========================
    else if (level == 3)
    {
        int rows = 7;
        int columns = 8;
        int startX = 65;

        for (int row = 0; row < rows; row++)
        {
            for (int col = 0; col < columns; col++)
            {
                // 한 칸씩 건너뛰어서 배치
                if ((row + col) % 2 == 0)
                {
                    int x = startX +
                            col * (BRICK_WIDTH + BRICK_GAP);

                    int y = startY +
                            row * (BRICK_HEIGHT + BRICK_GAP);

                    bricks.append(
                        QRect(
                            x,
                            y,
                            BRICK_WIDTH,
                            BRICK_HEIGHT
                        )
                    );
                }
            }
        }
    }
}

void MainWindow::resetBall()
{
    // 공을 화면 중앙으로
    ballX = GAME_WIDTH / 2 - BALL_SIZE / 2;
    ballY = GAME_HEIGHT / 2;

    // 다시 위쪽으로 출발
    int speed = 3 + level;

    ballDX = speed;
    ballDY = -speed;

    // 패들도 가운데로
    paddleX = GAME_WIDTH / 2 - PADDLE_WIDTH / 2;
}

void MainWindow::resetGame()
{
    score = 0;
    lives = 3;
    level = 1;

    gameOver = false;
    gameWon = false;

    leftPressed = false;
    rightPressed = false;

    createBricks();
    resetBall();
    setLevelTime();

    timer->start(16);

    update();
}

void MainWindow::nextLevel()
{
    level++;

    createBricks();
    resetBall();
    setLevelTime();

    update();
}

void MainWindow::setLevelTime()
{
    if (level == 1)
    {
        timeLeft = 180;   // 3분
    }
    else if (level == 2)
    {
        timeLeft = 60;    // 1분
    }
    else if (level == 3)
    {
        timeLeft = 30;    // 30초
    }

    frameCount = 0;
}
