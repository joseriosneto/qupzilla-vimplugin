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
        const QTimer* scrollTimer() const
        {
            return &m_scroll_timer;
        }

        static int stepSize()
        {
            return m_single_step;
        }

        static int stepsInterval()
        {
            return m_single_step_interval;
        }

        static int numStepsFullScroll()
        {
            return m_num_full_scroll_steps;
        }
#endif

    private slots:
        void scroll();
        void fullScroll();

    private:
        void startScroll(int scroll_hor, int scroll_vert);
        void stopScroll();
        void startFullScroll(int scroll_top, int scroll_height);
        void stopFullScroll();

        static int m_single_step;
        static int m_single_step_interval;
        static int m_num_full_scroll_steps;
        bool m_g_pressed;
        bool m_scroll_active;
        int m_scroll_hor;
        int m_scroll_vert;
        QTimer m_scroll_timer;
        WebPage *m_page;

};

#endif
