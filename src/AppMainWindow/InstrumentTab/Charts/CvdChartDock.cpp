//
// Created by DED on 28.02.2026.
//

#include "CvdChartDock.h"

#include <QPainter>
#include <QPainterPath>

#include "AppTheme.h"
#include <InstrumentTab/InstrumentContext.h>
#include <Logger/Logger.h>


CvdChartWidget::CvdChartWidget(QWidget* parent) : QWidget(parent) {
}

void CvdChartWidget::setCandles(const std::vector<Market::Candle>& candles) {
    _candles = candles;
    _candleWidth = 0.0;
    _viewOffset = 0;

    Logger::log<Logger::LogLevel::Debug>("Подгружены свечи {} штук !", _candles.size());

    recalculateCvd();
    recalculateView();
    update();
}

void CvdChartWidget::paintEvent(QPaintEvent* event) {
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    drawBackground(painter);
    if (!_cvdValues.empty()) {
        drawZeroLine(painter);
        drawGradient(painter);
        drawCvdLine(painter);
        drawAxisY(painter);
    }
}

void CvdChartWidget::resizeEvent(QResizeEvent* event) {
    QWidget::resizeEvent(event);
    recalculateCvd();
    update();
}

void CvdChartWidget::recalculateCvd() {
    _cvdValues.clear();
    _cvdValues.reserve(_candles.size());

    double runningCvd = 0.0;

    for (const auto& candle : _candles) {
        if (candle.buyVolume.has_value() && candle.sellVolume.has_value()) {
            runningCvd += *candle.buyVolume - *candle.sellVolume;
        }
        _cvdValues.push_back(runningCvd);
    }

    Logger::log<Logger::LogLevel::Debug>("Пересчет CVD!");
}

void CvdChartWidget::recalculateView() {
    if (_cvdValues.empty())
        return;

    if (_candleWidth <= 0.0) {
        _candleWidth = std::clamp(
            static_cast<double>(chartRect().width()) / _candles.size(),
            MIN_CANDLE_W,
            MAX_CANDLE_W
        );

        _viewOffset = std::max(0, static_cast<int>(_cvdValues.size()) - visibleCandleCount());
    }

    const int first = _viewOffset;
    const int last = std::min(_viewOffset + visibleCandleCount(), static_cast<int>(_candles.size()));

    _cvdMin = *std::min_element(_cvdValues.begin() + first, _cvdValues.begin() + last);
    _cvdMax = *std::max_element(_cvdValues.begin() + first, _cvdValues.begin() + last);

    // симметричный диапазон относительно нуля
    const double absMax = std::max(std::abs(_cvdMin), std::abs(_cvdMax));
    const double padding = absMax * 0.1;
    _cvdMin = -absMax - padding;
    _cvdMax =  absMax + padding;

    Logger::log<Logger::LogLevel::Debug>("Пересчет отображения!");
}

void CvdChartWidget::drawBackground(QPainter& painter) const {
    painter.fillRect(rect(), palette().color(QPalette::Base));

    // фон оси Y
    painter.fillRect(
        QRect(chartRect().width(), 0, Y_AXIS_WIDTH, height()),
        palette().color(QPalette::Window)
    );

    // граница оси Y
    painter.setPen(palette().color(QPalette::Mid));
    painter.drawLine(chartRect().width(), 0, chartRect().width(), height());

    Logger::log<Logger::LogLevel::Debug>("Отрисовка заднего фона!");
}

void CvdChartWidget::drawZeroLine(QPainter& painter) const {
    const int zeroY = static_cast<int>(cvdToY(0.0));

    painter.setPen(QPen(palette().color(QPalette::Mid), 1, Qt::DashLine));
    painter.drawLine(0, zeroY, chartRect().width(), zeroY);

    Logger::log<Logger::LogLevel::Debug>("Отрисовка 0 линии!");
}

