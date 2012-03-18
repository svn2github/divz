#include "SongSlideGroup.h"
#include "SongEditorHighlighter.h"

SongEditorHighlighter::SongEditorHighlighter(QTextDocument *parent)
    : QSyntaxHighlighter(parent)
{
	HighlightingRule rule;
				
	tagFormat.setFontWeight(QFont::Bold);
	tagFormat.setForeground(Qt::white);
	tagFormat.setBackground(Qt::red);
	rule.pattern = QRegExp(tr("^\\s*(%1)(\\s+\\d+)?(\\s*\\(.*\\))?\\s*.*$").arg(SongSlideGroup::songTagRegexpList()));
	rule.format = tagFormat;
	highlightingRules.append(rule);
	
	rearFormat.setFontWeight(QFont::Bold);
	rearFormat.setForeground(Qt::white);
	rearFormat.setBackground(Qt::blue);
	rule.pattern = QRegExp("^\\s*((?:B:|R:|C:|T:|G:|\\[|\\|).*).*$");
	rule.format = rearFormat;
	highlightingRules.append(rule);
}

void SongEditorHighlighter::highlightBlock(const QString &text)
{
	foreach (const HighlightingRule &rule, highlightingRules) 
	{
		QRegExp expression(rule.pattern);
		int index = expression.indexIn(text);
		while (index >= 0) 
		{
			int length = expression.matchedLength();
			setFormat(index, length, rule.format);
			index = expression.indexIn(text, index + length);
		}
	}
}

