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
#include "CasePropriete.hpp"
#include "Joueur.hpp"

using namespace CppUnit;
using namespace std;

class TestCasePropriete : public CppUnit::TestFixture
{
    CPPUNIT_TEST_SUITE(TestCasePropriete);
    CPPUNIT_TEST(testActionSansProprietaireCaseAchetable);
    CPPUNIT_TEST(testActionAvecProprietaireLanceMinijeu);
    CPPUNIT_TEST(testRepartitionGagnantRecupGamble);
    CPPUNIT_TEST(testRepartitionPerdantPerdGamble);
    CPPUNIT_TEST(testGambleMontantPositif);
    CPPUNIT_TEST_SUITE_END();

public:
    void setUp(void);
    void tearDown(void);

protected:
    // action() sans proprietaire : la case est proposee a l'achat
    void testActionSansProprietaireCaseAchetable(void);

    // action() avec proprietaire : le minijeu est lance
    void testActionAvecProprietaireLanceMinijeu(void);

    // repartition() : le gagnant recupere la mise du gamble
    void testRepartitionGagnantRecupGamble(void);

    // repartition() : le perdant perd la mise du gamble
    void testRepartitionPerdantPerdGamble(void);

    // gamble() : le montant mise doit etre strictement positif
    void testGambleMontantPositif(void);

private:
    CasePropriete *mCase;
    Joueur        *mProprietaire;
    Joueur        *mVisiteur;
};

// ---------------------------------------------------------------------------

void TestCasePropriete::setUp(void)
{
    mProprietaire = new Joueur("Alice", 1500);
    mVisiteur     = new Joueur("Bob",   1500);
    mCase         = new CasePropriete("Avenue Mozart", 300);
    mCase->setJoueurActif(mVisiteur);
}

void TestCasePropriete::tearDown(void)
{
    delete mCase;
    delete mProprietaire;
    delete mVisiteur;
}

// ---------------------------------------------------------------------------
// action() sans proprietaire : la case doit signaler qu'elle est achetable
void TestCasePropriete::testActionSansProprietaireCaseAchetable(void)
{
    CPPUNIT_ASSERT(mCase->getProprietaire() == nullptr);
    mCase->action();
    // On verifie que la case a bien propose l'achat (flag interne)
    CPPUNIT_ASSERT(mCase->achatPropose());
}

// ---------------------------------------------------------------------------
// action() avec proprietaire : un minijeu doit etre marque comme lance
void TestCasePropriete::testActionAvecProprietaireLanceMinijeu(void)
{
    mCase->setProprietaire(mProprietaire);
    mCase->setGamble(100);
    mCase->action();
    CPPUNIT_ASSERT(mCase->minijeuLance());
}

// ---------------------------------------------------------------------------
// repartition() : le gagnant recupere le montant du gamble de l'adversaire
void TestCasePropriete::testRepartitionGagnantRecupGamble(void)
{
    mCase->setProprietaire(mProprietaire);
    mCase->setGamble(200);
    mCase->setGagnant(mProprietaire);
    int capitalAvant = mProprietaire->getCapital();
    mCase->repartition();
    CPPUNIT_ASSERT(mProprietaire->getCapital() == capitalAvant + 200);
}

// repartition() : le perdant perd le montant du gamble
void TestCasePropriete::testRepartitionPerdantPerdGamble(void)
{
    mCase->setProprietaire(mProprietaire);
    mCase->setGamble(200);
    mCase->setGagnant(mProprietaire);
    int capitalAvant = mVisiteur->getCapital();
    mCase->repartition();
    CPPUNIT_ASSERT(mVisiteur->getCapital() == capitalAvant - 200);
}

// ---------------------------------------------------------------------------
// gamble() : le montant place doit etre > 0
void TestCasePropriete::testGambleMontantPositif(void)
{
    mCase->setProprietaire(mProprietaire);
    mCase->setGamblePredefini(150);
    mCase->gamble();
    CPPUNIT_ASSERT(mCase->getGamble() > 0);
}

// ---------------------------------------------------------------------------

CPPUNIT_TEST_SUITE_REGISTRATION(TestCasePropriete);

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

    ofstream xmlFileOut("cppTestCaseProprieteResults.xml");
    XmlOutputter xmlOut(&collectedresults, xmlFileOut);
    xmlOut.write();

    return collectedresults.wasSuccessful() ? 0 : 1;
}
