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

int VimEngine::m_single_step = 18;
int VimEngine::m_single_step_interval = 20;

VimEngine::VimEngine()
    : m_g_pressed(false)
    , m_scroll_active(false)
    , m_scroll_hor(0)
    , m_scroll_vert(0)
    , m_scroll_timer()
    , m_page(nullptr)
{
    m_scroll_timer.setInterval(m_single_step_interval);
    connect(&m_scroll_timer, SIGNAL(timeout()), this, SLOT(scroll()));
}

void VimEngine::handleKeyPressEvent(WebPage *page, QKeyEvent *event)
{
    Q_ASSERT(page);
    Q_ASSERT(event);

    const QPointF cur_scroll_pos = page->scrollPosition();

    if ("h" == event->text()) {
        startScroll(-1 * m_single_step, 0);
        goto end;
    }

    if ("j" == event->text()) {
        startScroll(0, m_single_step);
        goto end;
    }

    if ("k" == event->text()) {
        startScroll(0, -1 * m_single_step);
        goto end;
    }

    if ("l" == event->text()) {
        startScroll(m_single_step, 0);
        goto end;
    }

    if ("G" == event->text()) {
        page->runJavaScript(
                QString("window.scrollTo(%1, document.body.scrollHeight);")
                .arg(cur_scroll_pos.x()));
        goto end;
    }

    if ("u" == event->text()) {
        page->runJavaScript(
                QString("window.scrollTo(%1, %2 - (window.innerHeight / 2));")
                .arg(cur_scroll_pos.x()).arg(cur_scroll_pos.y()));
        goto end;
    }

    if ("d" == event->text()) {
        page->runJavaScript(
                QString("window.scrollTo(%1, %2 + (window.innerHeight / 2));")
                .arg(cur_scroll_pos.x()).arg(cur_scroll_pos.y()));
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
    m_page = page;
    m_g_pressed = false;
    return;
}

void VimEngine::handleKeyReleaseEvent(WebPage *page, QKeyEvent *event)
{
    Q_UNUSED(page);

    if ("h" == event->text()) {
        m_scroll_active = false;
        return;
    }

    if ("j" == event->text()) {
        m_scroll_active = false;
        return;
    }

    if ("k" == event->text()) {
        m_scroll_active = false;
        return;
    }

    if ("l" == event->text()) {
        m_scroll_active = false;
        return;
    }
}

void VimEngine::startScroll(int scroll_hor, int scroll_vert)
{
    m_scroll_hor = scroll_hor;
    m_scroll_vert = scroll_vert;
    if (!m_scroll_active) {
        m_scroll_active = true;
        m_scroll_timer.start();
    }
}

void VimEngine::scroll()
{
    m_page->scroll(m_scroll_hor, m_scroll_vert);
    if (!m_scroll_active) {
        m_scroll_hor = 0;
        m_scroll_vert = 0;
        m_scroll_timer.stop();
    }
}
