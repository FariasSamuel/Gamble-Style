#include <iostream>
#include <cppunit/TestCase.h>
#include <cppunit/TestFixture.h>
#include <cppunit/ui/text/TextTestRunner.h>
#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/TestResult.h>
#include <cppunit/TestResultCollector.h>
#include <cppunit/TestRunner.h>
#include <cppunit/BriefTestProgressListener.h>
#include <cppunit/CompilerOutputter.h>
#include <cppunit/XmlOutputter.h>
#include "CasePrison.hpp"
#include "CarteSortiePrison.hpp"
#include "Joueur.hpp"

using namespace CppUnit;
using namespace std;

class TestCasePrison : public CppUnit::TestFixture
{
    CPPUNIT_TEST_SUITE(TestCasePrison);
    CPPUNIT_TEST(testActionAvecCarteSortiePrisonLibereJoueur);
    CPPUNIT_TEST(testActionDoubleLibereJoueur);
    CPPUNIT_TEST(testActionSansDoubleIncrementeTour);
    CPPUNIT_TEST(testActionTroisToursSansDoubleLibereJoueur);
    CPPUNIT_TEST_SUITE_END();

public:
    void setUp(void);
    void tearDown(void);

protected:
    // action() : joueur avec carte sortie prison => libere sans lancer les des
    void testActionAvecCarteSortiePrisonLibereJoueur(void);

    // action() : un double aux des libere le joueur
    void testActionDoubleLibereJoueur(void);

    // action() : sans double, le compteur de tours en prison s'incremente
    void testActionSansDoubleIncrementeTour(void);

    // action() : apres 3 tours sans double, le compteur repasse a 0 et le joueur est libere
    void testActionTroisToursSansDoubleLibereJoueur(void);

private:
    CasePrison        *mCasePrison;
    Joueur            *mJoueur;
    CarteSortiePrison *mCarte;
};

// ---------------------------------------------------------------------------

void TestCasePrison::setUp(void)
{
    mJoueur    = new Joueur("Alice", 1500);
    mCasePrison = new CasePrison();
    mCasePrison->setJoueurActif(mJoueur);
    mCarte     = new CarteSortiePrison(mJoueur);
}

void TestCasePrison::tearDown(void)
{
    delete mCasePrison;
    delete mJoueur;
    delete mCarte;
}

// ---------------------------------------------------------------------------
// action() : si le joueur possede une CarteSortiePrison et choisit de l'utiliser,
//            son compteur de prison reste a 0 (liberation immediate)
void TestCasePrison::testActionAvecCarteSortiePrisonLibereJoueur(void)
{
    mJoueur->donnerCarte(mCarte);
    mCasePrison->setUtiliserCarteReponse(true); // simule le choix "oui"
    mCasePrison->action();
    CPPUNIT_ASSERT(mCasePrison->getTourPrison(mJoueur) == 0);
}

// ---------------------------------------------------------------------------
// action() : lancer un double (simule) => le joueur est libere (compteur = 0)
void TestCasePrison::testActionDoubleLibereJoueur(void)
{
    mCasePrison->forcerResultatDes(3, 3); // double 3
    mCasePrison->action();
    CPPUNIT_ASSERT(mCasePrison->getTourPrison(mJoueur) == 0);
}

// ---------------------------------------------------------------------------
// action() : sans double, le compteur en prison augmente de 1
void TestCasePrison::testActionSansDoubleIncrementeTour(void)
{
    mCasePrison->forcerResultatDes(2, 5); // pas de double
    int tourAvant = mCasePrison->getTourPrison(mJoueur);
    mCasePrison->action();
    CPPUNIT_ASSERT(mCasePrison->getTourPrison(mJoueur) == tourAvant + 1);
}

// ---------------------------------------------------------------------------
// action() x3 sans double : au 3eme tour le compteur repasse a 0
void TestCasePrison::testActionTroisToursSansDoubleLibereJoueur(void)
{
    mCasePrison->forcerResultatDes(1, 4); // pas de double
    mCasePrison->action(); // tour 1
    mCasePrison->action(); // tour 2
    mCasePrison->action(); // tour 3 => remise a 0
    CPPUNIT_ASSERT(mCasePrison->getTourPrison(mJoueur) == 0);
}

// ---------------------------------------------------------------------------

CPPUNIT_TEST_SUITE_REGISTRATION(TestCasePrison);

int main(int argc, char* argv[])
{
    CPPUNIT_NS::TestResult testresult;

    CPPUNIT_NS::TestResultCollector collectedresults;
    testresult.addListener(&collectedresults);

    CPPUNIT_NS::BriefTestProgressListener progress;
    testresult.addListener(&progress);

    CPPUNIT_NS::TestRunner testrunner;
    testrunner.addTest(CPPUNIT_NS::TestFactoryRegistry::getRegistry().makeTest());
    testrunner.run(testresult);

    CPPUNIT_NS::CompilerOutputter compileroutputter(&collectedresults, std::cerr);
    compileroutputter.write();

    ofstream xmlFileOut("cppTestCasePrisonResults.xml");
    XmlOutputter xmlOut(&collectedresults, xmlFileOut);
    xmlOut.write();

    return collectedresults.wasSuccessful() ? 0 : 1;
}
