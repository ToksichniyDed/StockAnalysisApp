//
// Created by DED on 27.02.2026.
//

#include <QPainter>
#include <QPainterPath>
#include <QMouseEvent>
#include <QFontMetrics>

#include "InstrumentTabButton.h"
#include "InstrumentContext.h"
#include "AppTheme.h"

static constexpr int MARKER_SIZE = 6;
static constexpr int CLOSE_SIZE = 14;
static constexpr int H_PAD = 10;
static constexpr int ACTIVE_LINE = 2;
static constexpr int SPACING = 6;


InstrumentTabButton::InstrumentTabButton(InstrumentContext* instrumentContext, QWidget* parent) : QWidget(parent),
    _instrumentContext(std::move(instrumentContext)) {
    setFixedHeight(36);
    setCursor(Qt::PointingHandCursor);

    // считаем ширину по содержимому
    const QFontMetrics fm(AppTheme::monoFont(11));
    const int textW = fm.horizontalAdvance(_instrumentContext->tickerName());
    setFixedWidth(H_PAD + MARKER_SIZE + 6 + textW + 4 + CLOSE_SIZE + H_PAD);

    connect(_instrumentContext, &InstrumentContext::signal_tickerChanged,
            this, [this](const Market::Ticker&) {
                recalculateWidth();
                update();
            });

    recalculateWidth();
}

InstrumentTabButton::~InstrumentTabButton() {
}

void InstrumentTabButton::setActive(bool isActive) {
    if (_active == isActive)
        return;

    _active = isActive;
    update();
}

bool InstrumentTabButton::isActive() const noexcept {
    return _active;
}

void InstrumentTabButton::setMarkerColor(const QColor& color) noexcept {
    _markerColor = color;
    update();
}

void InstrumentTabButton::paintEvent(QPaintEvent* event) {
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing);

    const QRect r = rect();

    // фон
    if (_active) {
        p.fillRect(r, QColor(AppTheme::BG_BASE));
    } else if (_hovered) {
        p.fillRect(r, QColor(AppTheme::BG_HOVER));
    }

    if (!_active) {
        p.setPen(QColor(AppTheme::BORDER));
        p.drawLine(r.right(), 6, r.right(), r.height() - 6);
    }

    if (_active) {
        p.fillRect(QRect(0, 0, r.width(), ACTIVE_LINE), QColor(AppTheme::ACCENT));
    }

    int x = H_PAD;

    p.setBrush(_markerColor);
    p.setPen(Qt::NoPen);
    const int markerY = (r.height() - MARKER_SIZE) / 2;
    p.drawEllipse(x, markerY, MARKER_SIZE, MARKER_SIZE);
    x += MARKER_SIZE + SPACING;

    // тикер
    p.setFont(AppTheme::monoFont(11));
    const QColor textColor = _active
                             ? QColor(AppTheme::TEXT_PRIMARY)
                             : QColor(AppTheme::TEXT_SECONDARY);
    p.setPen(textColor);

    const QString ticker = _instrumentContext->tickerName();
    constexpr int closeAreaW = H_PAD + CLOSE_SIZE + 4;
    const QRect textRect(x, 0, r.width() - x - closeAreaW, r.height());
    p.drawText(textRect, Qt::AlignVCenter | Qt::AlignLeft, ticker);

    // кнопка закрытия только при hover или active
    if (_hovered || _active) {
        const QRect cr = closeButtonRect();

        // фон крестика при hover именно на нём
        if (cr.contains(mapFromGlobal(QCursor::pos()))) {
            p.setBrush(QColor(AppTheme::BG_HOVER));
            p.setPen(Qt::NoPen);
            p.drawRoundedRect(cr, 3, 3);
            p.setPen(QColor(AppTheme::COLOR_SELL));
        } else {
            p.setPen(QColor(AppTheme::TEXT_MUTED));
        }

        p.setFont(AppTheme::displayFont(8));
        p.drawText(cr, Qt::AlignCenter, "x");
    }
}

void InstrumentTabButton::mousePressEvent(QMouseEvent* event) {
    if (event->button() != Qt::LeftButton)
        return;

    if (closeButtonRect().contains(event->pos()))
        emit closeRequested();
    else
        emit clicked();
}

void InstrumentTabButton::enterEvent(QEnterEvent* event) {
    _hovered = true;
    update();
}

void InstrumentTabButton::leaveEvent(QEvent* event) {
    _hovered = false;
    update();
}

void InstrumentTabButton::recalculateWidth() noexcept {
    const QFontMetrics fm(AppTheme::monoFont(11));
    const int textW = fm.horizontalAdvance(_instrumentContext->tickerName());
    setFixedWidth(H_PAD + MARKER_SIZE + SPACING + textW + 4 + CLOSE_SIZE + H_PAD);
}

QRect InstrumentTabButton::closeButtonRect() const noexcept {
    const int y = (height() - CLOSE_SIZE) / 2;
    return {width() - H_PAD - CLOSE_SIZE, y, CLOSE_SIZE, CLOSE_SIZE};
}
