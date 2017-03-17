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

#include "mainapplication.h"
#include "browserwindow.h"
#include "tabbedwebview.h"
#include "webpage.h"

class VimPluginTests : public QObject
{
    Q_OBJECT

    private slots:
        void initTestCase()
        {
            int argc = 1;
            static char *s[] = {"VimPluginTests"};

            m_app = new MainApplication(argc, s);

            /* QupZilla has some 'postLaunch' initialization functions that are
             * posted in event loop queue to be processed on the next(s)
             * iteration(s). The QTRY_* macros though do not process GUI and
             * Qwidgets events since it use QCoreApplication for processing
             * this. We need then to run a GUI event loop here to correctly
             * initialize application.
             */
            QTestEventLoop::instance().enterLoop(1);
        }

        void cleanupTestCase()
        {
            delete m_app;
        }

        void PluginSpecHasCorrectData();
        void ProcessOnlyONWebViewEvents();
        void LoadPluginOnVersion_2_1_99();
        void KeyJScrollDownSingleStep();

    private:
        MainApplication *m_app;
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

void VimPluginTests::LoadPluginOnVersion_2_1_99()
{
    VimPlugin vim_plugin;
    QVERIFY(vim_plugin.testPlugin());
}

void VimPluginTests::KeyJScrollDownSingleStep()
{
   QTRY_VERIFY(m_app->getWindow());
   BrowserWindow *window = m_app->getWindow();

   QTRY_VERIFY(window->weView());
   TabbedWebView *view = window->weView();

   view->show();
   QTest::qWaitForWindowExposed(view);

   QSignalSpy loadSpy(view->page(), SIGNAL(loadFinished(bool)));
   view->load(QUrl("file:///" + QDir::currentPath() + "/pages/long_page.html"));
   QTRY_COMPARE(loadSpy.count(), 1);

   view->page()->runJavaScript("window.scrollTo(0, 100);");
   QTRY_COMPARE(view->page()->scrollPosition().y(), qreal(100));
   QCOMPARE(view->page()->scrollPosition().x(), qreal(0));

   QTest::keyPress(view->parentWidget(), Qt::Key_J, Qt::NoModifier);
   QTRY_COMPARE(view->page()->scrollPosition().y(), qreal(130));
   QCOMPARE(view->page()->scrollPosition().x(), qreal(0));
}

/* Using "APPLESS" version because MainApplication is already a QApplication
 * and it was not coping well with QTEST_MAIN.
 */
QTEST_APPLESS_MAIN(VimPluginTests)
