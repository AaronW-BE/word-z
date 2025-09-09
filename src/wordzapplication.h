#pragma once

#include <QApplication>

class WordZApplication  : public QApplication
{
	Q_OBJECT

public:
	WordZApplication(int& argc, char** argv);
	~WordZApplication();

	QSettings* settings() const { return m_settings; }

	bool isRunning() const { return m_isRunning; }

private:
	QSettings* m_settings = nullptr;

	QScopedPointer<QLockFile> m_lockFile;
	bool m_isRunning = false;

	void initSettings();
};

