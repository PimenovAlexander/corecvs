#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/TestFixture.h>

class MatcherTest : public CppUnit::TestFixture  
{
CPPUNIT_TEST_SUITE( MatcherTest );
CPPUNIT_TEST(testTileMatching);
CPPUNIT_TEST(profileListMatching);
CPPUNIT_TEST(profileTileMatching);
CPPUNIT_TEST_SUITE_END(); 

public:
  void testTileMatching();
  void profileListMatching(); 
  void profileTileMatching();

}; 
