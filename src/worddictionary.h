#pragma once
#include <QVector>
#include <QString>
#include <QSqlDatabase>

struct WordItem {
    int id;
    QString word;
    QString pron;
    QString pos;
    QString definition;
	QString translations;
    QString example;
};

class WordDictionary
{
public:
    WordDictionary(const QString &dbPath = "words.db");

    ~WordDictionary();

    void importFromJson(const QString& filePath);

    bool isOpen() const;

    bool insertWord(const WordItem &item) const;

    QVector<WordItem> loadAllWords() const;

    WordItem randomWord() const;

    bool markLearned(int id);

private:
    QSqlDatabase db;
};

