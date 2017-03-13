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

#ifndef VIMPLUGIN_H
#define VIMPLUGIN_H

#include "plugininterface.h"

class VimPlugin
{
    public:
        explicit VimPlugin() {}

        PluginSpec pluginSpec() const;
        void init(InitState state, const QString &settingsPath);
        bool testPlugin();

        bool keyPress(const Qz::ObjectName &type, QObject* obj,
                QKeyEvent* event);

    private:
        QWebEnginePage *m_page;
};

#endif
