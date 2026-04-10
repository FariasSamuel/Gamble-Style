#include <iostream>
#include <list>
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
#include "CaseAleatoire.hpp"
#include "Carte.hpp"
#include "Joueur.hpp"

using namespace CppUnit;
using namespace std;

class TestCaseAleatoire : public CppUnit::TestFixture
{
    CPPUNIT_TEST_SUITE(TestCaseAleatoire);
    CPPUNIT_TEST(testActionPiochePremiereCarte);
    CPPUNIT_TEST(testActionDeplacePremierCarteEnFin);
    CPPUNIT_TEST(testActionAppliqueEffetCarte);
    CPPUNIT_TEST_SUITE_END();

public:
    void setUp(void);
    void tearDown(void);

protected:
    // action() : la premiere carte de la liste est utilisee
    void testActionPiochePremiereCarte(void);

    // action() : apres utilisation, la carte est placee en fin de liste
    void testActionDeplacePremierCarteEnFin(void);

    // action() : l'effet de la carte est bien applique sur le joueur
    void testActionAppliqueEffetCarte(void);

private:
    CaseAleatoire *mCaseAleatoire;
    Joueur        *mJoueur;
    Carte         *mCarte1;
    Carte         *mCarte2;
};

// ---------------------------------------------------------------------------

void TestCaseAleatoire::setUp(void)
{
    mJoueur       = new Joueur("Alice", 1500);
    mCarte1       = new CarteDeplacement(mJoueur, nullptr, 5);  // deplace vers case 5
    mCarte2       = new CarteDeplacement(mJoueur, nullptr, 10); // deplace vers case 10
    mCaseAleatoire = new CaseAleatoire();
    mCaseAleatoire->ajouterCarte(mCarte1);
    mCaseAleatoire->ajouterCarte(mCarte2);
    mCaseAleatoire->setJoueurActif(mJoueur);
}

void TestCaseAleatoire::tearDown(void)
{
    delete mCaseAleatoire;
    delete mJoueur;
    delete mCarte1;
    delete mCarte2;
}

// ---------------------------------------------------------------------------
// action() : c'est bien la premiere carte qui est activee (pas une autre)
void TestCaseAleatoire::testActionPiochePremiereCarte(void)
{
    // Avant action(), la premiere carte envoie vers case 5
    mCaseAleatoire->action();
    CPPUNIT_ASSERT(mJoueur->getCaseActuelle()->getIndex() == 5);
}

// ---------------------------------------------------------------------------
// action() : apres activation, la carte utilisee se retrouve en derniere position
void TestCaseAleatoire::testActionDeplacePremierCarteEnFin(void)
{
    mCaseAleatoire->action(); // consomme mCarte1
    // La prochaine carte en tete de liste doit maintenant etre mCarte2
    Carte* prochaineEnTete = mCaseAleatoire->getPremiereCarteDeFile();
    CPPUNIT_ASSERT(prochaineEnTete == mCarte2);
}

// ---------------------------------------------------------------------------
// action() : apres un second appel, c'est mCarte2 qui s'applique (case 10)
void TestCaseAleatoire::testActionAppliqueEffetCarte(void)
{
    mCaseAleatoire->action(); // mCarte1 => case 5
    mCaseAleatoire->action(); // mCarte2 => case 10
    CPPUNIT_ASSERT(mJoueur->getCaseActuelle()->getIndex() == 10);
}

// ---------------------------------------------------------------------------

CPPUNIT_TEST_SUITE_REGISTRATION(TestCaseAleatoire);

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

    ofstream xmlFileOut("cppTestCaseAleatoireResults.xml");
    XmlOutputter xmlOut(&collectedresults, xmlFileOut);
    xmlOut.write();

    return collectedresults.wasSuccessful() ? 0 : 1;
}
