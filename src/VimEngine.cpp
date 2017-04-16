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

#include "webview.h"
#include "browserwindow.h"
#include "tabbedwebview.h"
#include "tabwidget.h"

const int VimEngine::m_single_step = 9;
const int VimEngine::m_single_step_interval = 15;
const int VimEngine::m_num_scroll_steps = 7;

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
    m_page = page;

    if ("h" == event->text()) {
        startScroll(-1 * m_single_step, 0);
        goto end;
    }

    if ("j" == event->text()) {
        startScroll(0, m_single_step);
        goto end;
    }

    if ("J" == event->text()) {
        previousTab();
        goto end;
    }

    if ("k" == event->text()) {
        startScroll(0, -1 * m_single_step);
        goto end;
    }

    if ("K" == event->text()) {
        nextTab();
        goto end;
    }

    if ("l" == event->text()) {
        startScroll(m_single_step, 0);
        goto end;
    }

    if ("G" == event->text()) {
        m_page->runJavaScript(
            QString("var page_height = document.body.scrollHeight;"
                    "var page_y_offset = window.pageYOffset;"
                    /* Adding 10 because of int truncation. */
                    "((page_height - page_y_offset) / %1) + 10;")
                .arg(m_num_scroll_steps),
            [this] (const QVariant& res) {
                this->startFullVerticalScroll(res.toInt());
            });
        goto end;
    }

    if ("u" == event->text()) {
        const QRect viewport_size = m_page->view()->geometry();
        startScroll(0, -1 * (viewport_size.height() / 2) / m_num_scroll_steps);
        goto end;
    }

    if ("d" == event->text()) {
        const QRect viewport_size = m_page->view()->geometry();
        startScroll(0, (viewport_size.height() / 2) / m_num_scroll_steps);
        goto end;
    }

    if ("g" == event->text()) {
        if (!m_g_pressed) {
            m_g_pressed = true;
            return;
        }
        m_page->runJavaScript(
            /* Adding 10 because of int truncation. */
            QString("-1 * ((window.pageYOffset / %1) + 10)")
                .arg(m_num_scroll_steps),
            [this] (const QVariant& res) {
                this->startFullVerticalScroll(res.toInt());
            });
        goto end;
    }

    if ("r" == event->text()) {
        m_page->view()->reload();
        goto end;
    }

end:
    m_g_pressed = false;
    return;
}

void VimEngine::handleKeyReleaseEvent(WebPage *page, QKeyEvent *event)
{
    Q_UNUSED(page);

    if (Qt::Key_H == event->key()) {
        m_scroll_active = false;
        return;
    }

    if (Qt::Key_J == event->key()) {
        m_scroll_active = false;
        return;
    }

    if (Qt::Key_K == event->key()) {
        m_scroll_active = false;
        return;
    }

    if (Qt::Key_L == event->key()) {
        m_scroll_active = false;
        return;
    }

    if (Qt::Key_U == event->key()) {
        m_scroll_active = false;
        return;
    }

    if (Qt::Key_D == event->key()) {
        m_scroll_active = false;
        return;
    }
}

void VimEngine::scroll()
{
    static int step_i = 0;
    m_page->scroll(m_scroll_hor, m_scroll_vert);
    ++step_i;
    if (step_i >= m_num_scroll_steps) {
        step_i = 0;
        /* If the user is still pressing the key we don't stop scrolling. */
        if (!m_scroll_active)
            stopScroll();
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

void VimEngine::stopScroll()
{
    m_scroll_hor = 0;
    m_scroll_vert = 0;
    m_scroll_active = false;
    m_scroll_timer.stop();
}

void VimEngine::startFullVerticalScroll(int scroll_step_size)
{
    stopScroll();

    m_scroll_hor = 0;
    m_scroll_vert = scroll_step_size;
    m_scroll_timer.start();
}

void VimEngine::nextTab()
{
    TabbedWebView *tab_view = dynamic_cast<TabbedWebView*>(m_page->view());
    if (!tab_view || !tab_view->browserWindow())
        return;

    tab_view->browserWindow()->tabWidget()->nextTab();
}

void VimEngine::previousTab()
{
    TabbedWebView *tab_view = dynamic_cast<TabbedWebView*>(m_page->view());
    if (!tab_view || !tab_view->browserWindow())
        return;

    tab_view->browserWindow()->tabWidget()->previousTab();
}
