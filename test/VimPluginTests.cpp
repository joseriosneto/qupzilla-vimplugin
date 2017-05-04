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
#include "tabwidget.h"
#include "settings.h"
#include "datapaths.h"

#define TEST_PROFILE "VimPluginTests"
#define TEST_PAGE "w5000px_h5000px.html"
#define TEST_PAGE_2 "page_2.html"
#define TEST_PAGE_FILEPATH "/tmp/" TEST_PAGE
#define TEST_PAGE_2_FILEPATH "/tmp/" TEST_PAGE_2

class VimPluginTests : public QObject
{
    Q_OBJECT

    private slots:
        void initTestCase()
        {
            startMainApplication();
            loadVimPlugin();

            /* Copying resource test pages to "tmp" in order to make them
             * accessible to QUrl afterwards.
             */
            QFile::copy(":/vimplugin/" TEST_PAGE, TEST_PAGE_FILEPATH);
            QFile::copy(":/vimplugin/" TEST_PAGE_2, TEST_PAGE_2_FILEPATH);
        }

        void cleanupTestCase()
        {
            QSignalSpy spy(m_app->getWindow(), SIGNAL(destroyed(QObject*)));
            QCoreApplication::postEvent(m_app->getWindow(), new QCloseEvent);
            spy.wait(1000);
            delete m_app;

            QFile::remove(TEST_PAGE_FILEPATH);
            QFile::remove(TEST_PAGE_2_FILEPATH);
            QDir(DataPaths::currentProfilePath()).removeRecursively();
        }

        void init()
        {
            m_vim_plugin->init();
            loadTestPage();
        }

        void cleanup()
        {
            /* Create a new tab and close all others. This is done to
             * avoid dependencies among tests as much as possible.
             */
            BrowserWindow *window = m_app->getWindow();
            TabWidget *tab_widget = window->tabWidget();
            tab_widget->addView(QUrl(), Qz::NT_CleanSelectedTabAtTheEnd);
            tab_widget->setCurrentIndex(tab_widget->lastTabIndex());
            tab_widget->closeAllButCurrent(tab_widget->lastTabIndex());
        }

        void PluginSpecHasCorrectData();
        void ProcessOnlyONWebViewEvents();
        void LoadPluginOnlyOnCompatibleVersions();

        void ScrollNavigationWithHJKL_data();
        void ScrollNavigationWithHJKL();

        void ScrollToTopWithDoubleLowerCaseG_data();
        void ScrollToTopWithDoubleLowerCaseG();

        void ScrollToBottomWithCapitalG();

        void ScrollHalfViewportUpWithLowerCaseU_data();
        void ScrollHalfViewportUpWithLowerCaseU();

        void ScrollHalfViewportDownWithLowerCaseD_data();
        void ScrollHalfViewportDownWithLowerCaseD();

        void ReloadPageWithLowerCaseR();

        void TabIterationOnShiftJK_data();
        void TabIterationOnShiftJK();

        void CloseCurTabOnLowerCaseX();

        void StopScrollingWhenPageIsClosed();

        void RestoreClosedTabOnCapitalX();

    private:
        void startMainApplication()
        {
            /* The main idea on these args are to (a) avoid tests be affected by
             * other extensions and (b) other running instances of QupZilla, and
             * (c) avoid leaving unnecessary files in filesystem after tests.
             *
             * --no-remote:         avoid being affected by others running
             *                      QupZilla instances.
             *
             * --profile:           load only VimPlugin extension and avoid
             *                      messing current profile.
             */
            char *argv[] = {
                qstrdup("qupzilla"),
                qstrdup("--no-remote"),
                qstrdup("--profile=" TEST_PROFILE)
            };
            int argc = sizeof(argv) / sizeof(argv[0]);

            m_app = new MainApplication(argc, argv);

            /* QupZilla has some 'postLaunch' initialization functions that are
             * posted in event loop queue to be processed on the next(s)
             * iteration(s).
             */
            QTestEventLoop::instance().enterLoop(1);

            for (int i = 0; i < argc; ++i)
                free(argv[i]);
        }

