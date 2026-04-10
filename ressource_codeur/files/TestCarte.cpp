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
#include "Carte.hpp"
#include "CarteAnniversaire.hpp"
#include "CarteDeplacement.hpp"
#include "CarteSortiePrison.hpp"
#include "Joueur.hpp"
#include "Plateau.hpp"

using namespace CppUnit;
using namespace std;

class TestCarte : public CppUnit::TestFixture
{
    CPPUNIT_TEST_SUITE(TestCarte);
    CPPUNIT_TEST(testActionCarteAnniversaireTransfertVersJoueur);
    CPPUNIT_TEST(testActionCarteDeplacementChangeLaCase);
    CPPUNIT_TEST(testActionCarteSortiePrisonDonneCarte);
    CPPUNIT_TEST_SUITE_END();

public:
    void setUp(void);
    void tearDown(void);

protected:
    // action() - carte anniversaire : transfert argent de tous les joueurs vers le titulaire
    void testActionCarteAnniversaireTransfertVersJoueur(void);

    // action() - carte deplacement : deplace le joueur sur la case indiquee
    void testActionCarteDeplacementChangeLaCase(void);

    // action() - carte sortie prison : le joueur possede desormais la carte
    void testActionCarteSortiePrisonDonneCarte(void);

private:
    Plateau           *mPlateau;
    Joueur            *mTitulaire;
    Joueur            *mAutreJoueur;
    CarteAnniversaire *mCarteAnniversaire;
    CarteDeplacement  *mCarteDeplacement;
    CarteSortiePrison *mCarteSortiePrison;
};

// ---------------------------------------------------------------------------

void TestCarte::setUp(void)
{
    mPlateau          = new Plateau();
    mTitulaire        = new Joueur("Alice", 1500);
    mAutreJoueur      = new Joueur("Bob",   1500);
    mPlateau->ajouterJoueur(mTitulaire);
    mPlateau->ajouterJoueur(mAutreJoueur);

    mCarteAnniversaire = new CarteAnniversaire(mTitulaire, mPlateau, 50);
    mCarteDeplacement  = new CarteDeplacement(mTitulaire, mPlateau, 10); // case index 10
    mCarteSortiePrison = new CarteSortiePrison(mTitulaire);
}

void TestCarte::tearDown(void)
{
    delete mCarteAnniversaire;
    delete mCarteDeplacement;
    delete mCarteSortiePrison;
    delete mPlateau;
    delete mTitulaire;
    delete mAutreJoueur;
}

// ---------------------------------------------------------------------------
// action() CarteAnniversaire : chaque autre joueur verse le montant au titulaire
void TestCarte::testActionCarteAnniversaireTransfertVersJoueur(void)
{
    int capitalTitulaireAvant   = mTitulaire->getCapital();
    int capitalAutreJoueurAvant = mAutreJoueur->getCapital();
    int montant = 50;

    mCarteAnniversaire->action();

    CPPUNIT_ASSERT(mTitulaire->getCapital()   == capitalTitulaireAvant   + montant);
    CPPUNIT_ASSERT(mAutreJoueur->getCapital() == capitalAutreJoueurAvant - montant);
}

// ---------------------------------------------------------------------------
// action() CarteDeplacement : le joueur se retrouve sur la bonne case
void TestCarte::testActionCarteDeplacementChangeLaCase(void)
{
    mCarteDeplacement->action();
    CPPUNIT_ASSERT(mTitulaire->getCaseActuelle()->getIndex() == 10);
}

// ---------------------------------------------------------------------------
// action() CarteSortiePrison : le joueur possede maintenant la carte
void TestCarte::testActionCarteSortiePrisonDonneCarte(void)
{
    CPPUNIT_ASSERT(!mTitulaire->possedeCarte(CarteSortiePrison::ID));
    mCarteSortiePrison->action();
    CPPUNIT_ASSERT(mTitulaire->possedeCarte(CarteSortiePrison::ID));
}

// ---------------------------------------------------------------------------

CPPUNIT_TEST_SUITE_REGISTRATION(TestCarte);

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

    ofstream xmlFileOut("cppTestCarteResults.xml");
    XmlOutputter xmlOut(&collectedresults, xmlFileOut);
    xmlOut.write();

    return collectedresults.wasSuccessful() ? 0 : 1;
}
