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

#include "VimPlugin.h"

#include <QWebEngineView>

#include "mainapplication.h"
#include "pluginproxy.h"
#include "webview.h"
#include "webpage.h"


VimPlugin::VimPlugin()
    : QObject()
    , m_single_step(30)
{
}

PluginSpec VimPlugin::pluginSpec()
{
    PluginSpec spec;

    spec.name = "Vim plugin";
    spec.info = "How awesome is Vimium? This plugin try to port its "
        "functionalities to QupZilla.";
    spec.description = "Enable vim command set for browsing.";
    spec.version = "0.0.1";
    spec.author = "Jose Rios <joseriosneto@gmail.com>";
    spec.icon = QPixmap(":/vimplugin/data/vim-logo-en.png");
    spec.hasSettings = false;

    return spec;
}

void VimPlugin::init(InitState state, const QString &settingsPath)
{
    qDebug() << __FUNCTION__ << "called";

    Q_UNUSED(state)
    Q_UNUSED(settingsPath);

    QZ_REGISTER_EVENT_HANDLER(PluginProxy::KeyPressHandler);
}

bool VimPlugin::testPlugin()
{
    return true;
}

void VimPlugin::unload()
{
}

bool VimPlugin::keyPress(const Qz::ObjectName &type, QObject* obj,
        QKeyEvent* event)
{
    Q_UNUSED(type)

    WebView *view = qobject_cast<WebView *>(obj);
    if (!view)
        return false;

    if (Qt::Key_H == event->key())
        view->page()->scroll(-1 * m_single_step, 0);

    if (Qt::Key_J == event->key())
        view->page()->scroll(0, m_single_step);

    if (Qt::Key_K == event->key())
        view->page()->scroll(0, -1 * m_single_step);

    return false;
}
