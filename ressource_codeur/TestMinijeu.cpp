/**
 * @file TestMinijeu.cpp
 * @brief Tests unitaires de la classe abstraite Minijeu (CppUnit).
 * @project GambleStyle — GM4 INSA Rouen Normandie
 */
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
    // ── jouer(J, J) ─────────────────────────────────────────────
    CPPUNIT_TEST(testJouer2JoueursRetourneUnGagnant);            // [N] existant
    CPPUNIT_TEST(testJouer2JoueursGagnantEstUnDesDeuxJoueurs);   // [N] existant
    CPPUNIT_TEST(testJouer2_AppelMultiple_ToujoursValide);       // [L] nouveau
    // ── jouer(J, J, J) ──────────────────────────────────────────
    CPPUNIT_TEST(testJouer3JoueursRetourne3Joueurs);             // [N] existant
    CPPUNIT_TEST(testJouer3JoueursOrdreValide);                  // [N] existant
    CPPUNIT_TEST(testJouer3_Premier_EstBienPresent);             // [L] nouveau
    // ── jouer(J, J, J, J) ───────────────────────────────────────
    CPPUNIT_TEST(testJouer4JoueursRetourne4Joueurs);             // [N] existant
    CPPUNIT_TEST(testJouer4JoueursOrdreValide);                  // [N] existant
    CPPUNIT_TEST(testJouer4_Premier_EstBienPresent);             // [L] nouveau
    // ── robustesse ──────────────────────────────────────────────
    CPPUNIT_TEST(testJouer2_NomNonNul);                         // [N] nouveau
    CPPUNIT_TEST_SUITE_END();

public:
    void setUp(void);
    void tearDown(void);

protected:
    // jouer(J, J)
    void testJouer2JoueursRetourneUnGagnant(void);
    void testJouer2JoueursGagnantEstUnDesDeuxJoueurs(void);
    void testJouer2_AppelMultiple_ToujoursValide(void);
    // jouer(J, J, J)
    void testJouer3JoueursRetourne3Joueurs(void);
    void testJouer3JoueursOrdreValide(void);
    void testJouer3_Premier_EstBienPresent(void);
    // jouer(J, J, J, J)
    void testJouer4JoueursRetourne4Joueurs(void);
    void testJouer4JoueursOrdreValide(void);
    void testJouer4_Premier_EstBienPresent(void);
    // robustesse
    void testJouer2_NomNonNul(void);

private:
    Minijeu *mMinijeu;
    Joueur  *mJoueur1;
    Joueur  *mJoueur2;
    Joueur  *mJoueur3;
    Joueur  *mJoueur4;
};

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

// ── jouer(J, J) ──────────────────────────────────────────────────────────────

// [N] existant — gagnant != nullptr
void TestMinijeu::testJouer2JoueursRetourneUnGagnant(void)
{
    Joueur* gagnant = mMinijeu->jouer(mJoueur1, mJoueur2);
    CPPUNIT_ASSERT(gagnant != nullptr);
}

// [N] existant — gagnant est l'un des deux joueurs passés
void TestMinijeu::testJouer2JoueursGagnantEstUnDesDeuxJoueurs(void)
{
    Joueur* gagnant = mMinijeu->jouer(mJoueur1, mJoueur2);
    CPPUNIT_ASSERT(gagnant == mJoueur1 || gagnant == mJoueur2);
}

// [L] nouveau — 10 appels successifs retournent chacun un gagnant valide
void TestMinijeu::testJouer2_AppelMultiple_ToujoursValide(void)
{
    for (int i = 0; i < 10; ++i)
    {
        Joueur* g = mMinijeu->jouer(mJoueur1, mJoueur2);
        CPPUNIT_ASSERT(g == mJoueur1 || g == mJoueur2);
    }
}

// ── jouer(J, J, J) ───────────────────────────────────────────────────────────

// [N] existant — tableau[3] entièrement non nul
void TestMinijeu::testJouer3JoueursRetourne3Joueurs(void)
{
    Joueur** r = mMinijeu->jouer(mJoueur1, mJoueur2, mJoueur3);
    CPPUNIT_ASSERT(r[0] != nullptr);
    CPPUNIT_ASSERT(r[1] != nullptr);
    CPPUNIT_ASSERT(r[2] != nullptr);
    delete[] r;
}

