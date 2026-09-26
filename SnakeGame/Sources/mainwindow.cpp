#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QPainter>

#include <QTimer>
#include <QVector>
#include <QPoint>

#include <QKeyEvent>

#include <cstdlib>
#include <ctime>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    srand(static_cast<unsigned int>(time(nullptr)));

    setFixedSize(
        BOARD_WIDTH * CELL_SIZE,
        BOARD_HEIGHT * CELL_SIZE
    );

    // 뱀의 처음 위치
    snake.append(QPoint(5, 5));
    snake.append(QPoint(4, 5));
    snake.append(QPoint(3, 5));

    // 처음 이동 방향
    direction = QPoint(1, 0);

    spawnFood();

    // 타이머 생성
    timer = new QTimer(this);

    connect(timer, &QTimer::timeout,
            this, &MainWindow::moveSnake);

    // 200ms마다 뱀 이동
    timer->start(gameSpeed);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);

    painter.fillRect(rect(), Qt::black);

    painter.setBrush(Qt::green);

    for (const QPoint &part : snake)
    {
        painter.drawRect(
            part.x() * 20,
            part.y() * 20,
            20,
            20
        );
    }

    // 먹이 그리기
    painter.setBrush(Qt::red);

    painter.drawEllipse(
        food.x() * CELL_SIZE,
        food.y() * CELL_SIZE,
        CELL_SIZE,
        CELL_SIZE
    );

    // 점수 표시
    painter.setPen(Qt::white);

    QFont scoreFont = painter.font();
    scoreFont.setPointSize(16);
    painter.setFont(scoreFont);

    painter.drawText(
        20,
        30,
        "Score: " + QString::number(score)
    );
    if (gameSpeed == 5) {
        painter.drawText(
            20,
            55,
            "Max Speed"
        );
    }
    else {
        painter.drawText(
            20,
            55,
            "Speed: " + QString::number(100 - gameSpeed)
        );
    }

    if (gameOver)
    {
        painter.setPen(Qt::white);

        QFont font = painter.font();
        font.setPointSize(30);
        painter.setFont(font);

        painter.drawText(
            0, 300,
            width(), 100,
            Qt::AlignCenter,
            "GAME OVER"
        );

        font.setPointSize(16);
        painter.setFont(font);

        painter.drawText(
            0, 400,
            width(), 50,
            Qt::AlignCenter,
            "Press R to Restart"
        );
    }
}

void MainWindow::moveSnake()
{
    if (gameOver)
        return;

    QPoint newHead = snake[0] + direction;

    // 벽 충돌 검사
    if (newHead.x() < 0 ||
        newHead.x() >= BOARD_WIDTH ||
        newHead.y() < 0 ||
        newHead.y() >= BOARD_HEIGHT)
    {
        gameOver = true;
        timer->stop();
        update();
        return;
    }

    // 자기 몸과 충돌 검사
    if (snake.contains(newHead))
    {
        gameOver = true;
        timer->stop();
        update();
        return;
    }

    snake.prepend(newHead);

    if (newHead == food)
    {
        score += 10;

        // 먹이를 먹을 때마다 조금씩 빨라짐
        if (gameSpeed > 5)
        {
            gameSpeed -= 5;
            timer->setInterval(gameSpeed);
        }

        spawnFood();
    }
    else
    {
        // 먹지 않았으면 평소처럼 꼬리 제거
        snake.removeLast();
    }

    directionChanged = false;

    update();
}

void MainWindow::keyPressEvent(QKeyEvent *event)
{
    // 게임오버 상태에서 R키
    if (gameOver && event->key() == Qt::Key_R)
    {
        resetGame();
        return;
    }

    // 한 번 이동하기 전에 이미 방향을 변경했다면
    // 추가 방향 입력 무시
    if (directionChanged)
        return;

    if (event->key() == Qt::Key_Up &&
        direction != QPoint(0, 1))
    {
        direction = QPoint(0, -1);
        directionChanged = true;
    }
    else if (event->key() == Qt::Key_Down &&
             direction != QPoint(0, -1))
    {
        direction = QPoint(0, 1);
        directionChanged = true;
    }
    else if (event->key() == Qt::Key_Left &&
             direction != QPoint(1, 0))
    {
        direction = QPoint(-1, 0);
        directionChanged = true;
    }
    else if (event->key() == Qt::Key_Right &&
             direction != QPoint(-1, 0))
    {
        direction = QPoint(1, 0);
        directionChanged = true;
    }
}

void MainWindow::spawnFood()
{
    do
    {
        int x = rand() % (BOARD_WIDTH - 2) + 1;
        int y = rand() % (BOARD_HEIGHT - 2) + 1;

        food = QPoint(x, y);

    } while (snake.contains(food));
}

void MainWindow::resetGame()
{
    // 기존 뱀 제거
    snake.clear();

    // 처음 위치로 다시 생성
    snake.append(QPoint(5, 5));
    snake.append(QPoint(4, 5));
    snake.append(QPoint(3, 5));

    // 오른쪽으로 시작
    direction = QPoint(1, 0);

    // 점수 초기화
    score = 0;

    gameSpeed = 100;

    // 게임오버 해제
    gameOver = false;

    // 새로운 먹이 생성
    spawnFood();

    // 타이머 다시 시작
    timer->start(gameSpeed);

    directionChanged = false;

    // 화면 다시 그리기
    update();
}