        void loadVimPlugin()
        {
            QFileInfo lib_plugin(QCoreApplication::applicationDirPath()
                    + "/libVimPlugin.so");

            Settings settings;
            settings.beginGroup("Plugin-Settings");
            settings.setValue("EnablePlugins", true);
            settings.setValue("AllowedPlugins", lib_plugin.absoluteFilePath());
            settings.endGroup();
            settings.syncSettings();

            m_app->plugins()->loadSettings();
            m_app->plugins()->loadPlugins();

            m_vim_plugin = nullptr;
            foreach (auto plugin, mApp->plugins()->getAvailablePlugins()) {
                m_vim_plugin = dynamic_cast<VimPlugin*>(plugin.instance);
                if (m_vim_plugin)
                    break;
            }

            if (!m_vim_plugin)
                QFAIL("VimPlugin is not loaded in current QupZilla's profile!");
        }

        void loadTestPage()
        {
            QTRY_VERIFY(m_app->getWindow());
            BrowserWindow *window = m_app->getWindow();

            QTRY_VERIFY(window->weView());
            TabbedWebView *tab_view = window->weView();

            tab_view->show();
            QTest::qWaitForWindowExposed(tab_view);

            QSignalSpy loadSpy(tab_view->page(), SIGNAL(loadFinished(bool)));
            tab_view->load(QUrl::fromLocalFile(TEST_PAGE_FILEPATH));
            QTRY_COMPARE(loadSpy.count(), 1);

            m_cur_view = tab_view;
        }

        void setPagePosition(qreal x, qreal y)
        {
            m_cur_view->page()->runJavaScript(
                    QString("window.scrollTo(%1, %2);").arg(x).arg(y));
            QTRY_COMPARE(m_cur_view->page()->scrollPosition().x(), x);
            QTRY_COMPARE(m_cur_view->page()->scrollPosition().y(), y);
        }

        MainApplication *m_app;
        WebView *m_cur_view;
        VimPlugin *m_vim_plugin;
};

void VimPluginTests::PluginSpecHasCorrectData()
{
    PluginSpec spec;

    spec.name = "Vim plugin";
    spec.info =  "Enable vim command set for browsing.";
    spec.description = "How awesome is Vimium? This plugin try to port its "
                "functionalities to QupZilla.";
    spec.version = "0.0.1";
    spec.author = "Jose Rios <joseriosneto@gmail.com>";

    QVERIFY(spec == m_vim_plugin->pluginSpec());
}

void VimPluginTests::ProcessOnlyONWebViewEvents()
{
    QVERIFY(!m_vim_plugin->keyPress(Qz::ON_WebView, nullptr, nullptr));
}

void VimPluginTests::LoadPluginOnlyOnCompatibleVersions()
{
    QVERIFY(m_vim_plugin->testPlugin());
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

    QTestEventList key_h_scroll_once_leftwards;
    key_h_scroll_once_leftwards.addKeyPress('h');
    key_h_scroll_once_leftwards.addKeyRelease('H', Qt::ShiftModifier, 20);
    QTest::newRow("scroll left once when pressing 'h' and releasing shift+H")
        << key_h_scroll_once_leftwards
        << QPointF(initial_x - VimEngine::scrollSizeWithHJKL(), initial_y)
        << VimEngine::numSteps();

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

    QTestEventList key_j_scroll_once_downwards;
    key_j_scroll_once_downwards.addKeyPress('j');
    key_j_scroll_once_downwards.addKeyRelease('J', Qt::ShiftModifier, 20);
    QTest::newRow("scroll down once when pressing 'j' and releasing shift+J")
        << key_j_scroll_once_downwards
        << QPointF(initial_x, initial_y + VimEngine::scrollSizeWithHJKL())
        << VimEngine::numSteps();

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

    QTestEventList key_k_scroll_once_upwards;
    key_k_scroll_once_upwards.addKeyPress('k');
    key_k_scroll_once_upwards.addKeyRelease('K', Qt::ShiftModifier, 20);
    QTest::newRow("scroll up once when pressing 'k' and releasing shift+K")
        << key_k_scroll_once_upwards
        << QPointF(initial_x, initial_y - VimEngine::scrollSizeWithHJKL())
        << VimEngine::numSteps();

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

    QTestEventList key_l_scroll_once_rightwards;
    key_l_scroll_once_rightwards.addKeyPress('l');
    key_l_scroll_once_rightwards.addKeyRelease('L', Qt::ShiftModifier, 20);
    QTest::newRow("scroll right once when pressing 'l' and releasing shift+L")
        << key_l_scroll_once_rightwards
        << QPointF(initial_x + VimEngine::scrollSizeWithHJKL(), initial_y)
        << VimEngine::numSteps();
}