// [N] existant — aucun doublon, j1/j2/j3 tous présents
void TestMinijeu::testJouer3JoueursOrdreValide(void)
{
    Joueur** r = mMinijeu->jouer(mJoueur1, mJoueur2, mJoueur3);
    CPPUNIT_ASSERT(r[0] != r[1]);
    CPPUNIT_ASSERT(r[0] != r[2]);
    CPPUNIT_ASSERT(r[1] != r[2]);
    bool j1 = (r[0]==mJoueur1 || r[1]==mJoueur1 || r[2]==mJoueur1);
    bool j2 = (r[0]==mJoueur2 || r[1]==mJoueur2 || r[2]==mJoueur2);
    bool j3 = (r[0]==mJoueur3 || r[1]==mJoueur3 || r[2]==mJoueur3);
    CPPUNIT_ASSERT(j1 && j2 && j3);
    delete[] r;
}

// [L] nouveau — r[0] (le 1er classé) doit bien être l'un des 3 joueurs
void TestMinijeu::testJouer3_Premier_EstBienPresent(void)
{
    Joueur** r = mMinijeu->jouer(mJoueur1, mJoueur2, mJoueur3);
    bool premierEstParticipant =
        (r[0] == mJoueur1 || r[0] == mJoueur2 || r[0] == mJoueur3);
    CPPUNIT_ASSERT(premierEstParticipant);
    delete[] r;
}

// ── jouer(J, J, J, J) ────────────────────────────────────────────────────────

// [N] existant — tableau[4] entièrement non nul
void TestMinijeu::testJouer4JoueursRetourne4Joueurs(void)
{
    Joueur** r = mMinijeu->jouer(mJoueur1, mJoueur2, mJoueur3, mJoueur4);
    CPPUNIT_ASSERT(r[0] != nullptr);
    CPPUNIT_ASSERT(r[1] != nullptr);
    CPPUNIT_ASSERT(r[2] != nullptr);
    CPPUNIT_ASSERT(r[3] != nullptr);
    delete[] r;
}

// [N] existant — 6 paires distinctes, j1/j2/j3/j4 tous présents
void TestMinijeu::testJouer4JoueursOrdreValide(void)
{
    Joueur** r = mMinijeu->jouer(mJoueur1, mJoueur2, mJoueur3, mJoueur4);
    CPPUNIT_ASSERT(r[0]!=r[1]); CPPUNIT_ASSERT(r[0]!=r[2]);
    CPPUNIT_ASSERT(r[0]!=r[3]); CPPUNIT_ASSERT(r[1]!=r[2]);
    CPPUNIT_ASSERT(r[1]!=r[3]); CPPUNIT_ASSERT(r[2]!=r[3]);
    bool j1=(r[0]==mJoueur1||r[1]==mJoueur1||r[2]==mJoueur1||r[3]==mJoueur1);
    bool j2=(r[0]==mJoueur2||r[1]==mJoueur2||r[2]==mJoueur2||r[3]==mJoueur2);
    bool j3=(r[0]==mJoueur3||r[1]==mJoueur3||r[2]==mJoueur3||r[3]==mJoueur3);
    bool j4=(r[0]==mJoueur4||r[1]==mJoueur4||r[2]==mJoueur4||r[3]==mJoueur4);
    CPPUNIT_ASSERT(j1 && j2 && j3 && j4);
    delete[] r;
}

// [L] nouveau — r[0] est l'un des 4 participants
void TestMinijeu::testJouer4_Premier_EstBienPresent(void)
{
    Joueur** r = mMinijeu->jouer(mJoueur1, mJoueur2, mJoueur3, mJoueur4);
    bool premierEstParticipant =
        (r[0]==mJoueur1||r[0]==mJoueur2||r[0]==mJoueur3||r[0]==mJoueur4);
    CPPUNIT_ASSERT(premierEstParticipant);
    delete[] r;
}

// ── robustesse ────────────────────────────────────────────────────────────────

// [N] nouveau — getNom() retourne une chaîne non nulle et non vide
void TestMinijeu::testJouer2_NomNonNul(void)
{
    const char* nom = mMinijeu->getNom();
    CPPUNIT_ASSERT(nom != nullptr);
    CPPUNIT_ASSERT(nom[0] != '\0');
}

CPPUNIT_TEST_SUITE_REGISTRATION(TestMinijeu);


