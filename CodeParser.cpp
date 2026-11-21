#include "CodeParser.h"

namespace {

// turns a number 0..65535 into exactly 4 uppercase hex digits
QString toHex4(int value)
{
    return QString("%1").arg(value, 4, 16, QLatin1Char('0')).toUpper();
}

bool isHexDigit(QChar c)
{
    return (c >= '0' && c <= '9') || (c >= 'a' && c <= 'f') || (c >= 'A' && c <= 'F');
}

} // namespace

ParseResult CodeParser::parse(const QString& text, CodeFormat format)
{
    ParseResult result;
    const QStringList lines = text.split('\n');

    for (int i = 0; i < lines.size(); i++) {
        const int lineNumber = i + 1;
        QString line = lines[i];

        // everything after ';' is a comment
        const int comment = line.indexOf(';');
        if (comment >= 0)
            line = line.left(comment);
        line = line.trimmed();
        if (line.isEmpty())
            continue;

        if (format == CodeFormat::Hex) {
            // same rule as ProgramLoader: the first four characters must be hex
            if (line.size() < 4) {
                result.error = QString("Line %1: each instruction needs 4 hex digits.").arg(lineNumber);
                return result;
            }
            const QString first4 = line.left(4);
            for (QChar c : first4) {
                if (!isHexDigit(c)) {
                    result.error = QString("Line %1: \"%2\" is not a valid hex instruction.")
                                       .arg(lineNumber).arg(first4);
                    return result;
                }
            }
            result.hexLines << first4.toUpper();
        }
        else if (format == CodeFormat::Binary) {
            line.remove(' ');
            bool onlyBits = true;
            for (QChar c : line)
                if (c != '0' && c != '1')
                    onlyBits = false;

            if (!onlyBits || line.size() != 16) {
                result.error = QString("Line %1: a binary instruction needs exactly 16 bits (0 or 1).")
                                   .arg(lineNumber);
                return result;
            }
            result.hexLines << toHex4(line.toInt(nullptr, 2));
        }
        else { // Decimal
            bool ok = false;
            const int value = line.toInt(&ok, 10);
            if (!ok || value < 0 || value > 65535) {
                result.error = QString("Line %1: a decimal instruction must be a number from 0 to 65535.")
                                   .arg(lineNumber);
                return result;
            }
            result.hexLines << toHex4(value);
        }
    }

    if (result.hexLines.isEmpty()) {
        result.error = "Write at least one instruction first.";
        return result;
    }

    result.ok = true;
    return result;
}
