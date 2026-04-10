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
#include "CaseDepart.hpp"
#include "Joueur.hpp"

using namespace CppUnit;
using namespace std;

class TestCaseDepart : public CppUnit::TestFixture
{
    CPPUNIT_TEST_SUITE(TestCaseDepart);
    CPPUNIT_TEST(testActionDonneCapitalAuJoueur);
    CPPUNIT_TEST(testActionMontantExact);
    CPPUNIT_TEST(testActionSansEffetSiJoueurNePasse Pas);
    CPPUNIT_TEST_SUITE_END();

public:
    void setUp(void);
    void tearDown(void);

protected:
    // action() : le joueur reoit le montant defini par la case Depart
    void testActionDonneCapitalAuJoueur(void);

    // action() : le montant recu correspond exactement a l'attribut 'montant'
    void testActionMontantExact(void);

    // action() : sans joueur actif, aucun capital ne change
    void testActionSansEffetSiJoueurNePasse(void);

private:
    CaseDepart *mCaseDepart;
    Joueur     *mJoueur;
};

// ---------------------------------------------------------------------------

void TestCaseDepart::setUp(void)
{
    mCaseDepart = new CaseDepart(200); // 200 = montant classique Monopoly
    mJoueur     = new Joueur("Alice", 1500);
    mCaseDepart->setJoueurActif(mJoueur);
}

void TestCaseDepart::tearDown(void)
{
    delete mCaseDepart;
    delete mJoueur;
}

// ---------------------------------------------------------------------------
// action() : le capital du joueur doit augmenter apres passage sur la case Depart
void TestCaseDepart::testActionDonneCapitalAuJoueur(void)
{
    int capitalAvant = mJoueur->getCapital();
    mCaseDepart->action();
    CPPUNIT_ASSERT(mJoueur->getCapital() > capitalAvant);
}

// ---------------------------------------------------------------------------
// action() : le montant recu doit etre exactement celui de l'attribut 'montant'
void TestCaseDepart::testActionMontantExact(void)
{
    int capitalAvant = mJoueur->getCapital();
    mCaseDepart->action();
    CPPUNIT_ASSERT(mJoueur->getCapital() == capitalAvant + mCaseDepart->getMontant());
}

// ---------------------------------------------------------------------------
// action() : sans joueur actif defini, le capital ne change pas
void TestCaseDepart::testActionSansEffetSiJoueurNePasse(void)
{
    mCaseDepart->setJoueurActif(nullptr);
    int capitalAvant = mJoueur->getCapital();
    mCaseDepart->action(); // ne doit pas planter ni modifier le capital
    CPPUNIT_ASSERT(mJoueur->getCapital() == capitalAvant);
}

// ---------------------------------------------------------------------------

CPPUNIT_TEST_SUITE_REGISTRATION(TestCaseDepart);

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

    ofstream xmlFileOut("cppTestCaseDepartResults.xml");
    XmlOutputter xmlOut(&collectedresults, xmlFileOut);
    xmlOut.write();

    return collectedresults.wasSuccessful() ? 0 : 1;
}
