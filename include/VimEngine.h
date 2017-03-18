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

class VimEngine
{
    public:
        explicit VimEngine();

        int stepSize() const
        {
            return m_single_step;
        }

        void handleKeyEvent(WebPage *page, QKeyEvent *event);

    private:
        int m_single_step;
        bool m_g_pressed;
};

#endif