void VimPluginTests::ScrollNavigationWithHJKL()
{
    QFETCH(QTestEventList, key_event);
    QFETCH(QPointF, expected_pos);
    QFETCH(int, expected_scroll_steps);

    setPagePosition(1000, 1000);

    QSignalSpy spy(m_vim_plugin->vimEngine().scrollTimer(), SIGNAL(timeout()));
    key_event.simulate(m_cur_view->parentWidget());
    QTRY_COMPARE(spy.count(), expected_scroll_steps);

    QTRY_COMPARE(m_cur_view->page()->scrollPosition().x(), expected_pos.x());
    QTRY_COMPARE(m_cur_view->page()->scrollPosition().y(), expected_pos.y());
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
    keys_gjg_dont_scroll_top.addKeyClicks("gjg");
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

    setPagePosition(100, 4000);

    QSignalSpy spy(m_vim_plugin->vimEngine().scrollTimer(), SIGNAL(timeout()));
    key_event.simulate(m_cur_view->parentWidget());
    QTRY_COMPARE(spy.count(), expected_scroll_steps);
    QTRY_COMPARE(m_cur_view->page()->scrollPosition().x(), expected_pos.x());
    QTRY_COMPARE(m_cur_view->page()->scrollPosition().y(), expected_pos.y());
}

