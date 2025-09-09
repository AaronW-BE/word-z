#pragma once

#include <QtWidgets/QWidget>
#include "ui_wordz.h"
#include "worddictionary.h"
#include <QTimer>

class WordZ : public QWidget
{
    Q_OBJECT

public:
    WordZ(QWidget *parent = nullptr);
    ~WordZ();

private:
    bool m_dragEnabled = false;
    bool m_dragging = false;
    QPoint m_dragStartPos;

	QSystemTrayIcon* trayIcon;

    Ui::WordZClass ui;
    WordDictionary dictionary;
    WordItem currentWord;
    QTimer wordTimer;

    void nextWord();

    void showNextWord();

    QSettings settings();

    void initSysTray();

protected:
    bool nativeEvent(const QByteArray& eventType, void* message, qintptr* result) override;

	void mousePressEvent(QMouseEvent* event) override;
	void mouseMoveEvent(QMouseEvent* event) override;
	void mouseReleaseEvent(QMouseEvent* event) override;

};

