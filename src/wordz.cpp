#include "stdafx.h"
#include "wordz.h"

#ifdef Q_OS_WIN
#include <windows.h>
#endif

WordZ::WordZ(QWidget *parent)
    : QWidget(parent), dictionary(".\\db\\eng-zho.db")
{
    QPoint lastPos = settings().value("window/pos").toPoint();
    if (!lastPos.isNull()) {
        move(lastPos);
    }

    ui.setupUi(this);
    setWindowFlags(Qt::FramelessWindowHint | Qt::Tool | Qt::WindowStaysOnTopHint);
    setAttribute(Qt::WA_TranslucentBackground);

    this->adjustSize();

#ifdef Q_OS_WIN
    // 设置点透，让鼠标事件穿透
    HWND hwnd = (HWND)winId();
    LONG exStyle = GetWindowLong(hwnd, GWL_EXSTYLE);
    SetWindowLong(hwnd, GWL_EXSTYLE, exStyle | WS_EX_LAYERED | WS_EX_TRANSPARENT);

    if (!RegisterHotKey(hwnd, 1, MOD_CONTROL | MOD_ALT, 'N')) { // 0x4E = 'N'
        qDebug() << "注册全局热键失败 CTRL+ALT+N";
    }

    if (!RegisterHotKey(hwnd, 2, MOD_CONTROL | MOD_ALT, 'M')) {
        qDebug() << "注册全局热键失败 CTRL+ALT+M";
    }
#endif

    connect(&wordTimer, &QTimer::timeout, this, &WordZ::showNextWord);
    // per 30s
    wordTimer.start(30 * 1000);
    showNextWord();

    initSysTray();

}

WordZ::~WordZ()
{
    settings().setValue("window/pos", this->pos());
#ifdef Q_OS_WIN
    HWND hwnd = (HWND)winId();
    UnregisterHotKey(hwnd, 1);
    UnregisterHotKey(hwnd, 2);
#endif
}


void WordZ::nextWord()
{

}

void WordZ::showNextWord()
{
    currentWord = dictionary.randomWord();
    ui.wordLabel->setText(currentWord.word);
    if (currentWord.pron.isEmpty()) {
        ui.pronLabel->setText("-");
    }
    else {
        auto pronList = currentWord.pron.split(";");
        ui.pronLabel->setText(pronList[0]);
    }

    ui.translationsLabel->setText(currentWord.translations);
    //ui.definitionLabel->setText(currentWord.definition);
}

QSettings WordZ::settings()
{
    return QSettings("BMTeam", "WordZ");;
}

void WordZ::initSysTray()
{
    qDebug() << "init system tray icon";
    trayIcon = new QSystemTrayIcon(this);
    QIcon icon(":/icons/app.png");
    if (icon.isNull()) {
        qDebug() << "托盘图标加载失败";
    }
    else {
        trayIcon->setIcon(icon);
        trayIcon->setToolTip("WordZ");
        trayIcon->show();
    }

    QMenu* trayMenu = new QMenu(this);
    QAction* settingsAction = new QAction("设置", this);
    QAction* exitAction = new QAction("退出", this);

    connect(settingsAction, &QAction::triggered, this, []() {});
    connect(exitAction, &QAction::triggered, qApp, &QCoreApplication::quit);

    trayMenu->addAction(settingsAction);
    trayMenu->addSeparator();
    trayMenu->addAction(exitAction);

    trayIcon->setContextMenu(trayMenu);
    trayIcon->show();

    connect(trayIcon, &QSystemTrayIcon::activated, this, [this](QSystemTrayIcon::ActivationReason reason) {
        if (reason == QSystemTrayIcon::Trigger) {
            this->setVisible(!this->isVisible()); // 左键点击显示/隐藏
        }
    });
}

