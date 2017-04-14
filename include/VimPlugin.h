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
#include "VimEngine.h"

class VimPlugin : public QObject, public PluginInterface
{
    Q_OBJECT
    Q_INTERFACES(PluginInterface)
    Q_PLUGIN_METADATA(IID "QupZilla.Browser.plugin.VimPlugin")

    public:
        explicit VimPlugin();

        PluginSpec pluginSpec();
        void init(InitState state, const QString &settingsPath);
        bool testPlugin();
        void unload();

        bool keyPress(const Qz::ObjectName &type, QObject* obj,
                QKeyEvent* event);
        bool keyRelease(const Qz::ObjectName &type, QObject* obj,
                QKeyEvent* event);

#ifdef VIM_PLUGIN_TESTS
        void init()
        {
            m_vim_engine.init();
        }

        const VimEngine& vimEngine() const
        {
            return m_vim_engine;
        }
#endif

    private:
        VimEngine m_vim_engine;
};

#endif
