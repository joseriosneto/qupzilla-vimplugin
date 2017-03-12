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

#include <QtTest/QtTest>
#include "VimPlugin.h"

class VimPluginTests : public QObject
{
    Q_OBJECT

    private slots:
        void PluginSpecHasCorrectData();
        void ProcessOnlyONWebViewEvents();
};

void VimPluginTests::PluginSpecHasCorrectData()
{
    VimPlugin vim_plugin;
    PluginSpec spec;

    spec.name = "Vim plugin";
    spec.info = "How awesome is Vimium? This plugin try to port its "
                "functionalities to QupZilla.";
    spec.description = "Enable vim command set for browsing.";
    spec.version = "0.0.1";
    spec.author = "Jose Rios <joseriosneto@gmail.com>";

    QVERIFY(spec == vim_plugin.pluginSpec());
}

void VimPluginTests::ProcessOnlyONWebViewEvents()
{
    VimPlugin vim_plugin;
    QVERIFY(!vim_plugin.keyPress(Qz::ON_WebView, nullptr, nullptr));
}

QTEST_MAIN(VimPluginTests)