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
#include "Minijeu.hpp"
#include "Joueur.hpp"

using namespace CppUnit;
using namespace std;

class TestMinijeu : public CppUnit::TestFixture
{
    CPPUNIT_TEST_SUITE(TestMinijeu);
    // jouer(Joueur, Joueur)
    CPPUNIT_TEST(testJouer2JoueursRetourneUnGagnant);
    CPPUNIT_TEST(testJouer2JoueursGagnantEstUnDesDeuxJoueurs);
    // jouer(Joueur, Joueur, Joueur)
    CPPUNIT_TEST(testJouer3JoueursRetourne3Joueurs);
    CPPUNIT_TEST(testJouer3JoueursOrdreValide);
    // jouer(Joueur, Joueur, Joueur, Joueur)
    CPPUNIT_TEST(testJouer4JoueursRetourne4Joueurs);
    CPPUNIT_TEST(testJouer4JoueursOrdreValide);
    CPPUNIT_TEST_SUITE_END();

public:
    void setUp(void);
    void tearDown(void);

protected:
    // jouer(J, J) : renvoie exactement 1 joueur non null
    void testJouer2JoueursRetourneUnGagnant(void);

    // jouer(J, J) : le gagnant est l'un des deux joueurs passes en parametre
    void testJouer2JoueursGagnantEstUnDesDeuxJoueurs(void);

    // jouer(J, J, J) : le tableau renvoye contient bien 3 joueurs
    void testJouer3JoueursRetourne3Joueurs(void);

    // jouer(J, J, J) : chaque joueur apparait exactement une fois dans le tableau
    void testJouer3JoueursOrdreValide(void);

    // jouer(J, J, J, J) : le tableau renvoye contient bien 4 joueurs
    void testJouer4JoueursRetourne4Joueurs(void);

    // jouer(J, J, J, J) : chaque joueur apparait exactement une fois dans le tableau
    void testJouer4JoueursOrdreValide(void);

private:
    Minijeu *mMinijeu;
    Joueur  *mJoueur1;
    Joueur  *mJoueur2;
    Joueur  *mJoueur3;
    Joueur  *mJoueur4;
};

// ---------------------------------------------------------------------------

void TestMinijeu::setUp(void)
{
    mMinijeu = new Minijeu();
    mJoueur1 = new Joueur("Alice",   1500);
    mJoueur2 = new Joueur("Bob",     1500);
    mJoueur3 = new Joueur("Charlie", 1500);
    mJoueur4 = new Joueur("Diana",   1500);
}

void TestMinijeu::tearDown(void)
{
    delete mMinijeu;
    delete mJoueur1;
    delete mJoueur2;
    delete mJoueur3;
    delete mJoueur4;
}

// ---------------------------------------------------------------------------
// jouer(J1, J2) : le gagnant retourne ne doit pas etre null
void TestMinijeu::testJouer2JoueursRetourneUnGagnant(void)
{
    Joueur* gagnant = mMinijeu->jouer(mJoueur1, mJoueur2);
    CPPUNIT_ASSERT(gagnant != nullptr);
}

// jouer(J1, J2) : le gagnant doit etre l'un des deux joueurs
void TestMinijeu::testJouer2JoueursGagnantEstUnDesDeuxJoueurs(void)
{
    Joueur* gagnant = mMinijeu->jouer(mJoueur1, mJoueur2);
    CPPUNIT_ASSERT(gagnant == mJoueur1 || gagnant == mJoueur2);
}

// ---------------------------------------------------------------------------
// jouer(J1, J2, J3) : tableau de 3 elements non null
void TestMinijeu::testJouer3JoueursRetourne3Joueurs(void)
{
    Joueur** resultats = mMinijeu->jouer(mJoueur1, mJoueur2, mJoueur3);
    CPPUNIT_ASSERT(resultats[0] != nullptr);
    CPPUNIT_ASSERT(resultats[1] != nullptr);
    CPPUNIT_ASSERT(resultats[2] != nullptr);
}

// jouer(J1, J2, J3) : chaque joueur apparait exactement une fois (pas de doublons)
void TestMinijeu::testJouer3JoueursOrdreValide(void)
{
    Joueur** resultats = mMinijeu->jouer(mJoueur1, mJoueur2, mJoueur3);
    // Les trois slots doivent etre distincts
    CPPUNIT_ASSERT(resultats[0] != resultats[1]);
    CPPUNIT_ASSERT(resultats[0] != resultats[2]);
    CPPUNIT_ASSERT(resultats[1] != resultats[2]);
    // Chaque slot doit appartenir aux joueurs passes en entree
    bool j1Present = (resultats[0]==mJoueur1 || resultats[1]==mJoueur1 || resultats[2]==mJoueur1);
    bool j2Present = (resultats[0]==mJoueur2 || resultats[1]==mJoueur2 || resultats[2]==mJoueur2);
    bool j3Present = (resultats[0]==mJoueur3 || resultats[1]==mJoueur3 || resultats[2]==mJoueur3);
    CPPUNIT_ASSERT(j1Present && j2Present && j3Present);
}

// ---------------------------------------------------------------------------
// jouer(J1, J2, J3, J4) : tableau de 4 elements non null
void TestMinijeu::testJouer4JoueursRetourne4Joueurs(void)
{
    Joueur** resultats = mMinijeu->jouer(mJoueur1, mJoueur2, mJoueur3, mJoueur4);
    CPPUNIT_ASSERT(resultats[0] != nullptr);
    CPPUNIT_ASSERT(resultats[1] != nullptr);
    CPPUNIT_ASSERT(resultats[2] != nullptr);
    CPPUNIT_ASSERT(resultats[3] != nullptr);
}

// jouer(J1, J2, J3, J4) : chaque joueur apparait exactement une fois
void TestMinijeu::testJouer4JoueursOrdreValide(void)
{
    Joueur** resultats = mMinijeu->jouer(mJoueur1, mJoueur2, mJoueur3, mJoueur4);
    // Pas de doublons
    CPPUNIT_ASSERT(resultats[0] != resultats[1]);
    CPPUNIT_ASSERT(resultats[0] != resultats[2]);
    CPPUNIT_ASSERT(resultats[0] != resultats[3]);
    CPPUNIT_ASSERT(resultats[1] != resultats[2]);
    CPPUNIT_ASSERT(resultats[1] != resultats[3]);
    CPPUNIT_ASSERT(resultats[2] != resultats[3]);
    // Tous les joueurs presents
    bool j1Present = (resultats[0]==mJoueur1||resultats[1]==mJoueur1||resultats[2]==mJoueur1||resultats[3]==mJoueur1);
    bool j2Present = (resultats[0]==mJoueur2||resultats[1]==mJoueur2||resultats[2]==mJoueur2||resultats[3]==mJoueur2);
    bool j3Present = (resultats[0]==mJoueur3||resultats[1]==mJoueur3||resultats[2]==mJoueur3||resultats[3]==mJoueur3);
    bool j4Present = (resultats[0]==mJoueur4||resultats[1]==mJoueur4||resultats[2]==mJoueur4||resultats[3]==mJoueur4);
    CPPUNIT_ASSERT(j1Present && j2Present && j3Present && j4Present);
}

// ---------------------------------------------------------------------------

CPPUNIT_TEST_SUITE_REGISTRATION(TestMinijeu);

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

    ofstream xmlFileOut("cppTestMinijeuResults.xml");
    XmlOutputter xmlOut(&collectedresults, xmlFileOut);
    xmlOut.write();

    return collectedresults.wasSuccessful() ? 0 : 1;
}
