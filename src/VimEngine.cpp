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

    const QPointF cur_scroll_pos = page->scrollPosition();

    if ("h" == event->text()) {
        page->scroll(-1 * m_single_step, 0);
        goto end;
    }

    if ("j" == event->text()) {
        page->scroll(0, m_single_step);
        goto end;
    }

    if ("k" == event->text()) {
        page->scroll(0, -1 * m_single_step);
        goto end;
    }

    if ("l" == event->text()) {
        page->scroll(m_single_step, 0);
        goto end;
    }

    if ("G" == event->text()) {
        page->runJavaScript(
                QString("window.scrollTo(%1, document.body.scrollHeight);")
                .arg(cur_scroll_pos.x()));
        goto end;
    }

    if ("g" == event->text()) {
        if (!m_g_pressed) {
            m_g_pressed = true;
            return;
        }
        page->runJavaScript(QString("window.scrollTo(%1, 0)")
                .arg(cur_scroll_pos.x()));
        goto end;
    }

end:
    m_g_pressed = false;
    return;
}