void VimPluginTests::ScrollToBottomWithCapitalG()
{
    qreal initial_x = 100;
    qreal initial_y = 0;
    qreal page_y_offset = 0;
    qreal window_height = 0;
    qreal scroll_height = 0;

    setPagePosition(initial_x, initial_y);

    m_cur_view->page()->runJavaScript(
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

    QSignalSpy spy(m_vim_plugin->vimEngine().scrollTimer(), SIGNAL(timeout()));
    QTest::keyClick(m_cur_view->parentWidget(), 'G');
    QTRY_COMPARE(spy.count(), VimEngine::numSteps());
    QCOMPARE(m_cur_view->page()->scrollPosition().x(), qreal(initial_x));

    m_cur_view->page()->runJavaScript(
        QString("window.pageYOffset"),
        [&page_y_offset](const QVariant& res) {
            page_y_offset = res.toReal();
        });
    QTRY_VERIFY(scroll_height == page_y_offset + window_height);
}

void VimPluginTests::ScrollHalfViewportUpWithLowerCaseU_data()
{
    QTest::addColumn<QTestEventList>("key_event");
    QTest::addColumn<int>("expected_scroll_steps");

    QTestEventList key_u_scroll_half_page_up;
    key_u_scroll_half_page_up.addKeyClicks("u");
    QTest::newRow("scroll half page up on 'u'")
        << key_u_scroll_half_page_up
        << VimEngine::numSteps();

    QTestEventList key_u_scroll_once_half_page_up;
    key_u_scroll_once_half_page_up.addKeyPress('u');
    key_u_scroll_once_half_page_up.addKeyRelease('U', Qt::ShiftModifier);
    QTest::newRow("scroll half page up once when pressing 'u' and releasing shift+U")
        << key_u_scroll_once_half_page_up
        << VimEngine::numSteps();
}

void VimPluginTests::ScrollHalfViewportUpWithLowerCaseU()
{
    QFETCH(QTestEventList, key_event);
    QFETCH(int, expected_scroll_steps);

    qreal initial_x = 2000;
    qreal initial_y = 4000;

    setPagePosition(initial_x, initial_y);

    const QRect viewport_size = m_cur_view->geometry();
    int scroll_step_size = (viewport_size.height() / 2) / VimEngine::numSteps();

    QSignalSpy spy(m_vim_plugin->vimEngine().scrollTimer(), SIGNAL(timeout()));
    key_event.simulate(m_cur_view->parentWidget());
    QTRY_COMPARE(spy.count(), expected_scroll_steps);

    QTRY_COMPARE(m_cur_view->page()->scrollPosition().x(), initial_x);
    QTRY_COMPARE(m_cur_view->page()->scrollPosition().y(),
            initial_y - (VimEngine::numSteps() * scroll_step_size));
}

void VimPluginTests::ScrollHalfViewportDownWithLowerCaseD_data()
{
    QTest::addColumn<QTestEventList>("key_event");
    QTest::addColumn<int>("expected_scroll_steps");

    QTestEventList key_d_scroll_half_page_down;
    key_d_scroll_half_page_down.addKeyClicks("d");
    QTest::newRow("scroll half page down on 'd'")
        << key_d_scroll_half_page_down
        << VimEngine::numSteps();

    QTestEventList key_d_scroll_once_half_page_down;
    key_d_scroll_once_half_page_down.addKeyPress('d');
    key_d_scroll_once_half_page_down.addKeyRelease('D', Qt::ShiftModifier);
    QTest::newRow("scroll half page down once when pressing 'd' and releasing shift+D")
        << key_d_scroll_once_half_page_down
        << VimEngine::numSteps();
}

void VimPluginTests::ScrollHalfViewportDownWithLowerCaseD()
{
    QFETCH(QTestEventList, key_event);
    QFETCH(int, expected_scroll_steps);

    qreal initial_x = 100;
    qreal initial_y = 100;

    setPagePosition(initial_x, initial_y);

    const QRect viewport_size = m_cur_view->geometry();
    int scroll_step_size = (viewport_size.height() / 2) / VimEngine::numSteps();

    QSignalSpy spy(m_vim_plugin->vimEngine().scrollTimer(), SIGNAL(timeout()));
    key_event.simulate(m_cur_view->parentWidget());
    QTRY_COMPARE(spy.count(), expected_scroll_steps);

    QTRY_COMPARE(m_cur_view->page()->scrollPosition().x(), initial_x);
    QTRY_COMPARE(m_cur_view->page()->scrollPosition().y(),
            initial_y + (VimEngine::numSteps() * scroll_step_size));
}

void VimPluginTests::ReloadPageWithLowerCaseR()
{
    setPagePosition(100, 100);

    QSignalSpy spy(m_cur_view, SIGNAL(loadStarted()));
    QTest::keyClick(m_cur_view->parentWidget(), 'r');
    QTRY_COMPARE(spy.count(), 1);
}

void VimPluginTests::TabIterationOnShiftJK_data()
{
    QTest::addColumn<QTestEventList>("key_event");
    QTest::addColumn<int>("expected_tab_count");
    QTest::addColumn<int>("expected_current_tab");

    QTestEventList key_K;
    key_K.addKeyClicks("K");
    QTest::newRow("next tab on 'K'") << key_K << 2 << 1;

    QTestEventList key_J;
    key_J.addKeyClicks("J");
    QTest::newRow("previous tab on 'J'") << key_J << 2 << 1;
}

void VimPluginTests::TabIterationOnShiftJK()
{
    QFETCH(QTestEventList, key_event);
    QFETCH(int, expected_tab_count);
    QFETCH(int, expected_current_tab);

    TabWidget* tab_widget = nullptr;

    int initial_tab_count = 1;
    int initial_tab = 0;
    tab_widget =
        static_cast<TabbedWebView *>(m_cur_view)->browserWindow()->tabWidget();

    QTRY_COMPARE(tab_widget->normalTabsCount(), initial_tab_count);
    QTRY_COMPARE(tab_widget->currentIndex(), initial_tab);
    tab_widget->addView(QUrl::fromLocalFile(TEST_PAGE_FILEPATH),
            Qz::NT_CleanSelectedTabAtTheEnd);
    tab_widget->setCurrentIndex(initial_tab);
    QTRY_COMPARE(tab_widget->normalTabsCount(), expected_tab_count);
    QTRY_COMPARE(tab_widget->currentIndex(), initial_tab);

    key_event.simulate(m_cur_view->parentWidget());
    QTRY_COMPARE(tab_widget->currentIndex(), expected_current_tab);
}

void VimPluginTests::CloseCurTabOnLowerCaseX()
{
    TabWidget* tab_widget = nullptr;

    int initial_tab_count = 1;
    tab_widget =
        static_cast<TabbedWebView *>(m_cur_view)->browserWindow()->tabWidget();

    QTRY_COMPARE(tab_widget->normalTabsCount(), initial_tab_count);
    tab_widget->addView(QUrl::fromLocalFile(TEST_PAGE_FILEPATH),
            Qz::NT_CleanSelectedTabAtTheEnd);
    QTRY_COMPARE(tab_widget->normalTabsCount(), initial_tab_count + 1);

    QTest::keyClick(m_cur_view->parentWidget(), 'x');
    QTRY_COMPARE(tab_widget->normalTabsCount(), initial_tab_count);
}

void VimPluginTests::StopScrollingWhenPageIsClosed()
{
    TabWidget* tab_widget = nullptr;

    int initial_tab_count = 1;
    tab_widget =
        static_cast<TabbedWebView *>(m_cur_view)->browserWindow()->tabWidget();

    QTRY_COMPARE(tab_widget->normalTabsCount(), initial_tab_count);
    tab_widget->addView(QUrl::fromLocalFile(TEST_PAGE_FILEPATH),
            Qz::NT_CleanSelectedTabAtTheEnd);
    QTRY_COMPARE(tab_widget->normalTabsCount(), initial_tab_count + 1);

    QSignalSpy spy(m_vim_plugin->vimEngine().scrollTimer(), SIGNAL(timeout()));
    /* For some reason QupZilla sends me the first tab page in KeyPress here.
     * Not sure why yet. It is needed then to close the first tab to simulate
     * the desired behaviour.
     */
    QTest::keyPress(m_cur_view->parentWidget(), 'j');
    tab_widget->requestCloseTab(0);
    QTRY_COMPARE(tab_widget->normalTabsCount(), initial_tab_count);
}

void VimPluginTests::RestoreClosedTabOnCapitalX()
{
    const QUrl url_test_page = QUrl::fromLocalFile(TEST_PAGE_2_FILEPATH);
    BrowserWindow *browser_window = nullptr;
    TabWidget* tab_widget = nullptr;

    browser_window = static_cast<TabbedWebView *>(m_cur_view)->browserWindow();
    tab_widget = browser_window->tabWidget();

    tab_widget->addView(QUrl(), Qz::NT_CleanSelectedTabAtTheEnd);
    QTRY_VERIFY(browser_window->weView(1));

    QSignalSpy loadSpy(browser_window->weView(1)->page(),
            SIGNAL(loadFinished(bool)));
    browser_window->weView(1)->load(url_test_page);
    QTRY_COMPARE(loadSpy.count(), 1);

    /* Using "requestCloseTab" instead of
     * "keyClick(browser_window->weView(1)->parentWidget(), 'x')" to avoid any
     * flaw on 'x' feature break this test.
     */
    QSignalSpy changedSpy(tab_widget, SIGNAL(changed()));
    tab_widget->requestCloseTab(1);
    QTRY_VERIFY(changedSpy.count() >= 1);

    QTest::keyClick(browser_window->weView()->parentWidget(), 'X');
    QTRY_COMPARE(tab_widget->normalTabsCount(), 2);
    QTRY_COMPARE(browser_window->weView(1)->page()->url(), url_test_page);
}

/* Using "APPLESS" version because MainApplication is already a QApplication
 * and it was not coping well with QTEST_MAIN.
 */
QTEST_APPLESS_MAIN(VimPluginTests)
#include "VimPluginTests.moc"
