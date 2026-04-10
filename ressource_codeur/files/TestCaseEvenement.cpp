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
#include "CaseEvenement.hpp"
#include "Minijeu.hpp"
#include "Joueur.hpp"

using namespace CppUnit;
using namespace std;

class TestCaseEvenement : public CppUnit::TestFixture
{
    CPPUNIT_TEST_SUITE(TestCaseEvenement);
    CPPUNIT_TEST(testChoixMinijeuRetourneMinijeuValide);
    CPPUNIT_TEST(testDistributionGagnantRecupMise);
    CPPUNIT_TEST(testDistributionPerdantPerdMise);
    CPPUNIT_TEST(testActionDeclenche Minijeu);
    CPPUNIT_TEST_SUITE_END();

public:
    void setUp(void);
    void tearDown(void);

protected:
    // choixminijeu() : un minijeu valide est selectionne
    void testChoixMinijeuRetourneMinijeuValide(void);

    // distribution() : le gagnant recoit la mise
    void testDistributionGagnantRecupMise(void);

    // distribution() : le perdant perd la mise
    void testDistributionPerdantPerdMise(void);

    // action() : le minijeu est bien lance lors de l'atterrissage
    void testActionDeclencheMinijeu(void);

private:
    CaseEvenement *mCaseEvenement;
    Joueur        *mJoueur1;
    Joueur        *mJoueur2;
};

// ---------------------------------------------------------------------------

void TestCaseEvenement::setUp(void)
{
    mJoueur1      = new Joueur("Alice", 1500);
    mJoueur2      = new Joueur("Bob",   1500);
    mCaseEvenement = new CaseEvenement();
    mCaseEvenement->ajouterJoueur(mJoueur1);
    mCaseEvenement->ajouterJoueur(mJoueur2);
    mCaseEvenement->setMise(200);
}

void TestCaseEvenement::tearDown(void)
{
    delete mCaseEvenement;
    delete mJoueur1;
    delete mJoueur2;
}

// ---------------------------------------------------------------------------
// choixminijeu() : le minijeu renvoye ne doit pas etre null
void TestCaseEvenement::testChoixMinijeuRetourneMinijeuValide(void)
{
    Minijeu* jeu = mCaseEvenement->choixminijeu();
    CPPUNIT_ASSERT(jeu != nullptr);
}

// ---------------------------------------------------------------------------
// distribution() : le gagnant voit son capital augmenter de la mise totale
void TestCaseEvenement::testDistributionGagnantRecupMise(void)
{
    mCaseEvenement->setGagnant(mJoueur1);
    int capitalAvant = mJoueur1->getCapital();
    mCaseEvenement->distribution();
    CPPUNIT_ASSERT(mJoueur1->getCapital() > capitalAvant);
}

// distribution() : le perdant voit son capital diminuer
void TestCaseEvenement::testDistributionPerdantPerdMise(void)
{
    mCaseEvenement->setGagnant(mJoueur1);
    int capitalAvant = mJoueur2->getCapital();
    mCaseEvenement->distribution();
    CPPUNIT_ASSERT(mJoueur2->getCapital() < capitalAvant);
}

// ---------------------------------------------------------------------------
// action() : apres activation, un minijeu a bien eu lieu (flag ou compteur)
void TestCaseEvenement::testActionDeclencheMinijeu(void)
{
    CPPUNIT_ASSERT(!mCaseEvenement->minijeuJoue());
    mCaseEvenement->action();
    CPPUNIT_ASSERT(mCaseEvenement->minijeuJoue());
}

// ---------------------------------------------------------------------------

CPPUNIT_TEST_SUITE_REGISTRATION(TestCaseEvenement);

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

    ofstream xmlFileOut("cppTestCaseEvenementResults.xml");
    XmlOutputter xmlOut(&collectedresults, xmlFileOut);
    xmlOut.write();

    return collectedresults.wasSuccessful() ? 0 : 1;
}
