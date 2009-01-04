//-----------------------------------------------------------------------------
// Copyright (C) 2007 Matt Lawrence
//
// File: XTest.cpp
//
// Description:
//
// Implementation of the XTest unit testing framework.
//----------------------------------------------------------------------------

#include "XTest.h"

#include <iostream>

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
XTestResult::XTestResult()
: mFailed(false) {
}

XTestResult::~XTestResult() {
}

void XTestResult::setFailed(bool Failed) {
   mFailed = Failed;
}

void XTestResult::setFile(const std::string& File) {
   mFile = File;
}

void XTestResult::setLine(unsigned int Line) {
   mLine = Line;
}

void XTestResult::setDescription(const std::string& Description) {
   mDescription = Description;
}

bool XTestResult::failed() const {
   return mFailed;
}

const std::string& XTestResult::file() const {
   return mFile;
}

unsigned int XTestResult::line() const {
   return mLine;
}

const std::string& XTestResult::description() const {
   return mDescription;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
XTest::XTest(const std::string& Collection, const std::string& Name)
   : mName(Name) {
   XTestSuite::instance().addTest(Collection, *this);
}

XTest::~XTest() {
}

XTestResult& XTest::result() {
   return mResult;
}

const XTestResult& XTest::result() const {
   return mResult;
}

const std::string& XTest::name() const {
   return mName;
}

void XTest::comparisonTestFailed (
      const std::string& File,
      unsigned int Line,
      const std::string& Expected,
      const std::string& Actual,
      const std::string& ExpectedLiteral,
      const std::string& ActualLiteral,
      const std::string& Operator) {
   result().setFailed(true);
   result().setFile(File);
   result().setLine(Line);

   std::ostringstream Description;
   Description << "Comparison check failed!" << std::endl
               << "Assert        : [" << ActualLiteral << " " << Operator << " " << ExpectedLiteral << "]" << std::endl
               << "Expected      : [" << Expected << "]" << std::endl
               << "Actual        : [" << Actual << "]";
   result().setDescription(Description.str());
}

void XTest::exceptionTypeTestFailed(
      const std::string& File,
      unsigned int Line,
      const std::string& Statement,
      const std::string& ExpectedType) {
   result().setFailed(true);
   result().setFile(File);
   result().setLine(Line);

   std::ostringstream Description;
   Description << "Statement threw incorrect exception type." << std::endl
               << "Statement     : [" << Statement << "]" << std::endl
               << "Expected Type : [" << ExpectedType << "]";
   result().setDescription(Description.str());
}

void XTest::exceptionDidntThrow(
      const std::string& File,
      unsigned int Line,
      const std::string& Statement) {
   result().setFailed(true);
   result().setFile(File);
   result().setLine(Line);

   std::ostringstream Description;
   Description << "Expected statement to throw but it didn't." << std::endl
               << "Statement     : [" << Statement << "]";
   result().setDescription(Description.str());
}

void XTest::exceptionShouldntThrow(
      const std::string& File,
      unsigned int Line,
      const std::string& Statement) {
   result().setFailed(true);
   result().setFile(File);
   result().setLine(Line);

   std::ostringstream Description;
   Description << "Expected statement to not throw but it did." << std::endl
               << "Statement     : [" << Statement << "]";
   result().setDescription(Description.str());
}

void XTest::exceptionComparisonTestFailed(
      const std::string& File,
      unsigned int Line,
      const std::string& Statement,
      const std::string& ExpectedLiteral,
      const std::string& ActualLiteral,
      const std::string& Expected,
      const std::string& Actual) {
   result().setFailed(true);
   result().setFile(File);
   result().setLine(Line);

   std::ostringstream Description;
   Description << "Exception comparison check failed!" << std::endl
               << "Statement     : [" << Statement << "]" << std::endl
               << "Assert        : [" << ActualLiteral << " == " << ExpectedLiteral << "]" << std::endl
               << "Expected      : [" << Expected << "]" << std::endl
               << "Actual        : [" << Actual << "]";
   result().setDescription(Description.str());
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
XTestCollection::XTestCollection(const std::string& Name, XTestSuite& Parent)
   : mName(Name), mpParent(&Parent) {
}

XTestCollection::~XTestCollection() {
}

void XTestCollection::addTest(XTest& Test) {
   mTests.push_back(&Test);
}

const std::string& XTestCollection::name() const {
   return mName;
}

unsigned int XTestCollection::countOfTest() const {
   return mTests.size();
}

XTest& XTestCollection::test(unsigned int TestIndex) {
   return *mTests[TestIndex];
}

const XTest& XTestCollection::test(unsigned int TestIndex) const {
   return *mTests[TestIndex];
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
typedef std::vector<XTestCollection*>::iterator TCollIter;

XTestSuite::XTestSuite(const std::string& Name)
   : mName(Name) {
   mpFormatter = new XTestMinimalFormatter(std::cout);
}

XTestSuite::~XTestSuite() {
   for(TCollIter Iter = mCollections.begin(); Iter != mCollections.end(); Iter++) {
      delete *Iter;
   }
   delete mpFormatter;
}

XTestSuite& XTestSuite::instance() {
   static XTestSuite Suite("Unit Tests");
   return Suite;
}

void XTestSuite::addTest(const std::string& Collection, XTest& Test) {   
   for(TCollIter Iter = mCollections.begin(); Iter != mCollections.end(); Iter++) {
      if((*Iter)->name() == Collection) {
         (*Iter)->addTest(Test);
         return;
      }
   }
   mCollections.push_back(new XTestCollection(Collection, *this));
   mCollections.back()->addTest(Test);
}

void XTestSuite::runAllTests() {
   mpFormatter->enterSuite(*this);
   for(unsigned int CollectionIndex = 0; CollectionIndex < countOfCollection(); CollectionIndex++) {
      XTestCollection& Collection = collection(CollectionIndex);

      mpFormatter->enterCollection(Collection);

      for(unsigned int TestIndex = 0; TestIndex < Collection.countOfTest(); TestIndex++) {
         XTest& Test = Collection.test(TestIndex);

         mpFormatter->enterTest(Test);

         try {
            Test.runTest();
         } catch(...) {
            Test.result().setFailed(true);
            Test.result().setFile("");
            Test.result().setLine(0);
            Test.result().setDescription("An unhandled exception occured.");
         }

         mpFormatter->exitTest(Test);
      }

      mpFormatter->exitCollection(Collection);
   }
   mpFormatter->exitSuite(*this);
}

unsigned int XTestSuite::countOfCollection() const {
   return mCollections.size();
}

XTestCollection& XTestSuite::collection(unsigned int CollectionIndex) {
   return *mCollections[CollectionIndex];
}

const XTestCollection& XTestSuite::collection(unsigned int CollectionIndex) const {
   return *mCollections[CollectionIndex];
}

const std::string& XTestSuite::name() const {
   return mName;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
XTestFormatter::XTestFormatter(std::ostream& Stream)
   : _mpStream(&Stream) {
}

XTestFormatter::~XTestFormatter() {
}

std::ostream& XTestFormatter::stream() {
   return *_mpStream;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
XTestMinimalFormatter::XTestMinimalFormatter(std::ostream& Stream)
  : XTestFormatter(Stream) {
}

XTestMinimalFormatter::~XTestMinimalFormatter() {
}

void XTestMinimalFormatter::enterTest(const XTest&) {
}

void XTestMinimalFormatter::exitTest(const XTest& Test) {
   if(Test.result().failed()) {
      stream() << "@";
   } else {
      stream() << ".";
   }

   stream() << std::flush;
}

void XTestMinimalFormatter::enterCollection(const XTestCollection&) {
}

void XTestMinimalFormatter::exitCollection(const XTestCollection&) {
}

void XTestMinimalFormatter::enterSuite(const XTestSuite&) {   
}

void XTestMinimalFormatter::exitSuite(const XTestSuite& Suite) {
   stream() << std::endl << std::endl;

   std::ostringstream SuiteStream;   
   unsigned int CountOfFailure = 0;
   unsigned int CountOfTest = 0;

   for(unsigned int CollectionIndex = 0; CollectionIndex < Suite.countOfCollection(); CollectionIndex++) {     
      const XTestCollection& Collection = Suite.collection(CollectionIndex);
      bool CollectionFailed = false;
      std::ostringstream CollectionStream;

      CountOfTest += Collection.countOfTest();

      for(unsigned int TestIndex = 0; TestIndex < Collection.countOfTest(); TestIndex++) {
         const XTest& Test = Collection.test(TestIndex);
         CollectionStream << "      [" << Collection.test(TestIndex).name() << ":";
         if(Test.result().failed()) {
            CountOfFailure++;
            CollectionFailed = true;            
            CollectionStream << "FAIL]" << std::endl
                             << std::endl
                             << Test.result().description() << std::endl
                             << "Location      : " << Test.result().file() << ":" << Test.result().line() << std::endl
                             << std::endl;
         } else {
            CollectionStream << "PASS]" << std::endl;
         }
      }

      SuiteStream << "   [" << Collection.name() << ":" << (CollectionFailed?"FAIL":"PASS") << "]" << std::endl;
      if(CollectionFailed) {
         SuiteStream << CollectionStream.str();
      }
   }

   stream() << "[" << Suite.name() << ":" << (CountOfFailure != 0?"FAIL":"PASS") << "]" << std::endl
            << SuiteStream.str() << std::endl
            << (CountOfTest - CountOfFailure) << "/" << CountOfTest << " tests passed (" << (100.0f - (float)CountOfFailure / (float)CountOfTest * 100) << "% success)" << std::endl;

}

