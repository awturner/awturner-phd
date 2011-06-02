/*
* ################################################################################
* ### MIT License
* ################################################################################
* 
* Copyright (c) 2006-2011 Andy Turner
* 
* Permission is hereby granted, free of charge, to any person obtaining a copy
* of this software and associated documentation files (the "Software"), to deal
* in the Software without restriction, including without limitation the rights
* to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
* copies of the Software, and to permit persons to whom the Software is
* furnished to do so, subject to the following conditions:
* 
* The above copyright notice and this permission notice shall be included in
* all copies or substantial portions of the Software.
* 
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
* AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
* OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
* THE SOFTWARE.
*/
#include <cppunit/CompilerOutputter.h>
#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/TestResult.h>
#include <cppunit/TestResultCollector.h>
#include <cppunit/TestRunner.h>
#include <cppunit/BriefTestProgressListener.h>

int main (int argc, char* argv[])
{
   // informs test-listener about testresults
   CPPUNIT_NS :: TestResult testresult;

   // register listener for collecting the test-results
   CPPUNIT_NS :: TestResultCollector collectedresults;
   testresult.addListener (&collectedresults);

   // register listener for per-test progress output
   CPPUNIT_NS :: BriefTestProgressListener progress;
   testresult.addListener (&progress);

   // insert test-suite at test-runner by registry
   CPPUNIT_NS :: TestRunner testrunner;
   testrunner.addTest (CPPUNIT_NS :: TestFactoryRegistry :: getRegistry ().makeTest ());
   testrunner.run (testresult);

   // output results in compiler-format
   CPPUNIT_NS :: CompilerOutputter compileroutputter (&collectedresults, std::cerr);
   compileroutputter.write ();

   // return 0 if tests were successful
   return collectedresults.wasSuccessful () ? 0 : 1;
}