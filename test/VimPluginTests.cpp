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
#include "pluginproxy.h"

class VimPluginTests : public QObject
{
    Q_OBJECT

    private slots:
        void initTestCase()
        {
            int argc = 1;
            char *argv[argc];

            argv[0] =
                ((QByteArray) QString("VimPluginTests").toLatin1()).data();
            m_app = new MainApplication(argc, argv);

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

        void ScrollNavigationWithHJKL_data();
        void ScrollNavigationWithHJKL();

        void ScrollToTopWithDoubleLowerCaseG_data();
        void ScrollToTopWithDoubleLowerCaseG();

        void ScrollToBottomWithCapitalG();

        void ScrollHalfViewportUpWithLowerCaseU();
        void ScrollHalfViewportDownWithLowerCaseD();

    private:
        void loadTestPage(const QString &url, WebView **view)
        {
            QTRY_VERIFY(m_app->getWindow());
            BrowserWindow *window = m_app->getWindow();

            QTRY_VERIFY(window->weView());
            TabbedWebView *tab_view = window->weView();

            tab_view->show();
            QTest::qWaitForWindowExposed(tab_view);

            QSignalSpy loadSpy(tab_view->page(), SIGNAL(loadFinished(bool)));
            tab_view->load(QUrl(url));
            QTRY_COMPARE(loadSpy.count(), 1);

            (*view) = tab_view;
        }

        void processEvents(int ms_maxtime = 50)
        {
            QEventLoop loop;
            QTimer::singleShot(ms_maxtime, &loop, &QEventLoop::quit);
            loop.exec();
        }

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

void VimPluginTests::ScrollNavigationWithHJKL_data()
{
    QTest::addColumn<QTestEventList>("key_event");
    QTest::addColumn<QPointF>("expected_pos");
    QTest::addColumn<int>("expected_scroll_steps");

    int initial_x = 1000;
    int initial_y = 1000;

    QTestEventList key_h_scroll_left;
    key_h_scroll_left.addKeyClicks("h");
    QTest::newRow("scroll left on 'h'")
        << key_h_scroll_left
        << QPointF(initial_x - VimEngine::scrollSizeWithHJKL(), initial_y)
        << VimEngine::numSteps();

    QTestEventList key_H_dont_scroll;
    key_H_dont_scroll.addKeyClicks("H");
    QTest::newRow("dont scroll left on 'H'")
        << key_H_dont_scroll
        << QPointF(initial_x, initial_y)
        << 0;

    QTestEventList key_j_scroll_down;
    key_j_scroll_down.addKeyClicks("j");
    QTest::newRow("scroll down on 'j'")
        << key_j_scroll_down
        << QPointF(initial_x, initial_y + VimEngine::scrollSizeWithHJKL())
        << VimEngine::numSteps();

    QTestEventList key_J_dont_scroll;
    key_J_dont_scroll.addKeyClicks("J");
    QTest::newRow("dont scroll down on 'J'")
        << key_J_dont_scroll
        << QPointF(initial_x, initial_y)
        << 0;

    QTestEventList key_k_scroll_up;
    key_k_scroll_up.addKeyClicks("k");
    QTest::newRow("scroll up on 'k'")
        << key_k_scroll_up
        << QPointF(initial_x, initial_y - VimEngine::scrollSizeWithHJKL())
        << VimEngine::numSteps();

    QTestEventList key_K_dont_scroll;
    key_K_dont_scroll.addKeyClicks("K");
    QTest::newRow("dont scroll up on 'K'")
        << key_K_dont_scroll
        << QPointF(initial_x, initial_y)
        << 0;

    QTestEventList key_l_scroll_right;
    key_l_scroll_right.addKeyClicks("l");
    QTest::newRow("scroll right on 'l'")
        << key_l_scroll_right
        << QPointF(initial_x + VimEngine::scrollSizeWithHJKL(), initial_y)
        << VimEngine::numSteps();

    QTestEventList key_L_dont_scroll;
    key_L_dont_scroll.addKeyClicks("L");
    QTest::newRow("dont scroll right on 'L'")
        << key_L_dont_scroll
        << QPointF(initial_x, initial_y)
        << 0;
}

void VimPluginTests::ScrollNavigationWithHJKL()
{
    QFETCH(QTestEventList, key_event);
    QFETCH(QPointF, expected_pos);
    QFETCH(int, expected_scroll_steps);

    WebView *view = nullptr;
    QString test_page("file:///" + QCoreApplication::applicationDirPath()
            + "/pages/long_page_w5000px_h5000px.html");

    loadTestPage(test_page, &view);

    qreal initial_x = 1000;
    qreal initial_y = 1000;

    view->page()->runJavaScript(QString("window.scrollTo(%1, %2);")
            .arg(initial_x).arg(initial_y));
    QTRY_COMPARE(view->page()->scrollPosition().x(), initial_x);
    QTRY_COMPARE(view->page()->scrollPosition().y(), initial_y);

    VimPlugin *vim_plugin = static_cast<VimPlugin*>(
            mApp->plugins()->getAvailablePlugins().front().instance);
    QSignalSpy spy(vim_plugin->vimEngine().scrollTimer(), SIGNAL(timeout()));
    key_event.simulate(view->parentWidget());
    QTRY_COMPARE(spy.count(), expected_scroll_steps);

    QTRY_COMPARE(view->page()->scrollPosition().x(), expected_pos.x());
    QTRY_COMPARE(view->page()->scrollPosition().y(), expected_pos.y());
}

void VimPluginTests::ScrollToTopWithDoubleLowerCaseG_data()
{
    QTest::addColumn<QTestEventList>("key_event");
    QTest::addColumn<QPointF>("expected_pos");
    QTest::addColumn<int>("expected_scroll_steps");

    qreal initial_x = 100;
    qreal initial_y = 4000;

    QTestEventList keys_gg_scroll_top;
    keys_gg_scroll_top.addKeyClicks("gg");
    QTest::newRow("scroll to top on 'gg'")
        << keys_gg_scroll_top
        << QPointF(initial_x, 0)
        << VimEngine::numSteps();

    QTestEventList keys_gjg_dont_scroll_top;
    /* Last 'a' to clean the state and not affect the next test. */
    keys_gjg_dont_scroll_top.addKeyClicks("gjga");
    QTest::newRow("dont scroll to top with 'gjg' (vim key between 'g's")
        << keys_gjg_dont_scroll_top
        << QPointF(initial_x, initial_y + VimEngine::scrollSizeWithHJKL())
        << 0;

    QTestEventList keys_gag_dont_scroll_top;
    keys_gag_dont_scroll_top.addKeyClicks("gag");
    QTest::newRow("dont scroll to top with 'gag' (non vim key between 'g's")
        << keys_gag_dont_scroll_top
        << QPointF(initial_x, initial_y)
        << 0;
}

void VimPluginTests::ScrollToTopWithDoubleLowerCaseG()
{
    QFETCH(QTestEventList, key_event);
    QFETCH(QPointF, expected_pos);
    QFETCH(int, expected_scroll_steps);

    WebView *view = nullptr;
    QString test_page("file:///" + QCoreApplication::applicationDirPath()
            + "/pages/long_page_w5000px_h5000px.html");

    loadTestPage(test_page, &view);

    qreal initial_x = 100;
    qreal initial_y = 4000;

    view->page()->runJavaScript(QString("window.scrollTo(%1, %2);")
            .arg(initial_x).arg(initial_y));
    QTRY_COMPARE(view->page()->scrollPosition().x(), initial_x);
    QTRY_COMPARE(view->page()->scrollPosition().y(), initial_y);

    VimPlugin *vim_plugin = static_cast<VimPlugin*>(
            mApp->plugins()->getAvailablePlugins().front().instance);
    QSignalSpy spy(vim_plugin->vimEngine().scrollTimer(), SIGNAL(timeout()));
    key_event.simulate(view->parentWidget());
    QTRY_COMPARE(spy.count(), expected_scroll_steps);
    QTRY_COMPARE(view->page()->scrollPosition().x(), expected_pos.x());
    QTRY_COMPARE(view->page()->scrollPosition().y(), expected_pos.y());
}

void VimPluginTests::ScrollToBottomWithCapitalG()
{
    WebView *view = nullptr;
    QString test_page("file:///" + QCoreApplication::applicationDirPath()
            + "/pages/long_page_w5000px_h5000px.html");

    loadTestPage(test_page, &view);

    qreal initial_x = 100;
    qreal initial_y = 0;

    view->page()->runJavaScript(QString("window.scrollTo(%1, %2);")
            .arg(initial_x).arg(initial_y));
    QTRY_COMPARE(view->page()->scrollPosition().x(), initial_x);
    QTRY_COMPARE(view->page()->scrollPosition().y(), initial_y);

    qreal page_y_offset = 0;
    qreal window_height = 0;
    qreal scroll_height = 0;

    view->page()->runJavaScript(
        QString(
            "(function() {"
            "   var res = {"
            "       page_y_offset: window.pageYOffset,"
            "       window_height: document.documentElement.clientHeight,"
            "       scroll_height: document.body.scrollHeight"
            "   };"
            "   return res;"
            "})()"),
        [&page_y_offset, &window_height, &scroll_height](const QVariant& res) {
            const QVariantMap map = res.toMap();
            page_y_offset = map.value(QString("page_y_offset")).toReal();
            window_height = map.value(QString("window_height")).toReal();
            scroll_height = map.value(QString("scroll_height")).toReal();
        });

    QTRY_VERIFY(scroll_height != page_y_offset + window_height);

    VimPlugin *vim_plugin = static_cast<VimPlugin*>(
            mApp->plugins()->getAvailablePlugins().front().instance);
    QSignalSpy spy(vim_plugin->vimEngine().scrollTimer(), SIGNAL(timeout()));
    QTest::keyClick(view->parentWidget(), 'G');
    QTRY_COMPARE(spy.count(), VimEngine::numSteps());
    QCOMPARE(view->page()->scrollPosition().x(), qreal(initial_x));

    view->page()->runJavaScript(
        QString("window.pageYOffset"),
        [&page_y_offset](const QVariant& res) {
            page_y_offset = res.toReal();
        });
    QTRY_VERIFY(scroll_height == page_y_offset + window_height);
}

void VimPluginTests::ScrollHalfViewportUpWithLowerCaseU()
{
    WebView *view = nullptr;
    QString test_page("file:///" + QCoreApplication::applicationDirPath()
            + "/pages/long_page_w5000px_h5000px.html");

    loadTestPage(test_page, &view);

    qreal initial_x = 2000;
    qreal initial_y = 4000;

    view->page()->runJavaScript(QString("window.scrollTo(%1, %2);")
            .arg(initial_x).arg(initial_y));
    QTRY_COMPARE(view->page()->scrollPosition().x(), initial_x);
    QTRY_COMPARE(view->page()->scrollPosition().y(), initial_y);

    int scroll_step_size = view->page()->execJavaScript(
            QString("(document.documentElement.clientHeight / 2) / %1")
                .arg(VimEngine::numSteps())).toInt();

    VimPlugin *vim_plugin = static_cast<VimPlugin*>(
            mApp->plugins()->getAvailablePlugins().front().instance);
    QSignalSpy spy(vim_plugin->vimEngine().scrollTimer(), SIGNAL(timeout()));
    /* Since the scroll currently starts in VimEngine only after processing
     * the javascript request using 'QTest::keyClick' makes keyRelased event
     * to be generated before the scroll being started.
     */
    QTest::keyPress(view->parentWidget(), 'u');
    QTest::keyRelease(view->parentWidget(), 'u', Qt::NoModifier, 50);
    QTRY_COMPARE(spy.count(), VimEngine::numSteps());

    QTRY_COMPARE(view->page()->scrollPosition().x(), initial_x);
    QTRY_COMPARE(view->page()->scrollPosition().y(),
            initial_y - (VimEngine::numSteps() * scroll_step_size));
}

void VimPluginTests::ScrollHalfViewportDownWithLowerCaseD()
{
    WebView *view = nullptr;
    QString test_page("file:///" + QCoreApplication::applicationDirPath()
            + "/pages/long_page_w5000px_h5000px.html");

    loadTestPage(test_page, &view);

    qreal initial_x = 100;
    qreal initial_y = 100;

    view->page()->runJavaScript(QString("window.scrollTo(%1, %2);")
            .arg(initial_x).arg(initial_y));
    QTRY_COMPARE(view->page()->scrollPosition().x(), initial_x);
    QTRY_COMPARE(view->page()->scrollPosition().y(), initial_y);

    int scroll_step_size = view->page()->execJavaScript(
            QString("(document.documentElement.clientHeight / 2) / %1")
                .arg(VimEngine::numSteps())).toInt();

    VimPlugin *vim_plugin = static_cast<VimPlugin*>(
            mApp->plugins()->getAvailablePlugins().front().instance);
    QSignalSpy spy(vim_plugin->vimEngine().scrollTimer(), SIGNAL(timeout()));
    /* Since the scroll currently starts in VimEngine only after processing
     * the javascript request using 'QTest::keyClick' makes keyRelased event
     * to be generated before the scroll being started.
     */
    QTest::keyPress(view->parentWidget(), 'd');
    QTest::keyRelease(view->parentWidget(), 'd', Qt::NoModifier, 50);
    QTRY_COMPARE(spy.count(), VimEngine::numSteps());

    QTRY_COMPARE(view->page()->scrollPosition().x(), initial_x);
    QTRY_COMPARE(view->page()->scrollPosition().y(),
            initial_y + (VimEngine::numSteps() * scroll_step_size));
}

/* Using "APPLESS" version because MainApplication is already a QApplication
 * and it was not coping well with QTEST_MAIN.
 */
QTEST_APPLESS_MAIN(VimPluginTests)
#include "VimPluginTests.moc"
