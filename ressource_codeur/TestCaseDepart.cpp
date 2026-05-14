/**
 * @file TestCaseDepart.cpp
 * @brief Tests unitaires de CaseDepart (CppUnit).
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
#include "CaseDepart.hpp"
#include "Joueur.hpp"

using namespace CppUnit;
using namespace std;

class TestCaseDepart : public CppUnit::TestFixture
{
    CPPUNIT_TEST_SUITE(TestCaseDepart);
    CPPUNIT_TEST(testActionDonneCapitalAuJoueur);          // [N] existant
    CPPUNIT_TEST(testActionMontantExact);                  // [N] existant
    CPPUNIT_TEST(testActionSansEffetSiJoueurNePasse);      // [E] existant
    CPPUNIT_TEST(testAction_JoueurCapitalNegatif);         // [L] nouveau
    CPPUNIT_TEST(testAction_PassageMultiple_CumulatifOK);  // [L] nouveau
    CPPUNIT_TEST_SUITE_END();

public:
    void setUp(void);
    void tearDown(void);

protected:
    /** [N] existant — capital augmente après passage. */
    void testActionDonneCapitalAuJoueur(void);
    /** [N] existant — montant reçu == getMontant(). */
    void testActionMontantExact(void);
    /** [E] existant — setJoueurActif(nullptr) → aucun changement, pas de crash. */
    void testActionSansEffetSiJoueurNePasse(void);
    /** [L] nouveau — joueur avec capital négatif reçoit quand même la prime. */
    void testAction_JoueurCapitalNegatif(void);
    /** [L] nouveau — deux passages successifs cumulent les primes. */
    void testAction_PassageMultiple_CumulatifOK(void);

private:
    CaseDepart *mCaseDepart;
    Joueur     *mJoueur;
};

void TestCaseDepart::setUp(void)
{
    mCaseDepart = new CaseDepart(200);
    mJoueur     = new Joueur("Alice", 1500);
    mCaseDepart->setJoueurActif(mJoueur);
}

void TestCaseDepart::tearDown(void)
{
    delete mCaseDepart;
    delete mJoueur;
}

// [N] existant
void TestCaseDepart::testActionDonneCapitalAuJoueur(void)
{
    int capitalAvant = mJoueur->getCapital();
    mCaseDepart->action();
    CPPUNIT_ASSERT(mJoueur->getCapital() > capitalAvant);
}

// [N] existant
void TestCaseDepart::testActionMontantExact(void)
{
    int capitalAvant = mJoueur->getCapital();
    mCaseDepart->action();
    CPPUNIT_ASSERT(mJoueur->getCapital() == capitalAvant + mCaseDepart->getMontant());
}

// [E] existant — nullptr ne plante pas et ne modifie pas le capital
void TestCaseDepart::testActionSansEffetSiJoueurNePasse(void)
{
    mCaseDepart->setJoueurActif(nullptr);
    int capitalAvant = mJoueur->getCapital();
    mCaseDepart->action();
    CPPUNIT_ASSERT(mJoueur->getCapital() == capitalAvant);
}

// [L] nouveau — joueur avec capital négatif : la prime s'applique quand même
void TestCaseDepart::testAction_JoueurCapitalNegatif(void)
{
    mJoueur->setCapital(-100);
    int capitalAvant = mJoueur->getCapital(); // -100
    mCaseDepart->action();
    CPPUNIT_ASSERT(mJoueur->getCapital() == capitalAvant + mCaseDepart->getMontant()); // 100
}

// [L] nouveau — deux passages = deux primes cumulées
void TestCaseDepart::testAction_PassageMultiple_CumulatifOK(void)
{
    int capitalAvant = mJoueur->getCapital();
    mCaseDepart->action();
    mCaseDepart->action();
    CPPUNIT_ASSERT(mJoueur->getCapital() == capitalAvant + 2 * mCaseDepart->getMontant());
}

CPPUNIT_TEST_SUITE_REGISTRATION(TestCaseDepart);