bool WordZ::nativeEvent(const QByteArray& eventType, void* message, qintptr* result)
{
#ifdef Q_OS_WIN
    MSG* msg = static_cast<MSG*>(message);
    if (msg->message == WM_HOTKEY) {
        if (msg->wParam == 1) { // 热键 ID 1
            showNextWord();
			wordTimer.start(30 * 1000); // 重置计时器
        }
        else if (msg->wParam == 2) { // 热键 ID 2
            m_dragEnabled = !m_dragEnabled; // 切换拖动模式

            HWND hwnd = (HWND)winId();
            LONG exStyle = GetWindowLong(hwnd, GWL_EXSTYLE);
            if (m_dragEnabled) {
                // 激活窗口
                if (GetForegroundWindow() != hwnd) {
                    ShowWindow(hwnd, SW_RESTORE);
                    SetForegroundWindow(hwnd);
                }

                // 关闭穿透，允许交互
                SetWindowLong(hwnd, GWL_EXSTYLE, (exStyle & ~WS_EX_TRANSPARENT) | WS_EX_LAYERED); // 保留 WS_EX_LAYERED
                setAttribute(Qt::WA_NoSystemBackground, false);
                setAttribute(Qt::WA_TranslucentBackground, false);

                QPalette pal = palette();
                pal.setColor(QPalette::Window, QColor(180, 180, 180, 217)); // 0.85*255 ≈ 217
                setAutoFillBackground(true);
                setPalette(pal);
                setCursor(Qt::SizeAllCursor);
                qDebug() << "拖动模式开启，可用鼠标拖动窗口";
                update();
            }
            else {
                // 开启穿透，不能交互
                SetWindowLong(hwnd, GWL_EXSTYLE, exStyle | WS_EX_TRANSPARENT | WS_EX_LAYERED);
                setAttribute(Qt::WA_TranslucentBackground);

                settings().setValue("window/pos", this->pos());
				setPalette(QPalette()); // 恢复默认调色板
                setCursor(Qt::ArrowCursor);
                qDebug() << "拖动模式关闭，窗口恢复点透";
                update();
            }
        }
        *result = 0;
        return true;
    }
    else if (msg->message == WM_ACTIVATE) {
        if (LOWORD(msg->wParam) == WA_INACTIVE) {
            qDebug() << "Window deactived";

            m_dragEnabled = !m_dragEnabled;
            HWND hwnd = (HWND)winId();
            LONG exStyle = GetWindowLong(hwnd, GWL_EXSTYLE);

            // 开启穿透，不能交互
            SetWindowLong(hwnd, GWL_EXSTYLE, exStyle | WS_EX_TRANSPARENT | WS_EX_LAYERED);
            setAttribute(Qt::WA_TranslucentBackground);

            settings().setValue("window/pos", this->pos());
            setPalette(QPalette());
            setCursor(Qt::ArrowCursor);
            qDebug() << "Disable move window";
            update();
        }
    }
#endif
    return QWidget::nativeEvent(eventType, message, result);
}


void WordZ::mousePressEvent(QMouseEvent* event)
{
    if (m_dragEnabled && event->button() == Qt::LeftButton) {
        m_dragging = true;
        m_dragStartPos = event->globalPos() - frameGeometry().topLeft();
    }
    QWidget::mousePressEvent(event);
}

void WordZ::mouseMoveEvent(QMouseEvent* event)
{
    if (m_dragging && (event->buttons() & Qt::LeftButton)) {
        QPoint newPos = event->globalPos() - m_dragStartPos;
        QRect screenRect = screen()->availableGeometry();
        QSize winSize = size();

        if (newPos.x() < screenRect.left())
            newPos.setX(screenRect.left());
        else if (newPos.x() + winSize.width() > screenRect.right())
            newPos.setX(screenRect.right() - winSize.width());

        // 限制 Y 轴
        if (newPos.y() < screenRect.top())
            newPos.setY(screenRect.top());
        else if (newPos.y() + winSize.height() > screenRect.bottom())
            newPos.setY(screenRect.bottom() - winSize.height());

        move(newPos);
    }
    QWidget::mouseMoveEvent(event);
}

void WordZ::mouseReleaseEvent(QMouseEvent* event)
{
    if (event->button() == Qt::LeftButton) {
        m_dragging = false;
    }
    QWidget::mouseReleaseEvent(event);
}


