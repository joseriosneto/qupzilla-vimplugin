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

#include "VimEngine.h"

VimEngine::VimEngine()
    : m_single_step(30)
    , m_g_pressed(false)
{
}

void VimEngine::handleKeyEvent(WebPage *page, QKeyEvent *event)
{
    Q_ASSERT(page);
    Q_ASSERT(event);

    if (Qt::Key_H == event->key()) {
        page->scroll(-1 * m_single_step, 0);
        return;
    }

    if (Qt::Key_J == event->key()) {
        page->scroll(0, m_single_step);
        return;
    }

    if (Qt::Key_K == event->key()) {
        page->scroll(0, -1 * m_single_step);
        return;
    }

    if (Qt::Key_L == event->key()) {
        page->scroll(m_single_step, 0);
        return;
    }

    if ("g" == event->text()) {
        if (!m_g_pressed) {
            m_g_pressed = true;
            return;
        }
        page->runJavaScript("window.scrollTo(0, 0)");
        m_g_pressed = false;
        return;
    }
}
