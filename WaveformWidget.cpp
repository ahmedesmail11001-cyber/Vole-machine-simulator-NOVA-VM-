#include "WaveformWidget.h"

#include <QPainter>
#include <cmath>

namespace {
// resting shape of the bars (heights in pixels), copied from the look of the prototype
const int kIdleHeights[] = { 3, 3, 8, 3, 14, 3, 3, 10, 3, 20, 3, 3, 12, 3, 3, 16,
                             3, 3, 9, 3, 3, 6, 3, 18, 3, 8, 3, 12 };
const int kBarCount = sizeof(kIdleHeights) / sizeof(kIdleHeights[0]);
const int kBarWidth = 3;
const int kBarGap = 5;
}

WaveformWidget::WaveformWidget(QWidget* parent) : QWidget(parent), active(false), phase(0)
{
    connect(&timer, &QTimer::timeout, this, &WaveformWidget::advance);
    setMinimumHeight(34);
}

void WaveformWidget::setActive(bool isActive)
{
    active = isActive;
    if (active)
        timer.start(90);
    else
        timer.stop();
    update();
}

QSize WaveformWidget::sizeHint() const
{
    return QSize(kBarCount * (kBarWidth + kBarGap), 34);
}

void WaveformWidget::advance()
{
    phase++;
    update();
}

void WaveformWidget::paintEvent(QPaintEvent*)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setPen(Qt::NoPen);

    const int midY = height() / 2;

    for (int i = 0; i < kBarCount; i++) {
        double h = kIdleHeights[i];

        if (active) {
            // moving wave while the program runs
            const double wave = 0.5 + 0.5 * std::sin((i + phase) * 0.7);
            h = 3 + wave * 24;
            painter.setBrush(QColor("#a78bfa"));
        } else {
            // idle: tall bars are brighter than the small dots
            painter.setBrush(h > 3 ? QColor("#b9b2f0") : QColor("#4a4a66"));
        }

        const QRectF bar(i * (kBarWidth + kBarGap), midY - h / 2.0, kBarWidth, h);
        painter.drawRoundedRect(bar, 1.5, 1.5);
    }
}
