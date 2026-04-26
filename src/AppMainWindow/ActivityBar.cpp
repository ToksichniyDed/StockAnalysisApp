//
// Created by DED on 14.03.2026.
//

#include "AppTheme.h"
#include "ActivityBar.h"

ActivityBar::ActivityBar(QWidget* parent) : QWidget(parent) {
    setFixedWidth(BUTTON_SIZE);
    setStyleSheet(QString("background:%1; border-right:1px solid %2;")
        .arg(AppTheme::BG_PANEL, AppTheme::BORDER));

    _layout = new QVBoxLayout(this);
    _layout->setContentsMargins(4, 8, 4, 8);
    _layout->setSpacing(4);
    _layout->setAlignment(Qt::AlignTop);

    _priceDockBtn = createToolButton("P", "График цены");
    _cvdDockBtn = createToolButton("C", "CVD");
    _vdDockBtn = createToolButton("V", "Volume Delta");

    auto* sep = new QWidget(this);
    sep->setFixedHeight(1);
    sep->setStyleSheet(QString("background:%1;").arg(AppTheme::BORDER));

    _resetBtn = createToolButton("~", "Сбросить layout");
    _resetBtn->setCheckable(false);

    _layout->addWidget(_priceDockBtn);
    _layout->addWidget(_cvdDockBtn);
    _layout->addWidget(_vdDockBtn);
    _layout->addWidget(sep);
    _layout->addWidget(_resetBtn);
    _layout->addStretch();

    Logger::log<Logger::LogLevel::Debug>("Добавлен ActivityBar!");
}

ActivityBar::~ActivityBar() {
}

void ActivityBar::setActiveInstrumentDockSet(InstrumentDockSet* instrumentDockSet) {
    if (_activeInstrumentDockSet) {
        disconnect(_priceDockBtn, nullptr, _activeInstrumentDockSet, nullptr);
        disconnect(_cvdDockBtn, nullptr, _activeInstrumentDockSet, nullptr);
        disconnect(_vdDockBtn, nullptr, _activeInstrumentDockSet, nullptr);
        disconnect(_resetBtn, nullptr, _activeInstrumentDockSet, nullptr);

        disconnect(_activeInstrumentDockSet, nullptr, _priceDockBtn, nullptr);
        disconnect(_activeInstrumentDockSet, nullptr, _cvdDockBtn, nullptr);
        disconnect(_activeInstrumentDockSet, nullptr, _vdDockBtn, nullptr);
    }

    _activeInstrumentDockSet = instrumentDockSet;

    if (!_activeInstrumentDockSet) {
        _priceDockBtn->setEnabled(false);
        _cvdDockBtn->setEnabled(false);
        _vdDockBtn->setEnabled(false);
        _resetBtn->setEnabled(false);
        return;
    }

    _priceDockBtn->setEnabled(true);
    _cvdDockBtn->setEnabled(true);
    _vdDockBtn->setEnabled(true);
    _resetBtn->setEnabled(true);

    synchronizeDocksVisibilityWithButtons();

    connect(_priceDockBtn, &QToolButton::toggled, _activeInstrumentDockSet,
            &InstrumentDockSet::slot_PriceChartDockVisible);
    connect(_cvdDockBtn, &QToolButton::toggled, _activeInstrumentDockSet,
            &InstrumentDockSet::slot_CvdChartDockVisible);
    connect(_vdDockBtn, &QToolButton::toggled, _activeInstrumentDockSet, &InstrumentDockSet::slot_VdChartDockVisible);
    connect(_resetBtn, &QToolButton::clicked, _activeInstrumentDockSet, &InstrumentDockSet::slot_ResetDocksLayout);

    connect(_activeInstrumentDockSet, &InstrumentDockSet::signal_PriceChartDockChangedVisible, _priceDockBtn,
            &QToolButton::setChecked);
    connect(_activeInstrumentDockSet, &InstrumentDockSet::signal_CvdChartDockChangedVisible, _cvdDockBtn,
            &QToolButton::setChecked);
    connect(_activeInstrumentDockSet, &InstrumentDockSet::signal_VdChartDockChangedVisible, _vdDockBtn,
            &QToolButton::setChecked);

    Logger::log<Logger::LogLevel::Debug>("Смена активного ActivityBar!");
}

QToolButton* ActivityBar::createToolButton(const QString& icon, const QString& tooltip) {
    auto* btn = new QToolButton(this);
    btn->setText(icon);
    btn->setToolTip(tooltip);
    btn->setFixedSize(BUTTON_SIZE, BUTTON_SIZE);
    btn->setCheckable(true);
    btn->setStyleSheet(QString(R"(
        QToolButton {
            background: transparent;
            color: %1;
            border: none;
            border-radius: 6px;
            font-size: 13px;
        }
        QToolButton:hover {
            background: %2;
            color: %3;
        }
        QToolButton:checked {
            background: %4;
            color: %5;
        }
    )").arg(AppTheme::TEXT_SECONDARY,
            AppTheme::BG_HOVER,
            AppTheme::TEXT_PRIMARY,
            AppTheme::ACCENT_DIM,
            AppTheme::ACCENT));

    return btn;
}

void ActivityBar::synchronizeDocksVisibilityWithButtons() const {
    QSignalBlocker blocker1(_priceDockBtn), blocker2(_cvdDockBtn), blocker3(_vdDockBtn);

    _priceDockBtn->setChecked(_activeInstrumentDockSet->priceChartDockVisible());
    _cvdDockBtn->setChecked(_activeInstrumentDockSet->cvdChartDockVisible());
    _vdDockBtn->setChecked(_activeInstrumentDockSet->vdChartDockVisible());
}
