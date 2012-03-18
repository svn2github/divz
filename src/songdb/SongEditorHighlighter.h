#ifndef SongEditorHighlighter_H
#define SongEditorHighlighter_H

#include <QSyntaxHighlighter>
#include <QHash>
#include <QTextCharFormat>
#include <QTextDocument>
#include <QLineEdit>

class SongEditorHighlighter : public QSyntaxHighlighter
{
    Q_OBJECT

public:
	SongEditorHighlighter(QTextDocument *parent = 0);

protected:
	void highlightBlock(const QString &text);

private:
	struct HighlightingRule
	{
		QRegExp pattern;
		QTextCharFormat format;
	};
	QVector<HighlightingRule> highlightingRules;
	
	QTextCharFormat rearFormat;
	QTextCharFormat tagFormat;
};


#endif
