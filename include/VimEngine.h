/* ============================================================
* VimPlugin - Vim Plugin for QupZilla Web Broswer
* Copyright (C) 2017  Jose Rios <joseriosneto@gmail.com>
*
* This program is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program.  If not, see <http://www.gnu.org/licenses/>.
* ============================================================ */

#ifndef VIM_ENGINE_H
#define VIM_ENGINE_H

#include "webpage.h"

#include <QKeyEvent>
#include <QTimer>

class VimEngine : public QObject
{
    Q_OBJECT

    public:
        explicit VimEngine();

        void handleKeyPressEvent(WebPage *page, QKeyEvent *event);
        void handleKeyReleaseEvent(WebPage *page, QKeyEvent *event);

#ifdef VIM_PLUGIN_TESTS
        void init()
        {
            stopScroll();
            m_g_pressed = false;
            m_page = nullptr;
        }

        const QTimer* scrollTimer() const
        {
            return &m_scroll_timer;
        }

        static int stepSize()
        {
            return m_single_step;
        }

        static int scrollSizeWithHJKL()
        {
            return m_single_step * m_num_scroll_steps;
        }

        static int stepsInterval()
        {
            return m_single_step_interval;
        }

        static int numSteps()
        {
            return m_num_scroll_steps;
        }
#endif

    public slots:
        void stopScrollingIfPageWasDeleted(WebPage *deleted_page);

    private slots:
        void scroll();

    private:
        void startScroll(int scroll_hor, int scroll_vert);
        void startFullVerticalScroll(int scroll_step_size);
        void stopScroll();
        void nextTab();
        void previousTab();
        void closeCurTab();

        static const int m_single_step;
        static const int m_single_step_interval;
        static const int m_num_scroll_steps;
        bool m_g_pressed;
        bool m_scroll_active;
        int m_scroll_hor;
        int m_scroll_vert;
        QTimer m_scroll_timer;
        WebPage *m_page;
};

#endif
