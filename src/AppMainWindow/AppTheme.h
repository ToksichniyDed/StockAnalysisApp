//
// Created by DED on 24.02.2026.
//

#ifndef COOLAPPNAME_APPTHEME_H
#define COOLAPPNAME_APPTHEME_H

#include <QColor>
#include <QFont>
#include <QPalette>
#include <QString>

namespace AppTheme {

    // ── Цвета ───

    // Фоны
    inline constexpr auto BG_BASE = "#0a0c10"; // самый тёмный — основной фон
    inline constexpr auto BG_PANEL = "#0f1218"; // панели, тулбар, статусбар
    inline constexpr auto BG_CARD = "#141820"; // карточки, элементы списков
    inline constexpr auto BG_HOVER = "#1a2030"; // hover состояние

    // Границы
    inline constexpr auto BORDER = "#1e2535";
    inline constexpr auto BORDER_BRIGHT = "#2a3550";

    // Акценты
    inline constexpr auto ACCENT = "#00d4ff"; // основной акцент
    inline constexpr auto ACCENT_DIM = "#1a3a45"; // приглушённый акцент

    // Сигнальные
    inline constexpr auto COLOR_BUY = "#00e676"; // покупки / рост
    inline constexpr auto COLOR_SELL = "#ff3d5a"; // продажи / падение
    inline constexpr auto COLOR_NEUTRAL = "#ff9100"; // нейтральный / предупреждение

    // Текст
    inline constexpr auto TEXT_PRIMARY = "#e8eaf0";
    inline constexpr auto TEXT_SECONDARY = "#6b7a99";
    inline constexpr auto TEXT_MUTED = "#3d4a65";

    // ── Шрифты ───

    // Моноширинный — для цен, тикеров, чисел
    inline QFont monoFont(int pointSize = 10) {
        QFont f("JetBrains Mono", pointSize);
        f.setStyleHint(QFont::Monospace);
        f.setFamilies({"JetBrains Mono", "Consolas", "Courier New"});
        return f;
    }

    // Дисплейный — для заголовков, названий панелей
    inline QFont displayFont(int pointSize = 10, bool bold = false) {
        QFont f("Segoe UI", pointSize);
        f.setFamilies({"Segoe UI", "Inter", "SF Pro Display", "Arial"});
        f.setBold(bold);
        return f;
    }

    // ── QPalette ───

    // Применяется один раз в main.cpp через QApplication::setPalette()
    inline QPalette buildPalette() {
        QPalette p;

        const QColor base(BG_BASE);
        const QColor panel(BG_PANEL);
        const QColor card(BG_CARD);
        const QColor accent(ACCENT);
        const QColor textPrimary(TEXT_PRIMARY);
        const QColor textSecondary(TEXT_SECONDARY);
        const QColor border(BORDER);

        p.setColor(QPalette::Window, panel);
        p.setColor(QPalette::WindowText, textPrimary);
        p.setColor(QPalette::Base, card);
        p.setColor(QPalette::AlternateBase, base);
        p.setColor(QPalette::Text, textPrimary);
        p.setColor(QPalette::BrightText, Qt::white);
        p.setColor(QPalette::ButtonText, textPrimary);
        p.setColor(QPalette::Button, panel);
        p.setColor(QPalette::Highlight, accent);
        p.setColor(QPalette::HighlightedText, QColor(BG_BASE));
        p.setColor(QPalette::ToolTipBase, card);
        p.setColor(QPalette::ToolTipText, textPrimary);
        p.setColor(QPalette::PlaceholderText, textSecondary);
        p.setColor(QPalette::Mid, border);
        p.setColor(QPalette::Dark, base);
        p.setColor(QPalette::Shadow, base);

        // Disabled состояния
        p.setColor(QPalette::Disabled, QPalette::WindowText, textSecondary);
        p.setColor(QPalette::Disabled, QPalette::Text, textSecondary);
        p.setColor(QPalette::Disabled, QPalette::ButtonText, textSecondary);

        return p;
    }

    // ── QSS (stylesheet) ───

    // Базовые стили — применяются через QApplication::setStyleSheet()
    inline QString baseStyleSheet() {
        return QString(R"(
            QMainWindow {
                background-color: %1;
            }

            QMenuBar {
                background-color: %2;
                color: %3;
                border-bottom: 1px solid %4;
                padding: 2px 4px;
                font-size: 12px;
            }
            QMenuBar::item {
                padding: 4px 10px;
                border-radius: 4px;
            }
            QMenuBar::item:selected {
                background-color: %5;
                color: %6;
            }

            QMenu {
                background-color: %2;
                color: %3;
                border: 1px solid %4;
                border-radius: 6px;
                padding: 4px;
            }
            QMenu::item {
                padding: 6px 24px 6px 12px;
                border-radius: 4px;
            }
            QMenu::item:selected {
                background-color: %5;
                color: %7;
            }
            QMenu::separator {
                height: 1px;
                background: %4;
                margin: 4px 8px;
            }

            QToolBar {
                background-color: %2;
                border-bottom: 1px solid %4;
                spacing: 4px;
                padding: 4px 8px;
            }
            QToolBar::separator {
                width: 1px;
                background: %4;
                margin: 4px 6px;
            }

            QStatusBar {
                background-color: %2;
                color: %8;
                border-top: 1px solid %4;
                font-size: 11px;
            }

            QDockWidget {
                color: %3;
                font-size: 12px;
            }
            QDockWidget::title {
                background-color: %2;
                border-bottom: 1px solid %4;
                padding: 6px 10px;
                text-align: left;
            }
            QDockWidget::close-button,
            QDockWidget::float-button {
                background: transparent;
                border: none;
                padding: 2px;
                border-radius: 3px;
            }
            QDockWidget::close-button:hover,
            QDockWidget::float-button:hover {
                background-color: %5;
            }

            QSplitter::handle {
                background-color: %4;
            }
            QSplitter::handle:horizontal { width: 1px; }
            QSplitter::handle:vertical   { height: 1px; }
            QSplitter::handle:hover {
                background-color: %6;
            }

            QScrollBar:vertical {
                background: transparent;
                width: 6px;
                margin: 0;
            }
            QScrollBar::handle:vertical {
                background: %9;
                border-radius: 3px;
                min-height: 20px;
            }
            QScrollBar::handle:vertical:hover {
                background: %8;
            }
            QScrollBar::add-line:vertical,
            QScrollBar::sub-line:vertical { height: 0; }
            QScrollBar::add-page:vertical,
            QScrollBar::sub-page:vertical { background: transparent; }

            QToolTip {
                background-color: %7;
                color: %3;
                border: 1px solid %4;
                border-radius: 4px;
                padding: 4px 8px;
                font-size: 11px;
            }
        )")
               .arg(BG_BASE) // %1
               .arg(BG_PANEL) // %2
               .arg(TEXT_PRIMARY) // %3
               .arg(BORDER) // %4
               .arg(ACCENT_DIM) // %5
               .arg(ACCENT) // %6
               .arg(BG_CARD) // %7
               .arg(TEXT_SECONDARY) // %8
               .arg(BORDER_BRIGHT) // %9
                ;
    }

}

#endif //COOLAPPNAME_APPTHEME_H
