#include <iostream>
#include <string>
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
#include "Plateau.hpp"
#include "Joueur.hpp"

using namespace CppUnit;
using namespace std;

class TestPlateau : public CppUnit::TestFixture
{
    CPPUNIT_TEST_SUITE(TestPlateau);
    CPPUNIT_TEST(testDonnerCapitalAugmenteCapitalJoueur);
    CPPUNIT_TEST(testEnleverCapitalDiminueCapitalJoueur);
    CPPUNIT_TEST(testEnleverCapitalDeclenche Banqueroute);
    CPPUNIT_TEST(testTransfertArgentDebitEtCredit);
    CPPUNIT_TEST(testOrdreJoueursTrieParDe);
    CPPUNIT_TEST(testFinRetourneGagnantParCapitalPlusBiens);
    CPPUNIT_TEST_SUITE_END();

public:
    void setUp(void);
    void tearDown(void);

protected:
    // donnercapital(Joueur, montant)
    void testDonnerCapitalAugmenteCapitalJoueur(void);

    // enlevercapital(Joueur, montant)
    void testEnleverCapitalDiminueCapitalJoueur(void);
    void testEnleverCapitalDeclencheBanqueroute(void);

    // transfertargent(Joueur1, Joueur2, montant)
    void testTransfertArgentDebitEtCredit(void);

    // ordre()
    void testOrdreJoueursTrieParDe(void);

    // fin()
    void testFinRetourneGagnantParCapitalPlusBiens(void);

private:
    Plateau *mPlateau;
    Joueur  *mJoueur1;
    Joueur  *mJoueur2;
};

// ---------------------------------------------------------------------------

void TestPlateau::setUp(void)
{
    mPlateau = new Plateau();
    mJoueur1 = new Joueur("Alice", 1500);
    mJoueur2 = new Joueur("Bob",   500);
    mPlateau->ajouterJoueur(mJoueur1);
    mPlateau->ajouterJoueur(mJoueur2);
}

void TestPlateau::tearDown(void)
{
    delete mPlateau;
    delete mJoueur1;
    delete mJoueur2;
}

// ---------------------------------------------------------------------------
// donnercapital() : le capital du joueur doit augmenter du montant donne
void TestPlateau::testDonnerCapitalAugmenteCapitalJoueur(void)
{
    int capitalAvant = mJoueur1->getCapital();
    mPlateau->donnercapital(mJoueur1, 200);
    CPPUNIT_ASSERT(mJoueur1->getCapital() == capitalAvant + 200);
}

// ---------------------------------------------------------------------------
// enlevercapital() : le capital du joueur doit diminuer du montant retire
void TestPlateau::testEnleverCapitalDiminueCapitalJoueur(void)
{
    int capitalAvant = mJoueur1->getCapital();
    mPlateau->enlevercapital(mJoueur1, 300);
    CPPUNIT_ASSERT(mJoueur1->getCapital() == capitalAvant - 300);
}

// enlevercapital() : si le capital passe negatif, le joueur est en banqueroute
void TestPlateau::testEnleverCapitalDeclencheBanqueroute(void)
{
    mPlateau->enlevercapital(mJoueur2, 9999);
    CPPUNIT_ASSERT(mJoueur2->conditionfinanciere() != Condition::RICHE);
}

// ---------------------------------------------------------------------------
// transfertargent() : debit joueur1, credit joueur2 du meme montant
void TestPlateau::testTransfertArgentDebitEtCredit(void)
{
    int cap1Avant = mJoueur1->getCapital();
    int cap2Avant = mJoueur2->getCapital();
    mPlateau->transfertargent(mJoueur1, mJoueur2, 100);
    CPPUNIT_ASSERT(mJoueur1->getCapital() == cap1Avant - 100);
    CPPUNIT_ASSERT(mJoueur2->getCapital() == cap2Avant + 100);
}

// ---------------------------------------------------------------------------
// ordre() : le tableau joueurs doit etre trie par resultat de des (plus grand en premier)
void TestPlateau::testOrdreJoueursTrieParDe(void)
{
    mPlateau->ordre();
    // Apres tri, le premier joueur doit avoir un score de de >= au second
    CPPUNIT_ASSERT(mPlateau->getJoueur(0)->getDernierLancer()
                >= mPlateau->getJoueur(1)->getDernierLancer());
}

// ---------------------------------------------------------------------------
// fin() : le gagnant annonce doit etre celui avec le plus grand capital + biens
void TestPlateau::testFinRetourneGagnantParCapitalPlusBiens(void)
{
    // Alice a 1500 de capital, Bob 500 => Alice gagne
    Joueur* gagnant = mPlateau->fin();
    CPPUNIT_ASSERT(gagnant == mJoueur1);
}

// ---------------------------------------------------------------------------

CPPUNIT_TEST_SUITE_REGISTRATION(TestPlateau);

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

    ofstream xmlFileOut("cppTestPlateauResults.xml");
    XmlOutputter xmlOut(&collectedresults, xmlFileOut);
    xmlOut.write();

    return collectedresults.wasSuccessful() ? 0 : 1;
}
