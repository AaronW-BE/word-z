#include "worddictionary.h"
#include <QSqlQuery>
#include <qsqlerror.h>
#include <qfileinfo.h>

WordDictionary::WordDictionary(const QString& dbPath)
{
    // 检查驱动是否可用
    if (!QSqlDatabase::isDriverAvailable("QSQLITE")) {
		qDebug() << "SQLite driver is not available.";
		return;
	}

    QFileInfo info(dbPath);
    if (!info.path().isEmpty() && info.path() != ".") {
        QDir dir;
        if (!dir.exists(info.path())) {
            dir.mkpath(info.path());
        }
    }


	db = QSqlDatabase::addDatabase("QSQLITE");
	db.setDatabaseName(dbPath);
    if (!db.open()) {
        qDebug() << "Cannot open database:" << db.lastError().text();
    }
}

WordDictionary::~WordDictionary()
{
    if (db.isOpen()) db.close();
}

void WordDictionary::importFromJson(const QString& filePath)
{
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly)) {
        qDebug() << "Cannot open JSON file:" << file.errorString();
        return;
    }

    QByteArray data = file.readAll();
    QJsonDocument doc = QJsonDocument::fromJson(data);
    if (!doc.isArray()) return;

    QJsonArray arr = doc.array();
    for (const QJsonValue& v : arr) {
        QJsonObject obj = v.toObject();
        WordItem item;
        item.word = obj["word"].toString();
        item.definition = obj["definition"].toString();
        item.example = obj["example"].toString();
        insertWord(item);
    }
}

bool WordDictionary::isOpen() const
{
    return db.isOpen();
}

bool WordDictionary::insertWord(const WordItem& item) const
{
    QSqlQuery query;
    query.prepare("INSERT INTO dictionary (word, definition, example) VALUES (?, ?, ?)");
    query.addBindValue(item.word);
    query.addBindValue(item.definition);
    query.addBindValue(item.example);
    return query.exec();
}

QVector<WordItem> WordDictionary::loadAllWords() const
{
    QVector<WordItem> words;
    QSqlQuery query("SELECT id, word, definition, example FROM dictionary");
    while (query.next()) {
        words.push_back({ query.value(0).toInt(),
                         query.value(1).toString(),
                         query.value(2).toString(),
                         query.value(3).toString() });
    }
    return words;
}

WordItem WordDictionary::randomWord() const
{
    //QVector<WordItem> all = loadAllWords();
    QSqlQuery query("SELECT id, word, pron, pos, translations, definition "
                        "FROM dictionary ORDER BY RANDOM() LIMIT 1");
    if (query.next())
    {
        return {
            query.value(0).toInt(),
            query.value(1).toString().trimmed(),
            query.value(2).toString().trimmed(),
            query.value(3).toString().trimmed(),
            query.value(5).toString().trimmed(),
            query.value(4).toString().trimmed(),

        };
    }

    return {};
}

bool WordDictionary::markLearned(int id)
{
    QSqlQuery query;
    query.prepare("UPDATE dictionary SET learned = 1 WHERE id = ?");
    query.addBindValue(id);
    return query.exec();
}
