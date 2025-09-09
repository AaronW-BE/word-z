#include "wordzapplication.h"

WordZApplication::WordZApplication(int& argc, char** argv) : QApplication(argc, argv)
{
	initSettings();

	QString lockPath = QDir::temp().absoluteFilePath("WordZ.lock");
	m_lockFile.reset(new QLockFile(lockPath));
	m_lockFile->setStaleLockTime(0);
	if (!m_lockFile->tryLock()) {
		qWarning() << "程序已在运行中!";
		m_isRunning = true;
	}
}

WordZApplication::~WordZApplication()
{}

void WordZApplication::initSettings()
{
	QCoreApplication::setOrganizationName("BMTeam");
	QCoreApplication::setOrganizationDomain("github.com/AaronW-BE");
	QCoreApplication::setApplicationName("WordZ");
	m_settings = new QSettings("BMTeam", "WordZ", this);

	qDebug() << "config file path:" << m_settings->fileName();
}

