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
{
}

VimPlugin::~VimPlugin()
{
}

PluginSpec VimPlugin::pluginSpec()
{
    PluginSpec spec;

    spec.name = "Vim plugin";
    spec.info =  "Enable vim command set for browsing.";
    spec.description = "How awesome is Vimium? This plugin try to port its "
                "functionalities to QupZilla.";
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

    connect(mApp->plugins(), SIGNAL(webPageDeleted(WebPage *)),
        &m_vim_engine, SLOT(stopScrollingIfPageWasDeleted(WebPage *)));

    QZ_REGISTER_EVENT_HANDLER(PluginProxy::KeyPressHandler);
    QZ_REGISTER_EVENT_HANDLER(PluginProxy::KeyReleaseHandler);
}

bool VimPlugin::testPlugin()
{
    static const QString min_qupzilla_version("2.1.0");
    return QString(Qz::VERSION) >= min_qupzilla_version;
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

    m_vim_engine.handleKeyPressEvent(view->page(), event);

    return false;
}

bool VimPlugin::keyRelease(const Qz::ObjectName &type, QObject* obj,
        QKeyEvent* event)
{
    Q_UNUSED(type);

    WebView *view = qobject_cast<WebView *>(obj);
    if (!view)
        return false;

    m_vim_engine.handleKeyReleaseEvent(view->page(), event);

    return false;
}
