#ifndef CODEPARSER_H
#define CODEPARSER_H

#include <QString>
#include <QStringList>

/*
 Class responsibility:
 Reads the text the user typed in the "Machine code" box and converts every
 instruction to a 4-digit hex line, which is the only format ProgramLoader
 understands. It supports the three formats of the prototype:
   Binary  -> 16 bits per line   (spaces inside the line are allowed)
   Decimal -> a number 0..65535 per line
   Hex     -> 4 hex digits per line (the rest of the line is ignored, like ProgramLoader)
 Empty lines and everything after a ';' are ignored.
 */

enum class CodeFormat { Binary, Decimal, Hex };

struct ParseResult {
    bool ok = false;
    QStringList hexLines;   // one 4-digit uppercase hex instruction per entry
    QString error;          // filled when ok == false
};

class CodeParser {
public:
    static ParseResult parse(const QString& text, CodeFormat format);
};

#endif // CODEPARSER_H
