#ifndef CODEEDITOR_H
#define CODEEDITOR_H

#include <QPlainTextEdit>

/*
 Class responsibility:
 The "Machine code" box of the prototype: a text editor that shows a
 line-number gutter on the left (01, 02, 03 ...).
 */

class CodeEditor : public QPlainTextEdit
{
    Q_OBJECT

public:
    explicit CodeEditor(QWidget* parent = nullptr);

    int lineNumberAreaWidth() const;
    void lineNumberAreaPaintEvent(QPaintEvent* event);

protected:
    void resizeEvent(QResizeEvent* event) override;

private slots:
    void updateLineNumberAreaWidth();
    void updateLineNumberArea(const QRect& rect, int dy);

private:
    QWidget* lineNumberArea;

};

#endif // CODEEDITOR_H
