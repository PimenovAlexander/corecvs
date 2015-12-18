#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/TestResult.h>
#include <cppunit/TestResultCollector.h>
#include <cppunit/TestRunner.h>
#include <cppunit/XmlOutputter.h>
#include "MatcherTest.h"
#include <stdexcept>
#include <fstream>

CPPUNIT_TEST_SUITE_REGISTRATION(MatcherTest);

int main( int argc, char **argv)
{
  std::string testPath = (argc > 1) ? std::string(argv[1]) : std::string("");

  // Create the event manager and test controller
  CPPUNIT_NS::TestResult controller;

  // Add a listener that colllects test result
  CPPUNIT_NS::TestResultCollector result;
  controller.addListener( &result );
  
  CPPUNIT_NS::TestRunner runner;
  CppUnit::TestFactoryRegistry &registry = CppUnit::TestFactoryRegistry::getRegistry();
  runner.addTest( registry.makeTest() );
  try
  {
    CPPUNIT_NS::stdCOut() << "Running "  <<  testPath << "\n";
    runner.run( controller, testPath );

    CPPUNIT_NS::stdCOut() << "\n";
  
    std::ofstream file( "tests.xml" );
    CPPUNIT_NS::XmlOutputter xml( &result, file );
    xml.setStyleSheet( "report.xsl" );
    xml.write();
    file.close();
  }
  catch ( std::invalid_argument &e )  // Test path not resolved
  {
    CPPUNIT_NS::stdCOut()  <<  "\n"  
                            <<  "ERROR: "  <<  e.what()
                            << "\n";
    return 0;
  }

  return result.wasSuccessful() ? 0 : 1;
} 
