//
// Created by DED on 27.02.2026.
//

#include <QToolButton>

#include "InstrumentTabBar.h"
#include <AppTheme.h>
#include <qcoreevent.h>
#include <qevent.h>

#include <Logger/Logger.h>

InstrumentTabBar::InstrumentTabBar(QWidget* parent) : QWidget(parent) {
    setFixedHeight(36);
    setStyleSheet(QString("background:%1; border-bottom:1px solid %2;")
                      .arg(AppTheme::BG_PANEL, AppTheme::BORDER));

    _tabLayout = new QHBoxLayout(this);
    _tabLayout->setContentsMargins(8, 0, 8, 0);
    _tabLayout->setSpacing(2);
    _tabLayout->addStretch();

    _addInstrumentTabButton = new QToolButton(this);
    _addInstrumentTabButton->setText("+");
    _addInstrumentTabButton->setFixedSize(28,28);
    _addInstrumentTabButton->setStyleSheet(QString(R"(
        QToolButton {
            background: transparent;
            color: %1;
            border: none;
            font-size: 18px;
            border-radius: 4px;
        }
        QToolButton:hover { background: %2; color: %3; }
    )").arg(AppTheme::TEXT_MUTED, AppTheme::BG_HOVER, AppTheme::ACCENT));

    _tabLayout->addWidget(_addInstrumentTabButton);
    connect(_addInstrumentTabButton, &QToolButton::clicked, this, &InstrumentTabBar::slot_AddInstrumentButtonClicked);

    Logger::log<Logger::LogLevel::Debug>("Создан InstrumentTabBar!");
}

InstrumentTabBar::~InstrumentTabBar() {
}

void InstrumentTabBar::addInstrumentTab(InstrumentContext* instrumentContext) {
    auto newBtn = new InstrumentTabButton(instrumentContext, this);

    connect(newBtn, &InstrumentTabButton::clicked, this, [this, newBtn] {
        emit signal_InstrumentTabClicked(_instrumentTabs.indexOf(newBtn));
    });
    connect(newBtn, &InstrumentTabButton::closeRequested, this, [this, newBtn] {
        emit signal_InstrumentTabCloseRequested(_instrumentTabs.indexOf(newBtn));
    });

    _instrumentTabs.append(newBtn);
    _tabLayout->insertWidget(_instrumentTabs.size() - 1, newBtn);

    Logger::log<Logger::LogLevel::Debug>("Добавлен InstrumentTab!");
}

void InstrumentTabBar::removeInstrumentTab(int index) {
    if (index < 0 || index >= _instrumentTabs.size())
        return;

    auto removeBtn = _instrumentTabs.takeAt(index);
    _tabLayout->removeWidget(removeBtn);
    removeBtn->deleteLater();

    Logger::log<Logger::LogLevel::Debug>("Удален InstrumentTab!");
}

void InstrumentTabBar::setActiveInstrumentTab(const int index) {
    if (index < 0 || index >= _instrumentTabs.size())
        return;

    for (int i = 0; i < _instrumentTabs.size(); i++) {
        _instrumentTabs.at(i)->setActive(i == index);
    }

    Logger::log<Logger::LogLevel::Debug>("Смена активного InstrumentTab!");
}

void InstrumentTabBar::slot_AddInstrumentButtonClicked() {
    if (_tickerEdit)
        return;

    _tickerEdit = new QLineEdit(this);
    _tickerEdit->setFixedWidth(90);
    _tickerEdit->setFixedHeight(24);
    _tickerEdit->setPlaceholderText("Enter the ticker...");
    _tickerEdit->setStyleSheet(QString(R"(
        QLineEdit {
            background: %1;
            color: %2;
            border: 1px solid %3;
            border-radius: 4px;
            padding: 0 6px;
            font-family: 'JetBrains Mono', 'Consolas';
            font-size: 12px;
        }
        QLineEdit:focus { border-color: %4; }
    )").arg(AppTheme::BG_CARD, AppTheme::TEXT_PRIMARY, AppTheme::BORDER_BRIGHT, AppTheme::ACCENT));

    connect(_tickerEdit, &QLineEdit::destroyed, this, [this]{_tickerEdit = nullptr;});

    const int addBtnIdx = _tabLayout->indexOf(_addInstrumentTabButton);
    _tabLayout->insertWidget(addBtnIdx, _tickerEdit);

    Logger::log<Logger::LogLevel::Debug>("Добавлен InstrumentTabButton!");

    _tickerEdit->setFocus();

    connect(_tickerEdit, &QLineEdit::returnPressed,
           this, &InstrumentTabBar::slot_InstrumentTickerEntered);

    // Esc — отмена
    connect(_tickerEdit, &QLineEdit::textChanged, this, [this](const QString&) {
        // живём
    });

    _tickerEdit->installEventFilter(this);
}

void InstrumentTabBar::slot_InstrumentTickerEntered() {
    if (!_tickerEdit)
        return;

    const QString ticker = _tickerEdit->text().trimmed().toUpper();

    _tabLayout->removeWidget(_tickerEdit);
    _tickerEdit->deleteLater();
    _tickerEdit = nullptr;

    if (ticker.isEmpty())
        return;

    emit signal_AddInstrumentTabRequested(ticker);
}

void InstrumentTabBar::slot_TickerEditingCancelled() {
    if (!_tickerEdit)
        return;

    _tabLayout->removeWidget(_tickerEdit);
    _tickerEdit->deleteLater();
    _tickerEdit = nullptr;
}

bool InstrumentTabBar::eventFilter(QObject* sender, QEvent* event) {
    if (sender == _tickerEdit && event->type() == QEvent::KeyPress) {
        auto* ke = static_cast<QKeyEvent*>(event);
        if (ke->key() == Qt::Key_Escape) {
            slot_TickerEditingCancelled();
            return true;
        }
    }
    return QWidget::eventFilter(sender, event);
}
