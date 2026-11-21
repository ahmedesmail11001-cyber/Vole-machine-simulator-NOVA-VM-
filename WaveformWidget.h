#ifndef WAVEFORMWIDGET_H
#define WAVEFORMWIDGET_H

#include <QWidget>
#include <QTimer>

/*
 Class responsibility:
 The small row of vertical bars next to the Stop button in the prototype.
 It sits still while the machine is idle and "moves" while a program is running.
 */

class WaveformWidget : public QWidget
{
    Q_OBJECT

public:
    explicit WaveformWidget(QWidget* parent = nullptr);

    void setActive(bool active);          // true while the program is running
    QSize sizeHint() const override;

protected:
    void paintEvent(QPaintEvent* event) override;

private slots:
    void advance();                       // moves the bars one step

private:
    bool active;
    int phase;
    QTimer timer;
};

#endif // WAVEFORMWIDGET_H
