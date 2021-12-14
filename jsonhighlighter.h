#ifndef JSONHIGHLIGHTER_H
#define JSONHIGHLIGHTER_H

#include <QRegularExpression>
#include <QSyntaxHighlighter>
#include <QTextCharFormat>



class JsonHighlighter : public QSyntaxHighlighter {
public:
    JsonHighlighter(QTextDocument *parent = nullptr);

protected:
    void highlightBlock(const QString &text) override;

private:
    struct  HighlightingRule  {
        QRegularExpression pattern;
        QTextCharFormat format;
        int startOffset = 0;
        int lengthReduce = 0;
    };
    QVector<HighlightingRule> highlightingRules;

    QList<QString> keywords = {"timestamp", "logger", "level", "app-version", "service-name", "message", "func-name", "dev-message",
                               "exception", "thread", "threadId", "request", "username", "employeeId", "sessionId", "requestURI",
                               "requestQuery", "graphqlFunction"};
    QList<QString> logLevels = {"TRACE", "DEBUG", "INFO", "WARN", "ERROR", "FATAL"};
};

#endif // JSONHIGHLIGHTER_H
