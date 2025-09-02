#include <QtTest>

// add necessary includes here

class BoardStationTests : public QObject
{
    Q_OBJECT

public:
    BoardStationTests();
    ~BoardStationTests();

private slots:
    void test_case1();
};

BoardStationTests::BoardStationTests() {}

BoardStationTests::~BoardStationTests() {}

void BoardStationTests::test_case1() {}

QTEST_APPLESS_MAIN(BoardStationTests)

#include "tst_boardstationtests.moc"