void CvdChartWidget::drawGradient(QPainter& painter) const {
    if (_cvdValues.empty())
        return;

    const QRect cr = chartRect();
    const int zeroY = static_cast<int>(cvdToY(0.0));
    const int first = _viewOffset;
    const int last = std::min(
        _viewOffset + visibleCandleCount(),
        static_cast<int>(_cvdValues.size())
    );

    // строим путь кривой
    QPainterPath path;
    path.moveTo(candleX(first), cvdToY(_cvdValues[first]));
    for (int i = first + 1; i < last; ++i) {
        path.lineTo(candleX(i), cvdToY(_cvdValues[i]));
    }

    // замыкаем путь через нулевую линию
    path.lineTo(candleX(last - 1), zeroY);
    path.lineTo(candleX(first), zeroY);
    path.closeSubpath();

    // градиент сверху вниз — акцентный цвет к прозрачному
    QLinearGradient gradient(0, 0, 0, cr.height());
    gradient.setColorAt(0.0, QColor(0, 212, 255, 80)); // accent с прозрачностью
    gradient.setColorAt(1.0, QColor(0, 212, 255, 0));

    painter.setPen(Qt::NoPen);
    painter.setBrush(gradient);
    painter.drawPath(path);

    Logger::log<Logger::LogLevel::Debug>("Отрисовка градиента!");
}

void CvdChartWidget::drawCvdLine(QPainter& painter) const {
    if (_cvdValues.empty())
        return;

    const int first = _viewOffset;
    const int last = std::min(
        _viewOffset + visibleCandleCount(),
        static_cast<int>(_cvdValues.size())
    );

    QPainterPath path;
    path.moveTo(candleX(first), cvdToY(_cvdValues[first]));
    for (int i = first + 1; i < last; ++i) {
        path.lineTo(candleX(i), cvdToY(_cvdValues[i]));
    }

    painter.setPen(QPen(QColor(0, 212, 255), 1.5));
    painter.setBrush(Qt::NoBrush);
    painter.drawPath(path);

    Logger::log<Logger::LogLevel::Debug>("Отрисовка CVD линии!");
}

void CvdChartWidget::drawAxisY(QPainter& painter) const {
    if (qFuzzyCompare(_cvdMax, _cvdMin))
        return;

    const int axX = chartRect().width();
    static constexpr int GRID_LINES = 4;

    painter.setFont(AppTheme::monoFont(9));

    for (int i = 0; i <= GRID_LINES; ++i) {
        const double ratio = static_cast<double>(i) / GRID_LINES;
        const double cvd = _cvdMax - (_cvdMax - _cvdMin) * ratio;
        const int y = static_cast<int>(cvdToY(cvd));

        // засечка
        painter.setPen(palette().color(QPalette::Mid));
        painter.drawLine(axX, y, axX + 4, y);

        // значение
        painter.setPen(palette().color(QPalette::PlaceholderText));
        const QString label = QString::number(cvd, 'f', 0);
        painter.drawText(
            QRect(axX + 6, y - 10, Y_AXIS_WIDTH - 8, 20),
            Qt::AlignVCenter | Qt::AlignLeft,
            label
        );
    }

    Logger::log<Logger::LogLevel::Debug>("Отрисовка оси Y!");
}

QRect CvdChartWidget::chartRect() const noexcept {
    return { 0, 0, width() - Y_AXIS_WIDTH, height() };
}

double CvdChartWidget::cvdToY(double cvd) const noexcept {
    if (qFuzzyCompare(_cvdMax, _cvdMin))
        return height() / 2.0;

    return (_cvdMax - cvd) / (_cvdMax - _cvdMin) * chartRect().height();
}

double CvdChartWidget::candleX(int index) const noexcept {
    return (index - _viewOffset) * _candleWidth;
}

int CvdChartWidget::visibleCandleCount() const noexcept {
    if (_candleWidth <= 0.0)
        return 0;

    return static_cast<int>(std::ceil(chartRect().width() / _candleWidth)) + 1;
}

CvdChartDock::CvdChartDock(InstrumentContext* context, InstrumentSession* session, QWidget* parent) : QDockWidget(
    context->tickerName(), parent) {
    setSizePolicy(
        QSizePolicy::Expanding,
        QSizePolicy::Expanding
    );
    setObjectName(QString("CvdChartDock_%1").arg(context->tickerName()));

    _cvdChartWidget = new CvdChartWidget(this);
    setWidget(_cvdChartWidget);
}

CvdChartWidget* CvdChartDock::widget() {
    return _cvdChartWidget;
}