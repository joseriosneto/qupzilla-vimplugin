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

        /* This code is borrowed from 'execJavaScript' of QupZilla. */
        QVariant getElementAttrValue(WebPage *page, const QString& element)
        {
            QPointer<QEventLoop> loop = new QEventLoop;
            QVariant value = -1;

            QTimer::singleShot(500, loop.data(), &QEventLoop::quit);

            page->runJavaScript(element, [loop, &value] (const QVariant& res) {
                if (loop && loop->isRunning()) {
                    value = res;
                    loop->quit();
                }
            });

            loop->exec(QEventLoop::ExcludeUserInputEvents
                    | QEventLoop::ExcludeSocketNotifiers);
            delete loop;

            return value;
        }

        void processEvents(int ms_maxtime = 200)
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

    VimPlugin vim_plugin;
    int initial_x = 100;
    int initial_y = 100;

    QTestEventList key_h_scroll_left;
    key_h_scroll_left.addKeyClick(Qt::Key_H);
    QTest::newRow("scroll left on 'h'") << key_h_scroll_left
        << QPointF(initial_x - vim_plugin.singleStepSize(), initial_y);

    QTestEventList key_j_scroll_down;
    key_j_scroll_down.addKeyClick(Qt::Key_J);
    QTest::newRow("scroll down on 'j'") << key_j_scroll_down
        << QPointF(initial_x, initial_y + vim_plugin.singleStepSize());

    QTestEventList key_k_scroll_up;
    key_k_scroll_up.addKeyClick(Qt::Key_K);
    QTest::newRow("scroll up on 'k'") << key_k_scroll_up
        << QPointF(initial_x, initial_y - vim_plugin.singleStepSize());

    QTestEventList key_l_scroll_right;
    key_l_scroll_right.addKeyClick(Qt::Key_L);
    QTest::newRow("scroll right on 'l'") << key_l_scroll_right
        << QPointF(initial_x + vim_plugin.singleStepSize(), initial_y);
}

void VimPluginTests::ScrollNavigationWithHJKL()
{
    QFETCH(QTestEventList, key_event);
    QFETCH(QPointF, expected_pos);

    WebView *view = nullptr;
    QString test_page("file:///" + QCoreApplication::applicationDirPath()
            + "/pages/long_page_w5000px_h5000px.html");

    loadTestPage(test_page, &view);

    view->page()->runJavaScript("window.scrollTo(100, 100);");
    QTRY_COMPARE(view->page()->scrollPosition().x(), qreal(100));
    QTRY_COMPARE(view->page()->scrollPosition().y(), qreal(100));

    key_event.simulate(view->parentWidget());
    processEvents();
    QTRY_COMPARE(view->page()->scrollPosition().x(), expected_pos.x());
    QTRY_COMPARE(view->page()->scrollPosition().y(), expected_pos.y());
}

void VimPluginTests::ScrollToTopWithDoubleLowerCaseG_data()
{
    QTest::addColumn<QTestEventList>("key_event");
    QTest::addColumn<QPointF>("expected_pos");

    /* Initial position: (x = 100, y = 500) */

    QTestEventList keys_gg_scroll_top;
    keys_gg_scroll_top.addKeyClicks("gg");
    QTest::newRow("scroll to top on 'gg'") << keys_gg_scroll_top
        << QPointF(100, 0);

    QTestEventList keys_gjg_dont_scroll_top;
    /* Last 'a' to clean the state and not affect the next test. */
    keys_gjg_dont_scroll_top.addKeyClicks("gjga");
    QTest::newRow("dont scroll to top with 'gjg' (vim key between 'g's")
        << keys_gjg_dont_scroll_top << QPointF(100, 530);

    QTestEventList keys_gag_dont_scroll_top;
    keys_gag_dont_scroll_top.addKeyClicks("gag");
    QTest::newRow("dont scroll to top with 'gag' (non vim key between 'g's")
        << keys_gag_dont_scroll_top << QPointF(100, 500);
}

void VimPluginTests::ScrollToTopWithDoubleLowerCaseG()
{
    QFETCH(QTestEventList, key_event);
    QFETCH(QPointF, expected_pos);

    WebView *view = nullptr;
    QString test_page("file:///" + QCoreApplication::applicationDirPath()
            + "/pages/long_page_w5000px_h5000px.html");

    loadTestPage(test_page, &view);

    view->page()->runJavaScript("window.scrollTo(100, 500);");
    QTRY_COMPARE(view->page()->scrollPosition().x(), qreal(100));
    QTRY_COMPARE(view->page()->scrollPosition().y(), qreal(500));

    key_event.simulate(view->parentWidget());
    processEvents();
    QTRY_COMPARE(view->page()->scrollPosition().x(), expected_pos.x());
    QTRY_COMPARE(view->page()->scrollPosition().y(), expected_pos.y());
}

void VimPluginTests::ScrollToBottomWithCapitalG()
{
    WebView *view = nullptr;
    QString test_page("file:///" + QCoreApplication::applicationDirPath()
            + "/pages/long_page_w5000px_h5000px.html");

    loadTestPage(test_page, &view);

    view->page()->runJavaScript("window.scrollTo(100, 0);");
    QTRY_COMPARE(view->page()->scrollPosition().x(), qreal(100));
    QTRY_COMPARE(view->page()->scrollPosition().y(), qreal(0));

    qreal client_height = getElementAttrValue(view->page(),
            "document.body.clientHeight;").toReal();
    qreal scroll_height = getElementAttrValue(view->page(),
            "document.body.scrollHeight;").toReal();
    qreal scroll_top = getElementAttrValue(view->page(),
            "document.body.scrollTop;").toReal();

    QVERIFY(scroll_height != scroll_top + client_height);

    QTest::keyClick(view->parentWidget(), 'G');
    processEvents();
    QTRY_COMPARE(view->page()->scrollPosition().x(), qreal(100));

    scroll_top = getElementAttrValue(view->page(),
            "document.body.scrollTop;").toReal();
    QVERIFY(scroll_height == scroll_top + client_height);
}

/* Using "APPLESS" version because MainApplication is already a QApplication
 * and it was not coping well with QTEST_MAIN.
 */
QTEST_APPLESS_MAIN(VimPluginTests)
